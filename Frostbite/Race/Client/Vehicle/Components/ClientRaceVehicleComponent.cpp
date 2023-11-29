#include "ClientRaceVehicleComponent.h"

#include <Engine/Entity/UpdateInfo.h>
#include <Engine/Entity/SimpleUpdater.h>
#include <Engine/Entity/UpdateManager.h>

#include <Engine/Core/Memory/MemoryArena.h>
#include <Engine/Core/Math/Math.h>

#include <Engine/Physics/Vehicle/EAGR/IRaceVehicle.h>
#include <Race/Client/Vehicle/Components/ClientRaceVehicleComponentSimulation.h>

#include "../../../../../util/memoryutils.h"
#include "MinHook.h"


namespace
{

const int fieldhash_InterpolatedTransform = 0xDCAE85AA;
const int fieldhash_InterpolatedRpm = 0x1DD6F529;

}

namespace fb
{

ClientRaceVehicleComponentUpdater* ClientRaceVehicleComponent::s_updater = nullptr;
// ended up not needing any of this because it could be done with the existing component update functions
// originally the plan was to backport the functionality of updateTransformFromXxxx from later Frostbite versions
class ClientRaceVehicleComponentUpdater : public SimpleUpdaterWithAutoDelete<ClientRaceVehicleComponent>
{
public:
	ClientRaceVehicleComponentUpdater(Updater** updater)
		: SimpleUpdaterWithAutoDelete<ClientRaceVehicleComponent>(Realm_Client, UpdatePass_PostSim, "ClientRaceVehicleComponentUpdater", 10000, updater)
		, m_pass2(UpdatePass_FrameInterpolation)
	{
		g_updateManagers[m_realm].manager[m_pass2].addUpdater(this);
		__vftable = (void**)&s_vftable;
	}

	void dtor(bool d)
	{
		g_updateManagers[m_realm].manager[m_pass2].removeUpdater(this);
		SimpleUpdaterWithAutoDelete<ClientRaceVehicleComponent>::dtor(d);
	}

