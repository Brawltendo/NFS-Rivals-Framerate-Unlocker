#pragma once
#include "../../../Common/Core/Proxy/ProxyPropertyWriter.h"
#include <Engine/Core/Math/Vec4.h>
#include <Engine/Core/Math/LinearTransform.h>


namespace fb
{

struct EntityUpdateInfo;
class ClientRaceVehicleComponent;
class ClientCopComponent;


class ClientRaceLightComponent
{
public:
	static void ApplyPatches();

	void onFrameInterpolationUpdate(const EntityUpdateInfo& updateInfo);

private:
	void** __vftable;
	char gap8[376];
	ClientRaceVehicleComponent* m_vehicleComponent;
	ClientCopComponent* m_copComponent;
	ProxyPropertyWriter<float> m_tailBrakeLightIntensity;
	ProxyPropertyWriter<Vec4> m_carLightsMain;
	ProxyPropertyWriter<Vec4> m_carLightsBlinkers;
	ProxyPropertyWriter<Vec4> m_sirens;
	ProxyPropertyWriter<Vec4> m_onOff;
	ProxyPropertyWriter<LinearTransform> m_bodyTransform;
	bool m_isHeadlightOn;
	bool m_isTaillightOn;
	bool m_isBraking;
	float m_currentTime;
	bool m_prevHeadLights;
	bool m_prevTailLights;
	bool m_prevBrakeLights;
	bool m_prevBrakeLights_Right;
	bool m_prevBrakeLights_Left;
	bool m_prevReverse_Flare_Left;
	bool m_prevReverse_Flare_Right;
	bool m_prevSiren_Red1;
	bool m_prevSiren_Red2;
	bool m_prevSiren_Blue1;
	bool m_prevSiren_Blue2;
	bool m_prevSiren_Blinker_R;
	bool m_prevSiren_Blinker_L;
};
static_assert(sizeof(ClientRaceLightComponent) == 0x350, "Wrong size!");

}