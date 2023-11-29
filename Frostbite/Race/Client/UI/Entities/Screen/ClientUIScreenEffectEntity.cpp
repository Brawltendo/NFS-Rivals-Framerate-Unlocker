#include <Race/Client/UI/Entities/Screen/ClientUIScreenEffectEntity.h>

#include <Engine/Entity/UpdateInfo.h>
#include <Engine/Entity/SimpleUpdater.h>
#include <Engine/Entity/UpdateManager.h>

#include <Engine/Game/Common/Gameplay/Camera/Camera.h>
#include <Engine/Game/Common/Gameplay/Camera/CameraManager.h>

#include <Engine/Core/Math/LinearTransform.h>

#include <Race/Client/Vehicle/ClientRaceVehicleEntity.h>

#include <../util/memoryutils.h>
#include "MinHook.h"


namespace fb
{

class UpdateQueryCollector;
class ClientUIScreenEffectEntityUpdater : public SimpleUpdater<ClientUIScreenEffectEntity>
{
public:
    void dtor(bool d)
    {
        auto nativeDelete = reinterpret_cast<void(__fastcall*)(ClientUIScreenEffectEntityUpdater*,bool)>(0x1407618E0);

        g_updateManagers[m_realm].manager[UpdatePass_PostSim].removeUpdater(this);
        nativeDelete(this, d);
    }

