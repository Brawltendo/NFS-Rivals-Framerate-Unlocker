#include <Engine/Entity/MiscEntities.h>
#include <Engine/Entity/MiscEntityData.h>
#include <Engine/Entity/EntityFactory.h>

#include <Engine/Entity/UpdateInfo.h>
#include <Engine/Entity/UpdateManager.h>
#include <Engine/Entity/SimpleUpdater.h>

#include <Engine/Core/Memory/MemoryArena.h>

#include <../util/memoryutils.h>
#include "MinHook.h"


namespace
{

const int fieldhash_Out = 0xB87DF4B;
const int fieldhash_InterpolatedOut = 0x1DD6C2C8;

}

namespace fb
{

// another useless updater setup
// the weapon transform that's passed into this class does some weird stuff
// it only seems to interpolate properly for AI targets, while the player transform slingshots

TransformMultiplierEntityUpdater* TransformMultiplierEntity::s_updater = nullptr;
class TransformMultiplierEntityUpdater : public SimpleUpdaterWithAutoDelete<TransformMultiplierEntity>
{
public:
	TransformMultiplierEntityUpdater(Updater** updater)
		: SimpleUpdaterWithAutoDelete<TransformMultiplierEntity>(Realm_Client, UpdatePass_PostSim, "TransformMultiplierEntityUpdater", 1, updater)
		, m_pass2(UpdatePass_PostFrame)
	{
		g_updateManagers[m_realm].manager[m_pass2].addUpdater(this);
		__vftable = (void**)&s_vftable;
	}

