#pragma once
#include <Race/Common/Weapon/VehicleWeaponUpgradable.h>
#include <Race/Common/Weapon/VehicleWeaponType.h>

#include <Core/Reflection/String.h>
#include <Core/Math/LinearTransform.h>
#include <Core/Pattern/WeakPtr.h>
#include <Extension/Rpc/RpcDeclaration.h>


namespace fb
{

class ClientEntityCreationInfo;
class VehicleWeaponEntityData;
class EntityCreationInfo;
struct EntityUpdateInfo;
class EntityBus;
class ComponentCollection;
class ClientRaceVehicleEntity;
class ClientTargetingEntity;
class ClientAffectorEntity;
class Ghost;


struct ClientVehicleWeaponEntity
{
public:
	static void ApplyPatches();

	void frameInterpolationUpdate(const EntityUpdateInfo& info);

private:
	void applyInterpolation(float interpolationFactor, const LinearTransform& currentTransform);

	void **__vftable;
	char gap8[24];
	EntityBus* m_entityBus;
	VehicleWeaponEntityData *m_data;
	ComponentCollection *m_collection;
	char gap40[24];
	uintptr_t qword58;
	char gap60[80];
	uintptr_t qwordB0;
	char gapB8[16];
	uintptr_t qwordC8;
	char gapD0[600];
	VehicleWeaponUpgradable m_upgradable;
	VehicleWeaponType m_vehicleWeaponType;
	RpcDeclaration<0, RpcDirectionClientToServer>::Invoker weaponTriggered;
	RpcDeclaration<1, RpcDirectionClientToServer>::Invoker requestStartBuildup;
	RpcDeclaration1<2, RpcDirectionClientToServer, float>::Invoker requestFireWeapon;
	RpcDeclaration1<3, RpcDirectionClientToServer, Ghost*>::Invoker inflictCollisionEffect;
	WeakPtr<ClientRaceVehicleEntity> m_vehicle;
	WeakPtr<ClientTargetingEntity> m_targeter;
	ProxyPropertyWriter<int> m_ammunitionOutput;
	ProxyPropertyWriter<String> m_shortStringIdOutput;
	ProxyPropertyWriter<Vec3> m_linearVelocityOutput;
	ProxyPropertyWriter<LinearTransform> m_localCollisionPointOutput;
	ProxyPropertyWriter<LinearTransform> m_transformOutput;
	ProxyPropertyWriter<float> m_burnoffTimeOutput;
	ProxyPropertyWriter<float> m_buildupTimeLeftOutput;
	ProxyPropertyWriter<float> m_cooldownOutput;
	ProxyPropertyWriter<float> m_burnoffOutput;
	ProxyPropertyWriter<bool> m_isTargeterActiveOutput;
	ProxyPropertyWriter<bool> m_effectActiveOutput;
	ProxyPropertyWriter<uint> m_weaponTypeIdOutput;

	struct WeakClientRaceVehicleEntityPtrs // eastl::fixed_vector<WeakPtr<ClientRaceVehicleEntity>, 4>
	{
		void* mpBegin;
		void* mpEnd;
		void* mpCapacity;
		void* mAllocator;
		void* mpPoolBegin;
		char mBuffer[sizeof(WeakPtr<ClientRaceVehicleEntity>) * 4];
	} m_unrepliedCollisionNotifications;

	struct Affectors // eastl::fixed_vector<ClientAffectorEntity*, 8>
	{
		void* mpBegin;
		void* mpEnd;
		void* mpCapacity;
		void* mAllocator;
		void* mpPoolBegin;
		char mBuffer[sizeof(ClientAffectorEntity*) * 8];
	} m_collisionAffectors;

	uint m_slot;
	uint m_ammunition;
	float m_endOfBurnoff;
	float m_endOfCooldown;
	float m_cooldownTime;
	float m_buildupTimeLeft;
	bool m_awaitingFireResponse;
	bool m_hasBeenTriggered;
	bool m_inputPressed;
	bool m_buildupTriggered;

	// additional fields
	PropertyWriter<LinearTransform> m_interpolatedTransform;
	Quat m_prevOrientation;
	Quat m_currentOrientation;
	Vec3 m_prevPosition;
	Vec3 m_currentPosition;
};
static_assert(sizeof(ClientVehicleWeaponEntity) == 0x750, "Wrong size!");
//#include <cstddef>
//const static int offset = offsetof(ClientVehicleWeaponEntity, m_buildupTriggered);


}