	void* preQueryUpdate(const UpdateInfo& info, UpdateQueryCollector& collector)
	{
		/*flushCommandBuffer();
		ClientRaceVehicleComponent** it = getInstances().mpBegin;
		ClientRaceVehicleComponent** end = getInstances().mpEnd;
		for (; it != end; ++it)
			(*it)->update(info);*/
		return nullptr;
	}

private:
	UpdatePass m_pass2;
	// gross but it must be done
	static void* s_vftable[6];
};

auto dtorConv = &ClientRaceVehicleComponentUpdater::dtor;
auto preQueryUpdateConv = &ClientRaceVehicleComponentUpdater::preQueryUpdate;
auto postQueryUpdateConv = &ClientRaceVehicleComponentUpdater::postQueryUpdate;
void* ClientRaceVehicleComponentUpdater::s_vftable[6] =
{
	*(void**)&dtorConv,
	(void*)0x1400A8390, // preQueryUpdate
	*(void**)&postQueryUpdateConv,
	(void*)0x140169F90,
	(void*)0x140813A80, // exitLevel
	(void*)0x140169F90  // resetTicksTilNextUpdate
};


ClientRaceVehicleComponent::t_ctor ClientRaceVehicleComponent::o_ctor = nullptr;
ClientRaceVehicleComponent* ClientRaceVehicleComponent::ctor(CreationInfo* info, const RaceVehicleComponentData& data)
{
	auto createFieldOverride_LinearTransform = reinterpret_cast<void(__fastcall*)(void*, ProxyPropertyWriter<LinearTransform>*, void*, const void*, PropertyId, LinearTransform*)>(0x140816630);
	auto createFieldOverride_float = reinterpret_cast<void(__fastcall*)(void*, ProxyPropertyWriter<float>*, void*, const void*, PropertyId, float*)>(0x140816310);
	
	ClientRaceVehicleComponent* ret = o_ctor(this, info, data);
	m_vehicleTransformHistory = TransformHistory();
	m_vehicleBodyTransformHistory = TransformHistory();
	m_prevRpm = 0.f;
	m_currentRpm = 0.f;

	m_interpolatedTransform = ProxyPropertyWriter<LinearTransform>();
	LinearTransform t(LinearTransform::IdentityType::Identity);
	createFieldOverride_LinearTransform(this, &m_interpolatedTransform, *reinterpret_cast<void**>(info), &data, fieldhash_InterpolatedTransform, &t);
	
	m_interpolatedRpm = ProxyPropertyWriter<float>();
	float f = 0.f;
	createFieldOverride_float(this, &m_interpolatedRpm, *reinterpret_cast<void**>(info), &data, fieldhash_InterpolatedRpm, &f);

	m_interpolatedBodyTransform = LinearTransform(LinearTransform::IdentityType::Identity);
	return ret;
}

ClientRaceVehicleComponent::t_onDeinit ClientRaceVehicleComponent::o_onDeinit = nullptr;
void ClientRaceVehicleComponent::onDeinit()
{
	s_updater->removeInstance(this);
	o_onDeinit(this);
}

ClientRaceVehicleComponent::t_onInit ClientRaceVehicleComponent::o_onInit = nullptr;
void ClientRaceVehicleComponent::onInit(const ClientComponentInitInfo& info)
{
	o_onInit(this, info);

	if (s_updater == nullptr)
		s_updater = new (*s_levelArena) ClientRaceVehicleComponentUpdater((Updater**)&s_updater);

	s_updater->addInstance(this);
}

void ClientRaceVehicleComponent::applyInterpolation(float interpolationFactor)
{
	Vec interpPosition = VecLerp(m_vehicleTransformHistory.m_prevPosition, m_vehicleTransformHistory.m_currentPosition, interpolationFactor);
	Quat interpOrientation = slerp(m_vehicleTransformHistory.m_prevOrientation, m_vehicleTransformHistory.m_currentOrientation, interpolationFactor);
	LinearTransform interpT(interpOrientation, interpPosition);
	if (interpT != m_raceVehicleTransformCache)
		m_interpolatedTransform = interpT;

	float interpRpm = lerp(m_prevRpm, m_currentRpm, interpolationFactor);
	if (interpRpm != m_currentRpm)
		m_interpolatedRpm = interpRpm;

	interpPosition = VecLerp(m_vehicleBodyTransformHistory.m_prevPosition, m_vehicleBodyTransformHistory.m_currentPosition, interpolationFactor);
	interpOrientation = slerp(m_vehicleBodyTransformHistory.m_prevOrientation, m_vehicleBodyTransformHistory.m_currentOrientation, interpolationFactor);
	interpT = LinearTransform(interpOrientation, interpPosition);
	if (interpT != m_raceVehicleBodyTransform.get())
		m_interpolatedBodyTransform = interpT;
}

ClientRaceVehicleComponent::t_onPostPhysicsUpdateSync ClientRaceVehicleComponent::o_onPostPhysicsUpdateSync = nullptr;
void ClientRaceVehicleComponent::onPostPhysicsUpdateSync(const EntityUpdateInfo& updateInfo)
{	
	o_onPostPhysicsUpdateSync(this, updateInfo);

	m_vehicleTransformHistory.m_prevPosition = m_vehicleTransformHistory.m_currentPosition;
	m_vehicleTransformHistory.m_currentPosition = m_raceVehicleTransformCache.trans;
	m_vehicleTransformHistory.m_prevOrientation = m_vehicleTransformHistory.m_currentOrientation;
	m_vehicleTransformHistory.m_currentOrientation = Quat(m_raceVehicleTransformCache);

	m_vehicleBodyTransformHistory.m_prevPosition = m_vehicleBodyTransformHistory.m_currentPosition;
	m_vehicleBodyTransformHistory.m_currentPosition = m_raceVehicleBodyTransform.get().trans;
	m_vehicleBodyTransformHistory.m_prevOrientation = m_vehicleBodyTransformHistory.m_currentOrientation;
	m_vehicleBodyTransformHistory.m_currentOrientation = Quat(m_raceVehicleBodyTransform.get());

	m_prevRpm = m_currentRpm;
	if (m_vehiclePhysics)
	{
		m_currentRpm = m_vehiclePhysics->rpm();
	}

	// interpolate values and expose them to Schematics
	applyInterpolation(updateInfo.interpolationFactor);
}

ClientRaceVehicleComponent::t_onFrameInterpolationUpdate ClientRaceVehicleComponent::o_onFrameInterpolationUpdate = nullptr;
void ClientRaceVehicleComponent::onFrameInterpolationUpdate(const EntityUpdateInfo& updateInfo)
{
	o_onFrameInterpolationUpdate(this, updateInfo);

	// interpolate values and expose them to Schematics
	applyInterpolation(updateInfo.interpolationFactor);
}

void ClientRaceVehicleComponent::updaterPostSimUpdate(const UpdateInfo& info)
{
	m_vehicleTransformHistory.m_prevPosition = m_vehicleTransformHistory.m_currentPosition;
	m_vehicleTransformHistory.m_currentPosition = m_raceVehicleTransformCache.trans;
	m_vehicleTransformHistory.m_prevOrientation = m_vehicleTransformHistory.m_currentOrientation;
	m_vehicleTransformHistory.m_currentOrientation = Quat(m_raceVehicleTransformCache);

	m_prevRpm = m_currentRpm;
	if (m_vehiclePhysics)
	{
		m_currentRpm = m_vehiclePhysics->rpm();
	}

	// interpolate values and expose them to Schematics
	applyInterpolation(info.interpolationFactor);
}

void ClientRaceVehicleComponent::updaterFrameInterpolationUpdate(const UpdateInfo& info)
{
	// interpolate values and expose them to Schematics
	applyInterpolation(info.interpolationFactor);
}

void ClientRaceVehicleComponent::update(const UpdateInfo& info)
{
	if (info.updatePass == UpdatePass_PostSim)
	{
		updaterPostSimUpdate(info);
	}
	else if (info.updatePass == UpdatePass_FrameInterpolation)
	{
		updaterFrameInterpolationUpdate(info);
	}
}

void ClientRaceVehicleComponent::ApplyPatches()
{
	// increase size of fb::ClientRaceVehicleComponent from 0x13D0 to 0x1530
	// with this we can add extra fields for interpolation
	{
		uint8_t patch1[] = { 0x30, 0x15 };
		PatchBytes(0x141FA9D82, patch1, sizeof(patch1));
		uint8_t patch2[] = { 0x30, 0x15 };
		PatchBytes(0x14081CD5A, patch2, sizeof(patch2));
	}

	// override fb::ClientRaceVehicleComponent constructor
	{
		auto f = &ClientRaceVehicleComponent::ctor;
		ClientRaceVehicleComponent::t_ctor target = reinterpret_cast<ClientRaceVehicleComponent::t_ctor>(0x1408184B0);
		MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&ClientRaceVehicleComponent::o_ctor)); MH_EnableHook(target);
	}

	// override fb::ClientRaceVehicleComponent::onPostPhysicsUpdateSync
	{
		auto f = &ClientRaceVehicleComponent::onPostPhysicsUpdateSync;
		ClientRaceVehicleComponent::t_onPostPhysicsUpdateSync target = reinterpret_cast<ClientRaceVehicleComponent::t_onPostPhysicsUpdateSync>(0x140822400);
		MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&ClientRaceVehicleComponent::o_onPostPhysicsUpdateSync)); MH_EnableHook(target);
	}

	// override fb::ClientRaceVehicleComponent::onFrameInterpolationUpdate
	{
		auto f = &ClientRaceVehicleComponent::onFrameInterpolationUpdate;
		ClientRaceVehicleComponent::t_onFrameInterpolationUpdate target = reinterpret_cast<ClientRaceVehicleComponent::t_onFrameInterpolationUpdate>(0x1408202E0);
		MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&ClientRaceVehicleComponent::o_onFrameInterpolationUpdate)); MH_EnableHook(target);
	}

	// redirect fb::ClientRaceVehicleComponentPrediction::worldTransform vfptr to fb::ClientChassisComponentPrediction::worldTransform
	{
		uint8_t patch[] = { 0xB0, 0xC1, 0x2E, 0x40, 0x01, 0x00, 0x00, 0x00 };
		PatchBytes(0x141542188, patch, sizeof(patch));
	}
}

}