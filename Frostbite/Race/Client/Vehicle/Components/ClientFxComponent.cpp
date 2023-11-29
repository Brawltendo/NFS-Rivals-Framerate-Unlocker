#include <Race/Client/Vehicle/Components/ClientFxComponent.h>

#include <Core/Math/Vec3.h>
#include <Race/Client/Vehicle/Components/ClientRaceWheelComponentSimulation.h>

#include "safetyhook.hpp"
#include "memoryutils.h"


namespace fb
{

void ClientFxComponent::ApplyPatches()
{
    // redirect fb::ClientFxComponent::componentFrameInterpolationUpdateRequired vfptr
    // returns true so the component opts into the interpolation pass
    {
        uint8_t patch[] = { 0x40, 0x98, 0x75, 0x40, 0x01, 0x00, 0x00, 0x00 };
        PatchBytes(0x14154E958, patch, sizeof(patch));
    }

    // redirect fb::ClientFxComponent::onFrameInterpolationUpdate vfptr to fb::ClientFxComponent::onPostPhysicsUpdateSync
    // this fixes broken particle transforms
    {
        uint8_t patch[] = { 0x60, 0xD0, 0x86, 0x40, 0x01, 0x00, 0x00, 0x00 };
        PatchBytes(0x14154E9E0, patch, sizeof(patch));
    }

    // use interpolated wheel contact normal for wheel VFX
    static SafetyHookMid clientFxComponentHook1{};
    clientFxComponentHook1 = safetyhook::create_mid(reinterpret_cast<void*>(0x140872D05),
        [](SafetyHookContext& ctx)
    {
        fb::ClientRaceWheelComponentSimulation* wheelSim = *reinterpret_cast<fb::ClientRaceWheelComponentSimulation**>(ctx.r15 + 0x338);
        if (wheelSim)
        {
            fb::Vec& outContactNormal = *reinterpret_cast<fb::Vec*>(ctx.rbp - 0x40);
            outContactNormal = wheelSim->contactNormal();
        }
    });

    // use interpolated wheel contact position for wheel VFX
    static SafetyHookMid clientFxComponentHook2{};
    clientFxComponentHook2 = safetyhook::create_mid(reinterpret_cast<void*>(0x140872DDC),
        [](SafetyHookContext& ctx)
    {
        fb::ClientRaceWheelComponentSimulation* wheelSim = *reinterpret_cast<fb::ClientRaceWheelComponentSimulation**>(ctx.r15 + 0x338);
        if (wheelSim)
        {
            fb::Vec& outWheelTrans = *reinterpret_cast<fb::Vec*>(&ctx.xmm0.f32[0]);
            outWheelTrans = wheelSim->contactPosition();
        }
    });
}

}

