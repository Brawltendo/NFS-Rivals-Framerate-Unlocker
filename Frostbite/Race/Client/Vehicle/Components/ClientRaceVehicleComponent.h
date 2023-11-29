#pragma once
#include <Engine/Entity/Component.h>
#include "../../../Common/Core/Proxy/ProxyPropertyWriter.h"
#include <Engine/Core/Math/Vec3.h>
#include <Engine/Core/Math/LinearTransform.h>


namespace fb
{

struct EntityUpdateInfo;
struct UpdateInfo;
struct ImpulseInfo;
class ComponentCollection;
class RaceVehicleComponentData;
struct CreationInfo;
struct ClientComponentInitInfo;
class IRaceVehicle;
class ClientRaceVehicleComponentSimulation;
class ClientGhost;
class ClientRaceVehicleComponentUpdater;


class ClientRaceVehicleComponent : public Component
{
public:
	static void ApplyPatches();

	// fake constructor
	ClientRaceVehicleComponent* ctor(CreationInfo* info, const RaceVehicleComponentData& data);

	ClientRaceVehicleComponentSimulation* getController() { return m_controller; }

	void onDeinit();
	void onInit(const ClientComponentInitInfo& info);
	void onPostPhysicsUpdateSync(const EntityUpdateInfo& updateInfo);
	void onFrameInterpolationUpdate(const EntityUpdateInfo& updateInfo);

	typedef ClientRaceVehicleComponent*(__fastcall* t_ctor)(ClientRaceVehicleComponent*, CreationInfo*, const RaceVehicleComponentData&);
	static t_ctor o_ctor;

	typedef void(__fastcall* t_onDeinit)(ClientRaceVehicleComponent*);
	static t_onDeinit o_onDeinit;

	typedef void(__fastcall* t_onInit)(ClientRaceVehicleComponent*, const ClientComponentInitInfo&);
	static t_onInit o_onInit;

	typedef void(__fastcall* t_onPostPhysicsUpdateSync)(ClientRaceVehicleComponent*, const EntityUpdateInfo&);
	static t_onPostPhysicsUpdateSync o_onPostPhysicsUpdateSync;

	typedef void(__fastcall* t_onFrameInterpolationUpdate)(ClientRaceVehicleComponent*, const EntityUpdateInfo&);
	static t_onFrameInterpolationUpdate o_onFrameInterpolationUpdate;

	void update(const UpdateInfo& info);

	const LinearTransform& getInterpolatedTransform() { return m_interpolatedTransform.get(); }
	const LinearTransform& getInterpolatedBodyTransform() { return m_interpolatedBodyTransform; }

private:
	void applyInterpolation(float interpolationFactor);
	void updaterFrameInterpolationUpdate(const UpdateInfo& info);
	void updaterPostSimUpdate(const UpdateInfo& info);

