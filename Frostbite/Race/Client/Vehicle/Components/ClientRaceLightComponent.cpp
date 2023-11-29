#include "ClientRaceLightComponent.h"

#include "ClientRaceVehicleComponent.h"
#include <Race/Client/Vehicle/Components/ClientRaceVehicleComponentSimulation.h>

#include "safetyhook.hpp"
#include "memoryutils.h"


namespace fb
{

void ClientRaceLightComponent::onFrameInterpolationUpdate(const EntityUpdateInfo& updateInfo)
{
	m_bodyTransform = m_vehicleComponent->getInterpolatedBodyTransform();
}

void ClientRaceLightComponent::ApplyPatches()
{
	// make fb::ClientRaceLightComponent use the interpolated vehicle transform
	static SafetyHookMid clientRaceLightComponentHook1{};
	clientRaceLightComponentHook1 = safetyhook::create_mid(reinterpret_cast<void*>(0x140821686),
		[](SafetyHookContext& ctx)
	{
		ClientRaceLightComponent* thisPtr = reinterpret_cast<ClientRaceLightComponent*>(ctx.rbx);
		LinearTransform& vehicleTransform = *reinterpret_cast<LinearTransform*>(ctx.rbp - 0x70);

		vehicleTransform = thisPtr->m_vehicleComponent->getInterpolatedBodyTransform();
	});

	// redirect fb::ClientRaceLightComponent::componentFrameInterpolationUpdateRequired vfptr
	{
		uint8_t patch[] = { 0x40, 0x98, 0x75, 0x40, 0x01, 0x00, 0x00, 0x00 };
		PatchBytes(0x141541268, patch, sizeof(patch));
	}

	// redirect fb::ClientRaceLightComponent::onFrameInterpolationUpdate vfptr to our patched version
	{
		auto f = &ClientRaceLightComponent::onFrameInterpolationUpdate;
		union { void* ptr; uint8_t arr[8]; } patch{};
		patch.ptr = *(void**)&f;
		PatchBytes(0x1415412F0, patch.arr, sizeof(patch));
	}
}

}