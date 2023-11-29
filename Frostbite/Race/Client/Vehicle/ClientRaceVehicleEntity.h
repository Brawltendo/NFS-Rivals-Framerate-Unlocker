#pragma once
#include <Engine/Core/Types.h>
#include <Race/Common/Core/Proxy/ProxyPropertyWriter.h>
#include <Engine/Core/Math/Vec3.h>
#include <Engine/Core/Math/Vec4.h>


namespace fb
{

class ComponentCollection;
class SimplePhysicsEntity;
class ClientRaceVehicleComponent;
class MeshModel;


class ClientRaceVehicleEntity
{
public:
	bool hasLocalPlayer()
	{
		auto native = reinterpret_cast<bool(__fastcall*)(ClientRaceVehicleEntity*)>(0x1407BFCD0);
		return native(this);
	}

	Vec3& linearVelocity()
	{
		auto SimplePhysicsEntity_linearVelocity = reinterpret_cast<Vec3*(__fastcall*)(SimplePhysicsEntity*)>(0x1405444A0);
		return *SimplePhysicsEntity_linearVelocity(m_physicsEntity);
	}

	float impulseMagnitude() { return m_impulseMagnitude.get(); }
	Vec3 impulseNormal() { return m_impulseNormal.get(); }
	Vec3 accelerationWorldSpace() { return m_accelerationWorldSpace; }
	float electrifiedAmount() { return m_electrifiedAmount; }

protected:
	void** __vftable;
private:
	char gap_8[48];
	ComponentCollection *m_collection;
	char gap_40[936];
	SimplePhysicsEntity *m_physicsEntity;
	char gap_3F0[552];
	ClientRaceVehicleComponent* m_chassis;
	char gap_620[32];
	MeshModel* m_meshModel;
	MeshModel* m_camouflageMeshModel;
	MeshModel* m_cockpitMeshModel;
	char gap_658[968];
	u16 m_lightProbeHandle;
	uint m_preExplosionEffectHandle;
	uint m_criticallyDamagedEffectHandle;
	u8 flags;
	ProxyPropertyWriter<float> m_impulseMagnitude;
	ProxyPropertyWriter<Vec3> m_impulseNormal;
	ProxyPropertyWriter<Vec3> m_impulsePosition;
	ProxyPropertyWriter<float> m_lastDamageAmountTaken;
	ProxyPropertyWriter<int> m_lastCollisionLocation;
	ProxyPropertyWriter<bool> m_lastCollisionInvolvedPlayer;
	ProxyPropertyWriter<float> m_lastCollisionClosingVelocityAlongNormalMPH;
	ProxyPropertyWriter<bool> m_lastCollisionGlancingHitProperty;
	ProxyPropertyWriter<Vec3> m_damageColor;
	ProxyPropertyWriter<bool> m_powerEnabled;
	ProxyPropertyWriter<float> m_wrongWayDistancePropertyWriter;
	ProxyPropertyWriter<float> m_distanceDrivenPropertyWriter;
	ProxyPropertyWriter<float> m_timeDrivenPropertyWriter;
	ProxyPropertyWriter<int> m_nearMissesPropertyWriter;
	ProxyPropertyWriter<bool> m_hasLocalPlayerWriter;
	ProxyPropertyWriter<Vec4> m_VfxCollisionPlaneEquationWriter;
	bool m_lastCollisionGlancingHit;
	char gap_D21[123];
	bool m_isHornPressed;
	ProxyPropertyWriter<float> m_isOffroad;
	float m_fadeTimer;
	bool m_wasOffRoadLastFrame;
	float m_electrifiedAmount;
	float m_oldRotor;
	float m_exhaustPipeTemperature;
	float m_dirtFactor;
	float m_dustFactor;
	float m_mudFactor;
	float m_snowFactor;
	float m_scrollFactor;
	float m_prevDeltaTime;
	float m_steering;
	bool m_tireGripEffectEnabled;
	bool m_steeringEffectEnabled;
	bool m_isCustomizationVehicle;
	char gap_DFB[5461];
	Vec3 m_accelerationWorldSpace;
	char gap2360[48];
};


}