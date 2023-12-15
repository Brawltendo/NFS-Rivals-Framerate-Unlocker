#pragma once
#include <Race/Common/Core/Proxy/ProxyPropertyWriter.h>


namespace fb
{

struct UpdateInfo;
class ClientPlayer;
class AnimationComponentData;
class ClientRaceVehicleComponent;


class ClientAnimationComponent
{
public:
	static void ApplyPatches();

	void onPostPhysicsUpdateSync(const UpdateInfo& updateInfo, ClientPlayer* player);
	void onFrameInterpolationUpdate(const UpdateInfo& updateInfo);

protected:
	void** __vftable;
private:
	void applyInterpolation(float interpolationFactor);

	char gap8[8];
	AnimationComponentData* m_data;
	char gap18[360];
	ClientRaceVehicleComponent* m_vehicleComponent;
	ProxyPropertyWriter<float> m_highSpeedWingMovementProperty;
	ProxyPropertyWriter<float> m_highSpeedWingMovementSimpleProperty;
	ProxyPropertyWriter<float> m_airbrakeMovementProperty;
	ProxyPropertyWriter<float> m_airbrakeMovementSimpleProperty;
	ProxyPropertyWriter<float> m_airbrakeSimplePlusHighSpeedSimpleProperty;
	ProxyPropertyWriter<float> m_airbrakeSimplePlusHighSpeedProperty;
	ProxyPropertyWriter<float> m_airbrakePlusHighSpeedSimpleProperty;
	ProxyPropertyWriter<float> m_airbrakePlusHighSpeedProperty;
	float m_highSpeedWingMovement;
	float m_highSpeedWingMovementSimple;
	float m_airbrakeMovement;
	float m_airbrakeMovementSimple;
	float m_airbrakeSimplePlusHighSpeedSimple;
	float m_airbrakeSimplePlusHighSpeed;
	float m_airbrakePlusHighSpeedSimple;
	float m_airbrakePlusHighSpeed;

	// additional fields
	float m_highSpeedWingMovementPrev;
	float m_highSpeedWingMovementSimplePrev;
	float m_airbrakeMovementPrev;
	float m_airbrakeMovementSimplePrev;
	float m_airbrakeSimplePlusHighSpeedSimplePrev;
	float m_airbrakeSimplePlusHighSpeedPrev;
	float m_airbrakePlusHighSpeedSimplePrev;
	float m_airbrakePlusHighSpeedPrev;
};
static_assert(sizeof(ClientAnimationComponent) == 0x308, "Wrong size!");

}