	void** __vftable;
	ComponentCollection* m_collection;
	char gap_10[584];
	IRaceVehicle* m_vehiclePhysics;
	char gap_260[1056];
	ClientRaceVehicleComponentSimulation* m_controller;
	char gap_688[212];
	bool m_isRailing;
	ProxyPropertyWriter<LinearTransform> m_raceVehicleTransform;
	ProxyPropertyWriter<LinearTransform> m_raceVehicleBodyTransform;
	ProxyPropertyWriter<float> m_currentVelocity;
	ProxyPropertyWriter<Vec3> m_raceVehicleVelocity;
	ProxyPropertyWriter<Vec3> m_raceVehicleAcceleration;
	ProxyPropertyWriter<Vec3> m_localAngularVelocity;
	ProxyPropertyWriter<Vec3> m_angularVelocityOut;
	ProxyPropertyWriter<Vec3> m_boundingBoxMin;
	ProxyPropertyWriter<Vec3> m_boundingBoxMax;
	ProxyPropertyWriter<float> m_forwardAcceleration;
	ProxyPropertyWriter<float> m_forwardVelocity;
	ProxyPropertyWriter<float> m_speedMPS;
	ProxyPropertyWriter<float> m_rpm;
	ProxyPropertyWriter<float> m_maxRpm;
	ProxyPropertyWriter<float> m_gear;
	ProxyPropertyWriter<float> m_gearChangeTime;
	ProxyPropertyWriter<bool> m_isTumbling;
	ProxyPropertyWriter<bool> m_isTotaled;
	ProxyPropertyWriter<float> m_driftScale;
	ProxyPropertyWriter<float> m_driftAngleDegrees;
	ProxyPropertyWriter<float> m_driftDistancePropertyWriter;
	ProxyPropertyWriter<float> m_aboveGroundTestDistance;
	ProxyPropertyWriter<bool> m_isAboveGroundTestValid;
	ProxyPropertyWriter<float> m_timeInAir;
	ProxyPropertyWriter<bool> m_isAirborne;
	ProxyPropertyWriter<bool> m_isStationary;
	ProxyPropertyWriter<bool> m_isRaceCarPhysical;
	ProxyPropertyWriter<bool> m_doAllWheelsHaveTraction;
	ProxyPropertyWriter<bool> m_isAutomatic;
	ProxyPropertyWriter<float> m_throttle;
	ProxyPropertyWriter<float> m_clutch;
	ProxyPropertyWriter<float> m_load;
	ProxyPropertyWriter<float> m_brakePos;
	ProxyPropertyWriter<float> m_handbrake;
	ProxyPropertyWriter<float> m_roadNoiseFrequencyScale;
	ProxyPropertyWriter<float> m_roadNoiseMagnitudeScale;
	ProxyPropertyWriter<float> m_springCompression;
	ProxyPropertyWriter<float> m_lateralSlip;
	ProxyPropertyWriter<float> m_longitudinalSlip;
	ProxyPropertyWriter<float> m_lateralSlipFL;
	ProxyPropertyWriter<float> m_lateralSlipFR;
	ProxyPropertyWriter<float> m_lateralSlipRL;
	ProxyPropertyWriter<float> m_lateralSlipRR;
	ProxyPropertyWriter<float> m_longitudinalSlipFL;
	ProxyPropertyWriter<float> m_longitudinalSlipFR;
	ProxyPropertyWriter<float> m_longitudinalSlipRL;
	ProxyPropertyWriter<float> m_longitudinalSlipRR;
	ProxyPropertyWriter<float> m_loadFL;
	ProxyPropertyWriter<float> m_loadFR;
	ProxyPropertyWriter<float> m_loadRL;
	ProxyPropertyWriter<float> m_loadRR;
	ProxyPropertyWriter<float> m_angularVelocityFL;
	ProxyPropertyWriter<float> m_angularVelocityFR;
	ProxyPropertyWriter<float> m_angularVelocityRL;
	ProxyPropertyWriter<float> m_angularVelocityRR;
	ProxyPropertyWriter<float> m_physicsImpulseMagnitude;
	ProxyPropertyWriter<Vec3> m_physicsImpulseDirection;
	ProxyPropertyWriter<Vec3> m_physicsImpulsePosition;
	ProxyPropertyWriter<bool> m_forcedInductionIsTurboCharger;
	ProxyPropertyWriter<bool> m_forcedInductionIsSuperCharger;
	ProxyPropertyWriter<float> m_forcedInductionSpool;
	ProxyPropertyWriter<float> m_forcedInductionCurrentPSI;
	ProxyPropertyWriter<float> m_forcedInductionRelativeTorqueGain;
	ProxyPropertyWriter<float> m_forcedInductionByPassPosition;
	ProxyPropertyWriter<float> m_forcedInductionIsBlowoffOpened;
	ProxyPropertyWriter<float> m_forcedInductionIsWastegateOpened;
	ProxyPropertyWriter<float> m_nearestPursuitDistance;
	ProxyPropertyWriter<float> m_isInFrontOfPursuitTarget;
	LinearTransform m_raceVehicleTransformCache;
	float m_driftDistance;
	float m_speedMPSLastFrame;
	float m_loadCache;
	float m_steering;
	bool m_tireGripEffectEnabled;
	bool m_steeringEffectEnabled;
	float m_timeInCrashBelowSpeedThreshold;
	bool m_cachedBoundingBoxDirty;
	ClientGhost* m_ghost;

	// added fields

	struct TransformHistory
	{
		TransformHistory()
		{
			m_prevOrientation = Quat();
			m_currentOrientation = Quat();
			m_prevPosition = vecZero();
			m_currentPosition = vecZero();
		}

		Quat m_prevOrientation;
		Quat m_currentOrientation;
		Vec3 m_prevPosition;
		Vec3 m_currentPosition;
	};

	ProxyPropertyWriter<LinearTransform> m_interpolatedTransform;
	ProxyPropertyWriter<float> m_interpolatedRpm;
	float m_prevRpm;
	float m_currentRpm;
	TransformHistory m_vehicleTransformHistory;
	TransformHistory m_vehicleBodyTransformHistory;
	LinearTransform m_interpolatedBodyTransform;

	static ClientRaceVehicleComponentUpdater* s_updater;
};
const int sz = sizeof(ClientRaceVehicleComponent);
static_assert(sizeof(ClientRaceVehicleComponent) == 0x1530, "Wrong size!");
//#include <cstddef>
//const static int offset = offsetof(ClientRaceVehicleComponent, m_interpolatedBodyTransform);

}