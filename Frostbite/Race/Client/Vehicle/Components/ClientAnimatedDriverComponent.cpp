#include "ClientAnimatedDriverComponent.h"

#include "../../../../../util/memoryutils.h"


namespace fb
{

void ClientAnimatedDriverComponent::onFrameInterpolationUpdate(const EntityUpdateInfo& updateInfo)
{
    if (getRaceVehicle())
	    updateVehicleRuntimeParameters();
}

void ClientAnimatedDriverComponent::ApplyPatches()
{
    // redirect fb::ClientAnimatedDriverComponent::componentFrameInterpolationUpdateRequired vfptr
    {
        uint8_t patch[] = { 0x40, 0x98, 0x75, 0x40, 0x01, 0x00, 0x00, 0x00 };
        PatchBytes(0x14154E4B8, patch, sizeof(patch));
    }

    // redirect fb::ClientAnimatedDriverComponent::onFrameInterpolationUpdate vfptr to our patched version
    {
        auto f = &ClientAnimatedDriverComponent::onFrameInterpolationUpdate;
        union { void* ptr; uint8_t arr[8]; } patch{};
        patch.ptr = *(void**)&f;
        PatchBytes(0x14154E540, patch.arr, sizeof(patch));
    }

    // make fb::ClientAnimatedDriverGameStateInterfaceComponent::s_postSimUpdater use UpdatePass_PreAnimation
    {
        uint8_t patch[] = { 0x06 };
        PatchBytes(0x14087BC3A, patch, sizeof(patch));
    }

    // make fb::ClientAnimatedDriverGameStateInterfaceComponent::s_postFrameUpdater use UpdatePass_PostAnimation
    {
        uint8_t patch[] = { 0x08 };
        PatchBytes(0x14087BC91, patch, sizeof(patch));
    }

    // make fb::ClientAnimatedDriverGameStateInterfaceComponentUpdater::postQueryUpdate check for UpdatePass_PreAnimation
    {
        uint8_t patch[] = { 0x06 };
        PatchBytes(0x14087C3E8, patch, sizeof(patch));
    }

    // make fb::ClientAnimatedDriverGameStateInterfaceComponentUpdater::postQueryUpdate check for UpdatePass_PostAnimation
    {
        uint8_t patch[] = { 0x08 };
        PatchBytes(0x14087C429, patch, sizeof(patch));
    }
}

}