#include "pch.h"

#include <Extension/Cinebot/CinebotCamera.h>
#include <Race/Client/Vehicle/Components/ClientAnimatedDriverComponent.h>
#include <Race/Client/Vehicle/Components/ClientRaceLightComponent.h>
#include <Race/Client/Vehicle/Components/ClientFxComponent.h>
#include <Race/Client/Vehicle/Components/ClientRaceVehicleComponent.h>
#include <Race/Client/Vehicle/Components/ClientRaceVehicleComponentSimulation.h>
#include <Engine/Physics/Vehicle/EAGR/WheelNFS.h>
#include <Race/Client/Vehicle/Components/ClientRaceWheelComponentSimulation.h>
#include <Race/Client/UI/Entities/Screen/ClientUIScreenEffectEntity.h>
#include <Race/Client/Weapon/ClientVehicleWeaponEntity.h>
#include <Engine/Entity/MiscEntities.h>
#include <EAAudioCore/plugins/GinsuPlayer.h>

#include "util/memoryutils.h"
#include "MinHook.h"
#include <safetyhook.hpp>


void ApplyMiscPatches()
{

    // make fb::ClientUIVehicleDisplayEntity::update reference the value of ClientRaceVehicleComponent.m_interpolatedTransform
    {
        uint8_t patch1[] = { 0xF0, 0x13 };
        PatchBytes(0x140914CAF, patch1, sizeof(patch1));
        uint8_t patch2[] = { 0x00, 0x14 };
        PatchBytes(0x140914CB7, patch2, sizeof(patch2));
        uint8_t patch3[] = { 0x10, 0x14 };
        PatchBytes(0x140914CBF, patch3, sizeof(patch3));
        uint8_t patch4[] = { 0x20, 0x14 };
        PatchBytes(0x140914CC6, patch4, sizeof(patch4));
    }

    // fix map UI cursor speed
    static SafetyHookMid midhook1{};
    midhook1 = safetyhook::create_mid(reinterpret_cast<void*>(0x1409AAAD5),
        [](SafetyHookContext& ctx)
    {
        const float dT = ctx.xmm10.f32[0];
        const float frictionalForce = *reinterpret_cast<float*>(ctx.rsi + 0x108);
        const fb::Vec& currentForce = *reinterpret_cast<fb::Vec*>(ctx.rbx+0x200);
        const float dT30 = 1.f / 30.f;
        // get percentage of 30 FPS delta time from the current delta time
        // and apply it to the friction force to get consistent behavior at all times
        float speedMul = 1.f - frictionalForce * (dT / dT30);
        fb::Vec* newForce = reinterpret_cast<fb::Vec*>(&ctx.xmm0.f32[0]);
        *newForce = fb::VecMul(currentForce, fb::vec(speedMul));
    });

}

//DWORD WINAPI Start(LPVOID lpParam)
void InitHook()
{
    uintptr_t game = (uintptr_t)GetModuleHandle(NULL);
    MH_Initialize();
    fb::CinebotCamera::ApplyPatches();
    fb::ClientAnimatedDriverComponent::ApplyPatches();
    fb::ClientRaceLightComponent::ApplyPatches();
    fb::ClientFxComponent::ApplyPatches();
    fb::ClientRaceVehicleComponent::ApplyPatches();
    fb::WheelNFS::ApplyPatches();
    fb::ClientUIScreenEffectEntity::ApplyPatches();
    fb::TransformMultiplierEntity::ApplyPatches();
    fb::ClientVehicleWeaponEntity::ApplyPatches();
    EA::Audio::Core::GinsuPlayer::ApplyPatches();
    ApplyMiscPatches();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        InitHook();
        break;

    case DLL_PROCESS_DETACH:
        MH_Uninitialize();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE; // successful
}

