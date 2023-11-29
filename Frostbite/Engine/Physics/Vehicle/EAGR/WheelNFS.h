#pragma once
#include <Engine/Physics/Vehicle/EAGR/IRaceWheel.h>

#include <Core/Math/LinearTransform.h>
#include <Entity/Material/MaterialGridData.h>


namespace fb
{

class RaceTirePropertyRelationPhysicsData;
class MemoryArena;
class IRaceVehicle;
class IPhysicsManager;
class RaceWheelConfigData;
class hkpShapePhantom;
struct SafeQueryResult;


class SimSurface
{
private:
	const RaceTirePropertyRelationPhysicsData* m_materialData;
};

class WheelNFS : public IRaceWheel
{
public:
	static void ApplyPatches();

private:
	LinearTransform m_xCarAdjustment;
	Quat m_xCarWheelAdjustment;
	Vec3 m_steeringAngle;
	Vec3 m_contactNormal;
	Vec3 m_contactPosition;
	Vec3 m_contactVelocity;
	Quat m_steeringOrientation;
	Quat m_rotationOrientation;
	float m_angularVelocity;
	float m_rotationAngle;
	float m_dampenedSteeringAngle;
	MemoryArena& m_arena;
	IRaceVehicle* m_vehicle;
	IPhysicsManager* m_physicsManager;
	const RaceWheelConfigData& m_data;

	struct LastState
	{
		Vec3 m_contactPosition;
		Vec3 m_contactNormal;
		bool m_contactStatus;
	} m_lastState;

	WheelStatus m_status;
	MaterialDecl m_contactMaterial;
	MaterialDecl m_wheelMaterial;
	float m_springLength;
	bool m_contactStatus;
	Vec3 m_initialPosition;
	float m_localPositionY;
	float m_localPenetrationY;
	LinearTransform m_storedLocalTransform;
	LinearTransform m_storedWorldTirePatchTransform;
	hkpShapePhantom* m_phantom;
	float m_springHitLocalY;
	Vec3 m_NFSMountPosition;
	Vec3 m_NFSTirePatchPosition;
	Vec3 m_resetBlendPosition;
	float m_dataSpringAttachOffsetY;
	float m_dataSpringGetLength;
	int m_dataRotationDirectionIndex;
	int m_dataSteeringAngleIndex;
	SimSurface m_NFSSurface;
	float m_wheelRadius;
	float m_tireWallHeight;
	float m_wheelLowerLimit;
	float m_wheelUpperLimit;
	float m_suspensionCompression;
	SafeQueryResult* m_raycastResults;
	Vec3 m_raycastFrom;
	Vec3 m_raycastTo;
	bool m_wantsAsyncUpdate;
	bool m_validPosition;
	bool m_resetBlendPositionValid;
	bool m_isBlendingPosition;
	bool m_enabled;
	float m_surfaceGripScalar;
	float m_lateralGripScalar;
	float m_longitudinalGripScalar;
	float m_sideSlipAngle;
	float m_slipRatio;
	bool m_hasBurstTire;
};
static_assert(sizeof(WheelNFS) == 0x2A0, "Wrong size!");

}