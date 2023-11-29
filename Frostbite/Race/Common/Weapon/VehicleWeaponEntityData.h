#pragma once
#include <Engine/Entity/EntityData.h>
#include "VehicleWeaponEntityDataDefs.h"


namespace fb
{

class VehicleWeaponProjectileBlueprint;
class VehicleWeaponEffectBlueprint;
class VehicleWeaponTypeAsset;


struct VehicleWeaponProjectileProperties
{
protected:
	VehicleWeaponProjectileBlueprint* m_blueprint;
	VehicleWeaponEffectBlueprint* m_impactEffect;
	float m_lifetime;
	float m_inflictedDamage;
};

struct UpgradableVehicleWeaponProperties
{
protected:
	float m_buildupTime;
	float m_burnoffTime;
	float m_cooldownTime;
	float m_timeUntilMaxCharge;
	float m_minCharge;
	float m_replenishTime;
	uint m_replenishAmount;
	uint m_startingAmmo;
	uint m_maxAmmo;
	VehicleWeaponEffectBlueprint* m_targetInflictedEffect;
	VehicleWeaponEffectBlueprint* m_collisionInflictedEffect;
	float m_effectInflictedDamage;
	VehicleWeaponProjectileProperties m_projectile;
};


class VehicleWeaponEntityData : public GameComponentEntityData
{
public:
	VehicleWeaponTypeAsset* getVehicleWeaponType() const { return m_vehicleWeaponType; }

protected:
	String m_shortStringId;
	VehicleWeaponTypeAsset* m_vehicleWeaponType;
	VehicleWeaponClassification m_classification;
	float m_externalPreventFiringCooldownTime;
	float m_collisionNotificationThreshold;
	bool m_holdAndRelease;
	bool m_externalPreventFiring;
};

}