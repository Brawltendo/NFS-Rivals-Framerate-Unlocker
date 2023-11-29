#include <Engine/Physics/Vehicle/EAGR/WheelNFS.h>

#include "memoryutils.h"
#include "safetyhook.hpp"


namespace fb
{

void WheelNFS::ApplyPatches()
{
    // replace tire patch position with contact patch position so it can be interpolated properly
    // hook fb::WheelNFS::getState
    static SafetyHookMid wheelNFSHook1{};
    wheelNFSHook1 = safetyhook::create_mid(reinterpret_cast<void*>(0x14057A4B9),
        [](SafetyHookContext& ctx)
    {
        auto computeContactPostion = reinterpret_cast<Vec3&(__fastcall*)(WheelNFS*, Vec3& _retstr, float yPos)>(0x140577330);

        WheelNFS* thisPtr = reinterpret_cast<WheelNFS*>(ctx.rbx);
        Vec3& tirePatchLocalPos = *reinterpret_cast<Vec3*>(&ctx.xmm1.f32[0]);
        Vec3 contactPos;
        computeContactPostion(thisPtr, contactPos, tirePatchLocalPos.y);
        // put our new contact pos back into xmm1
        tirePatchLocalPos = contactPos;
    });

    // interpolate wheel rotation -- hook fb::WheelNFS::getFrameInterpolationState
    // by default, steering rotation is already interpolated but the rest of it isn't
    static SafetyHookMid wheelNFSHook2{};
    wheelNFSHook2 = safetyhook::create_mid(reinterpret_cast<void*>(0x140579150),
        [](SafetyHookContext& ctx)
    {
        Quat& rotationOrientation = *reinterpret_cast<Quat*>(&ctx.xmm9.f32[0]);
        float interpolationFactor = ctx.xmm11.f32[0];
        const Quat& prevRotation = *reinterpret_cast<Quat*>(ctx.r8 + 0x30);
        const Quat& currRotation = *reinterpret_cast<Quat*>(ctx.r9 + 0x30);

        rotationOrientation = slerp(prevRotation, currRotation, interpolationFactor);
    });

    // nop function call that we don't need anymore
    {
        uint8_t patch[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
        PatchBytes(0x1405794EC, patch, sizeof(patch));
    }

    // interpolate wheel contact position -- hook fb::WheelNFS::getFrameInterpolationState
    static SafetyHookMid wheelNFSHook3{};
    wheelNFSHook3 = safetyhook::create_mid(reinterpret_cast<void*>(0x1405794F8),
        [](SafetyHookContext& ctx)
    {
        float interpFactor = ctx.xmm11.f32[0];
        const Vec& prevContactPos = *reinterpret_cast<Vec*>(ctx.rdi + 0x60);
        const Vec& currContactPos = *reinterpret_cast<Vec*>(ctx.rbx + 0x60);

        Vec& interpolatedContactPos = *reinterpret_cast<Vec*>(&ctx.xmm0.f32[0]);
        interpolatedContactPos = VecLerp(prevContactPos, currContactPos, interpFactor);
    });

}

}