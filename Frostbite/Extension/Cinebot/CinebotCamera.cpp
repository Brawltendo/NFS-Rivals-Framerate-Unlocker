#include "CinebotCamera.h"

#include "CinebotCameraShot.h"
#include "ICinebotDirector.h"

#include <Engine/Core/Math/Math.h>

#include "../../../util/memoryutils.h"
#include "MinHook.h"


namespace fb
{

Quat CinebotCamera::m_prevOrientation;
Quat CinebotCamera::m_currentOrientation;
Vec3 CinebotCamera::m_prevPosition;
Vec3 CinebotCamera::m_currentPosition;
float CinebotCamera::m_prevFov;
float CinebotCamera::m_currentFov;
bool CinebotCamera::m_cameraCut;
bool CinebotCamera::m_useFrameInterpolation;

CinebotCamera::t_ctor CinebotCamera::o_ctor = nullptr;
CinebotCamera* CinebotCamera::ctor(CreationInfo& info, const CinebotCameraData& cameraData)
{
    m_prevOrientation = Quat();
    m_currentOrientation = Quat();
    m_prevPosition = vecZero();
    m_currentPosition = vecZero();
    m_prevFov = 0.f;
    m_cameraCut = false;
    m_useFrameInterpolation = false;
    return CinebotCamera::o_ctor(this, info, cameraData);
}

void CinebotCamera::postSimUpdate(const CameraUpdateInfo& info)
{
    uintptr_t settingsManager = 0x141CACD28;
    uintptr_t* clientSettings = reinterpret_cast<uintptr_t*>(0x141A4A8D0);
    uintptr_t clientSettings_TypeInfo = 0x141F717F0;
    auto settingsManager_getContainer = reinterpret_cast<char*(__fastcall*)(uintptr_t inSettingsManager, uintptr_t typeInfo)>(0x14016FAC0);

    char* container = reinterpret_cast<char*>(*clientSettings);
    if (!*clientSettings)
    {
        container = settingsManager_getContainer(settingsManager, clientSettings_TypeInfo);
        *clientSettings = reinterpret_cast<uintptr_t>(container);
    }
    if (*(bool*)(container + 0xCA))
    {
        m_useFrameInterpolation = true;
        o_postFrameUpdate(this, info);
    }
    else
    {
        m_useFrameInterpolation = false;
    }
}

CinebotCamera::t_postFrameUpdate CinebotCamera::o_postFrameUpdate = nullptr;
void CinebotCamera::postFrameUpdate(const CameraUpdateInfo& info)
{
    if (m_useFrameInterpolation)
    {
        float interpolationFactor = info.interpolationFactor;
        Vec interpPos = VecLerp(m_prevPosition, m_currentPosition, interpolationFactor);
        Quat interpOrientation = slerp(m_prevOrientation, m_currentOrientation, interpolationFactor);
        LinearTransform interpT(interpOrientation, interpPos);
        setTransform(interpT);
        m_fov = lerp(m_prevFov, m_currentFov, interpolationFactor);
    }
    else
    {
        o_postFrameUpdate(this, info);
        LinearTransform trans(m_currentOrientation, m_currentPosition);
        setTransform(trans);
        m_fov = m_currentFov;
    }
}

CinebotCamera::t_commitShot CinebotCamera::o_commitShot = nullptr;
void CinebotCamera::commitShot(const CinebotCameraShot& shot)
{
    if (shot.valid)
    {
        LinearTransform transform(getTransform());
        shot.calcTransform(transform);
        LinearTransform rotY(LinearTransform::RotationAxisY::RotateY, 3.1415927f);
        LinearTransform newT(rotY * transform);

        m_prevPosition = m_currentPosition;
        m_currentPosition = newT.trans;
        m_prevOrientation = m_currentOrientation;
        m_currentOrientation = Quat(newT);
        m_prevFov = m_currentFov;
        m_currentFov = shot.fov;
        // seems to be unused in 2015?
        if (m_cameraCut)
        {
            m_prevFov = shot.fov;
            setTransform(newT);
            m_cameraCut = false;
            m_fov = m_currentFov;
        }
        setViewportOffset(shot.viewPortOffset);
        setNearPlaneOverride(shot.nearPlane);
        setFarPlaneOverride(shot.farPlane);
        m_director->setTimeScale(shot.timeScale);
    }
}

void CinebotCamera::ApplyPatches()
{
    // increase size of fb::CinebotCamera from 0x670 to 0x6C0
    // with this we can add extra fields for interpolation
    /*{
        uint8_t patch1[] = { 0xC0 };
        PatchBytes(0x1400F4D6A, patch1, sizeof(patch1));
        uint8_t patch2[] = { 0xC0 };
        PatchBytes(0x141F2BFEA, patch2, sizeof(patch2));
    }*/

    // redirect fb::CinebotCamera::postSimUpdate vfptr to our patched version
    {
        auto f = &CinebotCamera::postSimUpdate;
        union { void* ptr; uint8_t arr[8]; } patch{};
        patch.ptr = *(void**)&f;
        PatchBytes(0x1413E7A48, patch.arr, sizeof(patch));
    }

    // nop call to fb::TargetCamera::postFrameUpdate in fb::CinebotCamera::postFrameUpdate
    {
        uint8_t patch[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        //PatchBytes(0x1400F7D40, patch, sizeof(patch));
    }

    // nop check for magnitude before calling fb::CinebotCamera::commitShot
    {
        uint8_t patch[] = { 0x90, 0x90 };
        //PatchBytes(0x1400F990E, patch, sizeof(patch));
    }

    // override fb::CinebotCamera constructor
    {
        auto f = &CinebotCamera::ctor;
        CinebotCamera::t_ctor target = reinterpret_cast<CinebotCamera::t_ctor>(0x1400EF920);
        MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&CinebotCamera::o_ctor)); MH_EnableHook(target);
    }

    // override fb::CinebotCamera::postFrameUpdate
    {
        auto f = &CinebotCamera::postFrameUpdate;
        CinebotCamera::t_postFrameUpdate target = reinterpret_cast<CinebotCamera::t_postFrameUpdate>(0x1400F7D30);
        MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&CinebotCamera::o_postFrameUpdate)); MH_EnableHook(target);
    }

    // override fb::CinebotCamera::commitShot
    {
        auto f = &CinebotCamera::commitShot;
        CinebotCamera::t_commitShot target = reinterpret_cast<CinebotCamera::t_commitShot>(0x1400F47F0);
        MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&CinebotCamera::o_commitShot)); MH_EnableHook(target);
    }
}

}