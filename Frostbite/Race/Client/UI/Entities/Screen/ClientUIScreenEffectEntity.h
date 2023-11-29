#pragma once
#include <Engine/Core/Reflection/PropertyWriter.h>
#include <Engine/Core/Math/Vec3.h>


namespace fb
{

struct UpdateInfo;
class ClientRaceVehicleEntityProxy;
struct CreationInfo;
class UIScreenEffectEntityData;
class EntityInitInfo;
class ClientUIScreenEffectEntityUpdater;


class ClientUIScreenEffectEntity
{
public:
	static void ApplyPatches();

	// fake constructor
	ClientUIScreenEffectEntity* ctor(CreationInfo* info, const UIScreenEffectEntityData& data);

	void onInit(const EntityInitInfo& info);
	void update(const UpdateInfo& info);

	typedef ClientUIScreenEffectEntity* (__fastcall* t_ctor)(ClientUIScreenEffectEntity*, CreationInfo*, const UIScreenEffectEntityData&);
	static t_ctor o_ctor;

	typedef void(__fastcall* t_onInit)(ClientUIScreenEffectEntity*, const EntityInitInfo&);
	static t_onInit o_onInit;

protected:
	static ClientUIScreenEffectEntityUpdater** s_updater;

	void** __vftable;

private:
	char gap8[24];
	void* m_entityBus;
	void* m_data;
	char gap30[16];
	PropertyWriter<Vec3> m_velocity;
	PropertyWriter<Vec3> m_acceleration;
	PropertyWriter<Vec3> m_vehicleImpulse;
	PropertyWriter<float> m_cameraAngularVelocityZ;
	ClientRaceVehicleEntityProxy* m_vehicleProxy;
	bool m_isElectrified;
	float m_rotationZ;
	float m_prevRotationZ;
	float m_prevDeltaTime;

	// extra fields
	Vec3 m_prevVehicleVel;
	Vec3 m_currentVehicleVel;
	Vec3 m_prevVehicleAccel;
	Vec3 m_currentVehicleAccel;
};
static_assert(sizeof(ClientUIScreenEffectEntity) == 0xC0, "Wrong size!");

}