    void addToPass(UpdatePass pass)
    {
        g_updateManagers[m_realm].manager[pass].addUpdater(this);
    }

};


ClientUIScreenEffectEntityUpdater** ClientUIScreenEffectEntity::s_updater = reinterpret_cast<ClientUIScreenEffectEntityUpdater**>(0x141D996D0);

ClientUIScreenEffectEntity::t_ctor ClientUIScreenEffectEntity::o_ctor = nullptr;
ClientUIScreenEffectEntity* ClientUIScreenEffectEntity::ctor(CreationInfo* info, const UIScreenEffectEntityData& data)
{
    ClientUIScreenEffectEntity* ret = o_ctor(this, info, data);
    m_prevVehicleAccel = vecZero();
    m_currentVehicleAccel = vecZero();
    m_prevVehicleVel = vecZero();
    m_currentVehicleVel = vecZero();

    return ret;
}

ClientUIScreenEffectEntity::t_onInit ClientUIScreenEffectEntity::o_onInit = nullptr;
void ClientUIScreenEffectEntity::onInit(const EntityInitInfo& info)
{
    bool isFirstInit = (*s_updater) == nullptr;
    o_onInit(this, info);

    // will be initialized by this point, only needs to be added once
    if (isFirstInit)
        (*s_updater)->addToPass(UpdatePass_PostSim);
}

void ClientUIScreenEffectEntity::update(const UpdateInfo& info)
{
    ClientRaceVehicleEntity* vehicle;
    {
        char* bytes = reinterpret_cast<char*>(m_vehicleProxy);
        vehicle = *reinterpret_cast<ClientRaceVehicleEntity**>(bytes + 0xB8);
    }

    if (vehicle && vehicle->hasLocalPlayer())
    {
        Camera* activeCamera = (*g_cameraManager)->getActiveCamera(0);
        const LinearTransform& cameraTransform = activeCamera->getTransform();
        

        if (info.updatePass == UpdatePass_PostSim)
        {
            m_prevVehicleVel = m_currentVehicleVel;
            m_currentVehicleVel = vehicle->linearVelocity();
            m_prevVehicleAccel = m_currentVehicleAccel;
            m_currentVehicleAccel = vehicle->accelerationWorldSpace();
            bool prevIsElectrified = m_isElectrified;
            m_isElectrified = vehicle->electrifiedAmount() > 0.000001f;

            if (m_isElectrified != prevIsElectrified)
            {
                auto EntityBusPeer_internalFireEvent = reinterpret_cast<void(__fastcall*)(void*, void*, void*, int)>(0x140240550);
                EntityBusPeer_internalFireEvent(this, m_entityBus, m_data, prevIsElectrified ? 0x8CD5957B : 0x8CB26B33);
            }
        }

        Vec lerpVel = VecLerp(m_prevVehicleVel, m_currentVehicleVel, info.interpolationFactor);
        if (!VecIsTrueXYZ(_mm_cmpeq_ps(lerpVel, m_currentVehicleVel)))
        {
            Vec3 cameraSpaceVel(invRotate(lerpVel, cameraTransform));
            m_velocity = cameraSpaceVel;
        }


        Vec lerpAccel = VecLerp(m_prevVehicleAccel, m_currentVehicleAccel, info.interpolationFactor);
        if (!VecIsTrueXYZ(_mm_cmpeq_ps(lerpAccel, m_currentVehicleAccel)))
        {
            Vec3 cameraSpaceAcc(invRotate(lerpAccel, cameraTransform));
            m_acceleration = cameraSpaceAcc;
        }

        Vec3 vehicleImpulse = vehicle->impulseNormal().simdValue * vehicle->impulseMagnitude();
        m_vehicleImpulse = vehicleImpulse;

        if (info.updatePass == UpdatePass_PostFrame)
        {
            auto MathUtil_getRotationZ = reinterpret_cast<float(__fastcall*)(const LinearTransform*)>(0x1402AEC20);
            float rotationZ = MathUtil_getRotationZ(&cameraTransform);
            float cameraAngVelZ = (rotationZ - m_prevRotationZ) / (info.deltaTime + m_prevDeltaTime);
            m_cameraAngularVelocityZ = cameraAngVelZ;
            m_prevRotationZ = m_rotationZ;
            m_rotationZ = rotationZ;
            m_prevDeltaTime = info.deltaTime;
        }        
    }
}

void ClientUIScreenEffectEntity::ApplyPatches()
{
    // increase size of fb::ClientUIScreenEffectEntity from 0x78 to 0xB0
    // with this we can add extra fields for interpolation
    {
        uint8_t patch1[] = { 0xB0 };
        PatchBytes(0x1409873A1, patch1, sizeof(patch1));
        uint8_t patch2[] = { 0xB0 };
        PatchBytes(0x141FC059A, patch2, sizeof(patch2));
    }

    // override fb::ClientUIScreenEffectEntity constructor
    {
        auto f = &ClientUIScreenEffectEntity::ctor;
        ClientUIScreenEffectEntity::t_ctor target = reinterpret_cast<ClientUIScreenEffectEntity::t_ctor>(0x1409847F0);
        MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&ClientUIScreenEffectEntity::o_ctor)); MH_EnableHook(target);
    }

    // override fb::ClientUIScreenEffectEntity::onInit
    {
        auto f = &ClientUIScreenEffectEntity::onInit;
        ClientUIScreenEffectEntity::t_onInit target = reinterpret_cast<ClientUIScreenEffectEntity::t_onInit>(0x14098C150);
        MH_CreateHook(target, *(void**)&f, reinterpret_cast<LPVOID*>(&ClientUIScreenEffectEntity::o_onInit)); MH_EnableHook(target);
    }

    // redirect ~fb::SimpleUpdater<fb::ClientUIScreenEffectEntity> vfptr to our patched version
    {
        auto f = &ClientUIScreenEffectEntityUpdater::dtor;
        union { void* ptr; uint8_t arr[8]; } patch{};
        patch.ptr = *(void**)&f;
        PatchBytes(0x14159EBA0, patch.arr, sizeof(patch));
    }

    // redirect fb::SimpleUpdater<fb::ClientUIScreenEffectEntity>::postQueryUpdate vfptr to our patched version
    {
        auto f = &ClientUIScreenEffectEntityUpdater::postQueryUpdate;
        union { void* ptr; uint8_t arr[8]; } patch{};
        patch.ptr = *(void**)&f;
        PatchBytes(0x14159EBB0, patch.arr, sizeof(patch));
    }
}

}