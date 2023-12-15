#include "pch.h"

#include <Extension/Cinebot/CinebotCamera.h>
#include <Race/Client/Vehicle/Components/ClientAnimatedDriverComponent.h>
#include <Race/Client/Vehicle/Components/ClientRaceLightComponent.h>
#include <Race/Client/Vehicle/Components/ClientFxComponent.h>
#include <Race/Client/Vehicle/Components/ClientRaceVehicleComponent.h>
#include <Race/Client/Vehicle/Components/ClientRaceVehicleComponentSimulation.h>
#include <Race/Client/Vehicle/Components/ClientAnimationComponent.h>
#include <Engine/Physics/Vehicle/EAGR/WheelNFS.h>
#include <Race/Client/Vehicle/Components/ClientRaceWheelComponentSimulation.h>
#include <Race/Client/UI/Entities/Screen/ClientUIScreenEffectEntity.h>
#include <Race/Client/Weapon/ClientVehicleWeaponEntity.h>
#include <Engine/Entity/MiscEntities.h>
#include <EAAudioCore/plugins/GinsuPlayer.h>

#include <fstream>
#include <string>
#include <shellapi.h>

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
    static SafetyHookMid uiCursorMidHook{};
    uiCursorMidHook = safetyhook::create_mid(reinterpret_cast<void*>(0x1409AAAD5),
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

    // fix procedural animation speed for rigid mesh entities
    // hook fb::RigidMeshEntityRenderer::buildPrimitives
    static SafetyHookMid rigidMeshProcAnimMidHook{};
    rigidMeshProcAnimMidHook = safetyhook::create_mid(reinterpret_cast<void*>(0x1406AF293),
        [](SafetyHookContext& ctx)
    {
        char* viewInfo = *reinterpret_cast<char**>(ctx.r12 + 0x8);
        const float simDeltaTime = *reinterpret_cast<float*>(viewInfo + 0x210);
        const float dT30 = 1.f / 30.f;
        // adjust procedural animation local time with time scale from 30 FPS delta time
        ctx.xmm2.f32[0] *= (simDeltaTime / dT30);
    });

    // fix procedural animation speed for static mesh entities
    // hook fb::StaticMeshEntityRenderer::buildPrimitives
    static SafetyHookMid staticMeshProcAnimMidHook{};
    staticMeshProcAnimMidHook = safetyhook::create_mid(reinterpret_cast<void*>(0x14029AD4A),
        [](SafetyHookContext& ctx)
    {
        char* builder = *reinterpret_cast<char**>(ctx.rsp + 0x50);
        char* viewInfo = *reinterpret_cast<char**>(builder + 0x8);
        const float simDeltaTime = *reinterpret_cast<float*>(viewInfo + 0x210);
        const float dT30 = 1.f / 30.f;
        // adjust procedural animation local time with time scale from 30 FPS delta time
        ctx.xmm5.f32[0] *= (simDeltaTime / dT30);
    });

    // hijack fb::ExecutionContext ctor to pass in more arguments
    // Steam users end up with a launch command that's too long after launching from Frosty,
    // so the solution is to just not have them enter commands through the launcher
    // it's also one less thing users can mess up during installation ¯\_(ツ)_/¯
    static SafetyHookMid executionContextMidHook{};
    executionContextMidHook = safetyhook::create_mid(reinterpret_cast<void*>(0x14016860D),
        [](SafetyHookContext& ctx)
    {
        int argcOld = ctx.rdx;
        char** argv = reinterpret_cast<char**>(ctx.r8);
        std::ifstream commands("commands.txt");
        std::string commandLine;
        std::getline(commands, commandLine);
        if (!commandLine.empty())
        {
            int argcNew;
            LPWSTR* wArglist = CommandLineToArgvW(std::wstring(commandLine.begin(), commandLine.end()).c_str(), &argcNew);
            if (wArglist)
            {
                int utfCharCount = 0;
                for (int i = 0; i < argcNew; ++i)
                {
                    utfCharCount += WideCharToMultiByte(CP_UTF8, 0, wArglist[i], -1, NULL, 0, NULL, NULL);
                }

                // allocate space for the new arguments
                static char** argvNew = (char**)malloc((argcOld + argcNew + 1) * sizeof(char*) + utfCharCount);
                if (argvNew)
                {
                    // place old arg pointers into the beginning of the list
                    for (int i = 0; i < argcOld; ++i)
                    {
                        argvNew[i] = argv[i];
                    }

                    // get start of string list
                    char* argStrList = (char*)&argvNew[argcOld + argcNew + 1];
                    for (int i = 0; i < argcNew; ++i)
                    {
                        // start after the old arg pointers
                        argvNew[argcOld+i] = argStrList;
                        // write string and advance position in list
                        argStrList += WideCharToMultiByte(CP_UTF8, 0, wArglist[i], -1, argStrList, utfCharCount, NULL, NULL);
                    }
                    argvNew[argcOld + argcNew] = NULL;

                    ctx.rdx = argcOld + argcNew;
                    ctx.r8 = (uintptr_t)argvNew;

                    LocalFree(wArglist);
                }
            }
        }
    });

}

void InitHook()
{
    uintptr_t game = (uintptr_t)GetModuleHandle(NULL);
    MH_Initialize();
    fb::CinebotCamera::ApplyPatches();
    fb::ClientAnimatedDriverComponent::ApplyPatches();
    fb::ClientAnimationComponent::ApplyPatches();
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