	// have to do a non virtual override because of the vtable situation
	void dtor(bool d)
	{
		g_updateManagers[m_realm].manager[m_pass2].removeUpdater(this);
		SimpleUpdaterWithAutoDelete<TransformMultiplierEntity>::dtor(d);
	}

private:
	UpdatePass m_pass2;
	// gross but it must be done
	static void* s_vftable[6];
};

auto dtorConv = &TransformMultiplierEntityUpdater::dtor;
auto preQueryUpdateConv = &TransformMultiplierEntityUpdater::preQueryUpdate;
auto postQueryUpdateConv = &TransformMultiplierEntityUpdater::postQueryUpdate;
void* TransformMultiplierEntityUpdater::s_vftable[6] =
{
	*(void**)&dtorConv,
	(void*)0x1400A8390, // preQueryUpdate
	*(void**)&postQueryUpdateConv,
	(void*)0x140169F90,
	(void*)0x140813A80, // exitLevel
	(void*)0x140169F90  // resetTicksTilNextUpdate
};


TransformMultiplierEntity::t_EntityCreator_create_Client TransformMultiplierEntity::o_EntityCreator_create_Client = nullptr;
TransformMultiplierEntity* TransformMultiplierEntity::EntityCreator_create_Client(DefaultEntityCreator* creator, EntityFactoryParams& params)
{
	//TransformMultiplierEntity* entity = o_EntityCreator_create_Client(creator, params);
	if (params.wantsGhost != params.data->getIsGhost())
		return nullptr;

	auto EntityBus_getArena = reinterpret_cast<const MemoryArena&(__fastcall*)(EntityBus*)>(0x14023D7F0);
	
	void* memory = EntityBus_getArena(params.base.entityBus).alloc(sizeof(TransformMultiplierEntity), alignof(TransformMultiplierEntity));
	TransformMultiplierEntity* entity = new (memory) TransformMultiplierEntity(params.base, static_cast<const TransformMultiplierEntityData*>(params.data));

	auto internalCreate = reinterpret_cast<void(__fastcall*)(EntityFactoryParams&, void*)>(0x14020EA20);
	internalCreate(params, entity);
	return entity;
}

TransformMultiplierEntity::t_EntityCreator_create_Server TransformMultiplierEntity::o_EntityCreator_create_Server = nullptr;
TransformMultiplierEntity* TransformMultiplierEntity::EntityCreator_create_Server(DefaultEntityCreator* creator, EntityFactoryParams& params)
{
	//TransformMultiplierEntity* entity = o_EntityCreator_create_Server(creator, params);
	
	auto EntityBus_getArena = reinterpret_cast<const MemoryArena & (__fastcall*)(EntityBus*)>(0x14023D7F0);

	void* memory = EntityBus_getArena(params.base.entityBus).alloc(sizeof(TransformMultiplierEntity), alignof(TransformMultiplierEntity));
	TransformMultiplierEntity* entity = new (memory) TransformMultiplierEntity(params.base, static_cast<const TransformMultiplierEntityData*>(params.data));

	auto internalCreate = reinterpret_cast<void(__fastcall*)(EntityFactoryParams&, void*)>(0x14020EA20);
	internalCreate(params, entity);
	return entity;
}

TransformMultiplierEntity::t_onCreate TransformMultiplierEntity::o_onCreate = nullptr;
void TransformMultiplierEntity::onCreate(const EntityCreationInfo& info)
{
	auto EntityBus_internalCreateFieldOverride = reinterpret_cast<PropertyWriterBase*(__fastcall*)(void*, PropertyWriterBase*, DataContainer*, int, uintptr_t, void*)>(0x14023FC00);
	o_onCreate(this, info);

	// Since the blueprint logic has to be set up in Frosty, and it doesn't preserve the 
	// value of Vec3's pad field, we're gonna get hacky and use the trans.z value straight
	// from the entity data to get the magic value.
	// This allows for an external value to be passed in and still override this, while  
	// working around Frosty's limitations.
	const LinearTransform& in1 = m_data->getIn1();
	u32 transZ = in1.trans.simdValue.m128_u32[2];
	// This is genuinely garbage but only specific instances should be opted in for updating 
	// in order to avoid adding unnecessary overhead every frame from this VERY common entity.
	// Unfortunately, this is the only way I could come up with without going through the 
	// massive amount of work to implement a custom Frostbite RTTI class just for this scenario, 
	// so instead we're just gonna check for a magic value and pray.
	if (transZ == 0xBADBADAF)
	{
		// m_interpolatedOut won't be initialized without passing in a value
		LinearTransform ident(LinearTransform::IdentityType::Identity);
		EntityBus_internalCreateFieldOverride(info.entityBus, &m_interpolatedOut, m_data, fieldhash_InterpolatedOut, 0x1419F5640, &ident);
	}
}

TransformMultiplierEntity::t_onInit TransformMultiplierEntity::o_onInit = nullptr;
void TransformMultiplierEntity::onInit(const EntityInitInfo& info)
{
	o_onInit(this, info);

	if (m_interpolatedOut.hasOutgoingConnections())
	{
		// we only wanna create this if InterpolatedOut is hooked up to another Schematics node
		if (s_updater == nullptr)
		{
			s_updater = new (*s_levelArena) TransformMultiplierEntityUpdater((Updater**)&s_updater);
		}

		s_updater->addInstance(this);
	}
}

void TransformMultiplierEntity::onDeinit(const EntityDeinitInfo& info)
{
	auto base_onDeinit = reinterpret_cast<void(__fastcall*)(void*, const EntityDeinitInfo&)>(0x140221FC0);

	if (m_interpolatedOut.hasOutgoingConnections())
	{
		s_updater->removeInstance(this);
	}

	base_onDeinit(this, info);
}

TransformMultiplierEntity::t_writeOut TransformMultiplierEntity::o_writeOut = nullptr;
void TransformMultiplierEntity::writeOut()
{

	if ( (m_in1.hasConnection() && !m_in1.hasConnectionValue()) || (m_in2.hasConnection() && !m_in2.hasConnectionValue()) )
	{
		return;
	}

	const LinearTransform& in1 = m_in1.get();
	const LinearTransform& in2 = m_in2.get();
	LinearTransform mul = in1 * in2;
	// world space
	if (in1.trans.simdValue.m128_u32[3] == 0xAFAFAFAF || in2.trans.simdValue.m128_u32[3] == 0xAFAFAFAF)
	{
		mul.trans.simdValue.m128_u32[3] = 0xAFAFAFAF;
	}
	m_out = mul;

	// only bother with this if InterpolatedOut is connected
	if (m_interpolatedOut.hasOutgoingConnections())
	{
		m_outCache = mul;
		/*m_prevPosition = m_currentPosition;
		m_prevOrientation = m_currentOrientation;
		m_currentPosition = m_outCache.trans;
		m_currentOrientation = Quat(m_outCache);*/
	}
}

void TransformMultiplierEntity::update(const UpdateInfo& info)
{
	// only update TransformMultiplierEntities that opt into interpolation
	if (!m_interpolatedOut.hasOutgoingConnections())
	{
		return;
	}

	if (info.updatePass == UpdatePass_PostSim)
	{
		m_prevPosition = m_currentPosition;
		m_prevOrientation = m_currentOrientation;
		m_currentPosition = m_outCache.trans;
		m_currentOrientation = Quat(m_outCache);
	}

	Vec interpPosition = VecLerp(m_prevPosition, m_currentPosition, info.interpolationFactor);
	Quat interpOrientation = slerp(m_prevOrientation, m_currentOrientation, info.interpolationFactor);
	LinearTransform interpT(interpOrientation, interpPosition);
	if (interpT != m_outCache)
		m_interpolatedOut = interpT;
}

void TransformMultiplierEntity::ApplyPatches()
{
	// increase size of fb::TransformMultiplierEntity from 0x58 to 0xE0
	// with this we can add extra fields for interpolation
	{
		uint8_t patch1[] = { 0xE0 };
		//PatchBytes(0x1402181F2, patch1, sizeof(patch1));
		uint8_t patch2[] = { 0xE0 };
		//PatchBytes(0x140218139, patch2, sizeof(patch2));
		uint8_t patch3[] = { 0xE0 };
		PatchBytes(0x141F37732, patch3, sizeof(patch3));
	}

	// override fb::TransformMultiplierEntity Client EntityCreator
	{
		auto f = &TransformMultiplierEntity::EntityCreator_create_Client;
		TransformMultiplierEntity::t_EntityCreator_create_Client target = reinterpret_cast<TransformMultiplierEntity::t_EntityCreator_create_Client>(0x1402181B0);
		MH_CreateHook(target, reinterpret_cast<LPVOID>(f), reinterpret_cast<LPVOID*>(&TransformMultiplierEntity::o_EntityCreator_create_Client)); MH_EnableHook(target);
	}

	// override fb::TransformMultiplierEntity Server EntityCreator
	{
		auto f = &TransformMultiplierEntity::EntityCreator_create_Server;
		TransformMultiplierEntity::t_EntityCreator_create_Server target = reinterpret_cast<TransformMultiplierEntity::t_EntityCreator_create_Server>(0x140218110);
		MH_CreateHook(target, reinterpret_cast<LPVOID>(f), reinterpret_cast<LPVOID*>(&TransformMultiplierEntity::o_EntityCreator_create_Server)); MH_EnableHook(target);
	}

	// override fb::TransformMultiplierEntity::onCreate
	{
		auto f = &TransformMultiplierEntity::onCreate;
		TransformMultiplierEntity::t_onCreate target = reinterpret_cast<TransformMultiplierEntity::t_onCreate>(0x140221200);
		MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&TransformMultiplierEntity::o_onCreate)); MH_EnableHook(target);
	}

	// override fb::TransformMultiplierEntity::onInit
	{
		auto f = &TransformMultiplierEntity::onInit;
		TransformMultiplierEntity::t_onInit target = reinterpret_cast<TransformMultiplierEntity::t_onInit>(0x1402233F0);
		MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&TransformMultiplierEntity::o_onInit)); MH_EnableHook(target);
	}

	// redirect fb::TransformMultiplierEntity::onDeinit vfptr to our patched version
	{
		auto f = &TransformMultiplierEntity::onDeinit;
		union { void* ptr; uint8_t arr[8]; } patch{};
		patch.ptr = *(void**)&f;
		PatchBytes(0x141412D60, patch.arr, sizeof(patch));
	}

	// override fb::TransformMultiplierEntity::writeOut
	{
		auto f = &TransformMultiplierEntity::writeOut;
		TransformMultiplierEntity::t_writeOut target = reinterpret_cast<TransformMultiplierEntity::t_writeOut>(0x140229F90);
		MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&TransformMultiplierEntity::o_writeOut)); MH_EnableHook(target);
	}
}

}