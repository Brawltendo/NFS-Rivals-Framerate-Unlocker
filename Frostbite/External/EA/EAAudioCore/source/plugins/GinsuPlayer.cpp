#include "EAAudioCore/plugins/GinsuPlayer.h"

#include "safetyhook.hpp"
#include "memoryutils.h"


namespace EA
{
namespace Audio
{
namespace Core
{

//extern "C" void GinsuPatchCave();

void GinsuPlayer::ApplyPatches()
{
    // use the SlewTime attribute from the GinsuPlayer so it can use the value directly from the sound patch
    // instead of a constant 33ms time
    {
        uint8_t patch[] =
        {
            0xF3, 0x0F, 0x59, 0x5B, 0x68,  // mulss xmm3,[rbx+68h]
            0x90, 0x90, 0x90               // nop 3
        };
        PatchBytes(0x14103152F, patch, sizeof(patch));
    }

    {
        // addss xmm1,xmm6 -> nop 4
        uint8_t patch1[] = { 0x90, 0x90, 0x90, 0x90 };
        PatchBytes(0x141031643, patch1, sizeof(patch1));
        // subss xmm4,xmm1 -> addss xmm4,xmm2
        uint8_t patch2[] = { 0xF3, 0x0F, 0x58, 0xE2 };
        PatchBytes(0x14103169B, patch2, sizeof(patch2));
        // comiss xmm4,xmm5 -> movaps xmm7,xmm4
        uint8_t patch3[] = { 0x0F, 0x28, 0xFC };
        PatchBytes(0x14103169F, patch3, sizeof(patch3));
        // ja 1410316B7 -> jmp 1410316D1
        uint8_t patch4[] = { 0xEB, 0x2D };
        PatchBytes(0x1410316A2, patch4, sizeof(patch4));
    }

    // patch slew samples
    // this is the main fix, as without this the Ginsu synth will lock up try
    static SafetyHookMid ginsuPatchHook1{};
    ginsuPatchHook1 = safetyhook::create_mid(reinterpret_cast<void*>(0x141031552),
        [](SafetyHookContext& ctx)
    {
        GinsuPlayer* ginsuPlayer = reinterpret_cast<GinsuPlayer*>(ctx.rbx);

        float slewSamplesRemaining = ctx.xmm3.f32[0];
        slewSamplesRemaining -= ginsuPlayer->mOutputSamplesRequested;
        if (slewSamplesRemaining < 0.f)
        {
            slewSamplesRemaining = 0.f;
        }
        ctx.xmm1.f32[0] = slewSamplesRemaining;
    });

    // xmm3 -> xmm1
    // the decremented and clamped slew value from above should be used to set the mfSlewSamplesRemaining field
    // swapping the register here makes that happen
    uint8_t patch1[] = { 0x8B };
    PatchBytes(0x14103155B, patch1, sizeof(patch1));

    // same exact code but it needs to access different registers

    static SafetyHookMid ginsuPatchHook2{};
    ginsuPatchHook2 = safetyhook::create_mid(reinterpret_cast<void*>(0x1410317FB),
        [](SafetyHookContext& ctx)
    {
        uintptr_t& numSamples = ctx.r8;
        if (numSamples > 223)
        {
            numSamples = 223;
        }
    });

    static SafetyHookMid ginsuPatchHook3{};
    ginsuPatchHook3 = safetyhook::create_mid(reinterpret_cast<void*>(0x1410319E4),
        [](SafetyHookContext& ctx)
    {
        uintptr_t& numSamples = ctx.rcx;
        if (numSamples > 223)
        {
            numSamples = 223;
        }
    });
}

} // namespace Core
} // namespace Audio
} // namespace EA