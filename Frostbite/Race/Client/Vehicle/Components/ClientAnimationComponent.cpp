#include <Race/Client/Vehicle/Components/ClientAnimationComponent.h>
#include <Race/Common/Vehicle/Components/AnimationComponentData.h>

#include <Core/Math/Math.h>
#include <Engine/Entity/UpdateInfo.h>

#include <Race/Client/Vehicle/Components/ClientRaceVehicleComponent.h>
#include <Engine/Physics/Vehicle/EAGR/IRaceVehicle.h>

#include "memoryutils.h"
#include "safetyhook.hpp"


namespace fb
{

void ClientAnimationComponent::applyInterpolation(float interpolationFactor)
{
    float interpolated;

    interpolated = lerp(m_airbrakeMovementPrev, m_airbrakeMovement, interpolationFactor);
    m_airbrakeMovementProperty = interpolated != m_airbrakeMovement ? interpolated : m_airbrakeMovement;

    interpolated = lerp(m_highSpeedWingMovementPrev, m_highSpeedWingMovement, interpolationFactor);
    m_highSpeedWingMovementProperty = interpolated != m_highSpeedWingMovement ? interpolated : m_highSpeedWingMovement;

    interpolated = lerp(m_airbrakeMovementSimplePrev, m_airbrakeMovementSimple, interpolationFactor);
    m_airbrakeMovementSimpleProperty = interpolated != m_airbrakeMovementSimple ? interpolated : m_airbrakeMovementSimple;

    interpolated = lerp(m_highSpeedWingMovementSimplePrev, m_highSpeedWingMovementSimple, interpolationFactor);
    m_highSpeedWingMovementSimpleProperty = interpolated != m_highSpeedWingMovementSimple ? interpolated : m_highSpeedWingMovementSimple;

    interpolated = lerp(m_airbrakeSimplePlusHighSpeedSimplePrev, m_airbrakeSimplePlusHighSpeedSimple, interpolationFactor);
    m_airbrakeSimplePlusHighSpeedSimpleProperty = interpolated != m_airbrakeSimplePlusHighSpeedSimple ? interpolated : m_airbrakeSimplePlusHighSpeedSimple;

    interpolated = lerp(m_airbrakeSimplePlusHighSpeedPrev, m_airbrakeSimplePlusHighSpeed, interpolationFactor);
    m_airbrakeSimplePlusHighSpeedProperty = interpolated != m_airbrakeSimplePlusHighSpeed ? interpolated : m_airbrakeSimplePlusHighSpeed;

    interpolated = lerp(m_airbrakePlusHighSpeedSimplePrev, m_airbrakePlusHighSpeedSimple, interpolationFactor);
    m_airbrakePlusHighSpeedSimpleProperty = interpolated != m_airbrakePlusHighSpeedSimple ? interpolated : m_airbrakePlusHighSpeedSimple;

    interpolated = lerp(m_airbrakePlusHighSpeedPrev, m_airbrakePlusHighSpeed, interpolationFactor);
    m_airbrakePlusHighSpeedProperty = interpolated != m_airbrakePlusHighSpeed ? interpolated : m_airbrakePlusHighSpeed;
}

void ClientAnimationComponent::onPostPhysicsUpdateSync(const UpdateInfo& updateInfo, ClientPlayer* player)
{
    const AnimationComponentData& data = *m_data;
    const float forwardSpeedMph = m_vehicleComponent->getVehiclePhysics()->getForwardSpeed() * 2.23699f;
    const float brake = m_vehicleComponent->getVehiclePhysics()->brake();
    const float wingUpSpeedMph = data.getWingUpSpeedMph();
    const float wingDownSpeedMph = data.getWingDownSpeedMph();
    const float airbrakeUpSpeedMph = data.getAirbrakeUpSpeedMph();
    const float airbrakeDownSpeedMph = data.getAirbrakeDownSpeedMph();

    const float brakeModifier = data.getBrakeScalar();
    const float brakeModified = (brake * brakeModifier) - 1.0;
    const float finalBrake = clamp(brakeModified, 0.f, 1.f);

    const float highSpeedMovementSpeedRange = wingUpSpeedMph - wingDownSpeedMph;
    const float highSpeedMovementFactor = (forwardSpeedMph - wingDownSpeedMph) / highSpeedMovementSpeedRange;
    const float highSpeedMovementFactorClamped = clamp(highSpeedMovementFactor, 0.f, 1.f);
    const float highSpeedMovementFinal = highSpeedMovementFactorClamped * data.getMaxWingMovement();
    float highSpeedWingMovementSimpleFinal = m_highSpeedWingMovementSimple;
    if (forwardSpeedMph > wingDownSpeedMph)
    {
        highSpeedWingMovementSimpleFinal = 1.f;
    }
    else if (forwardSpeedMph < wingDownSpeedMph)
    {
        highSpeedWingMovementSimpleFinal = 0.f;
    }

    const float airbrakeSpeedRange = airbrakeUpSpeedMph - airbrakeDownSpeedMph;
    const float airbrakeFactor = (forwardSpeedMph - airbrakeDownSpeedMph) / airbrakeSpeedRange;
    const float airbrakeFactorClamped = clamp(airbrakeFactor, 0.f, 1.f);
    const float airbrakeMovementFinal = airbrakeFactorClamped * finalBrake;

    float airbrakeMovementSimpleFinal = m_airbrakeMovementSimple;
    if (forwardSpeedMph > airbrakeUpSpeedMph)
    {
        airbrakeMovementSimpleFinal = 1.f;
    }
    else if (forwardSpeedMph < airbrakeDownSpeedMph)
    {
        airbrakeMovementSimpleFinal = 0.f;
    }
    airbrakeMovementSimpleFinal = clamp(airbrakeMovementSimpleFinal * finalBrake, 0.f, 1.f);

    float airbrakeMovementSpeed = data.getWingMovementSpeed();
    m_highSpeedWingMovementPrev = m_highSpeedWingMovement;
    m_highSpeedWingMovementSimplePrev = m_highSpeedWingMovementSimple;
    m_airbrakeMovementPrev = m_airbrakeMovement;
    m_airbrakeMovementSimplePrev = m_airbrakeMovementSimple;
    m_airbrakeSimplePlusHighSpeedSimplePrev = m_airbrakeSimplePlusHighSpeedSimple;
    m_airbrakeSimplePlusHighSpeedPrev = m_airbrakeSimplePlusHighSpeed;
    m_airbrakePlusHighSpeedSimplePrev = m_airbrakePlusHighSpeedSimple;
    m_airbrakePlusHighSpeedPrev = m_airbrakePlusHighSpeed;

    if (m_airbrakeMovement >= airbrakeMovementFinal)
    {
        float diff = airbrakeMovementFinal - m_airbrakeMovement;
        float speed = -(airbrakeMovementSpeed * updateInfo.deltaTime);
        m_airbrakeMovement += diff > speed ? diff : speed;
    }
    else
    {
        float diff = airbrakeMovementFinal - m_airbrakeMovement;
        float speed = airbrakeMovementSpeed * updateInfo.deltaTime;
        m_airbrakeMovement += diff < speed ? diff : speed;
    }

    if (m_highSpeedWingMovement >= highSpeedMovementFinal)
    {
        float diff = highSpeedMovementFinal - m_highSpeedWingMovement;
        float speed = -(airbrakeMovementSpeed * updateInfo.deltaTime);
        m_highSpeedWingMovement += diff > speed ? diff : speed;
    }
    else
    {
        float diff = highSpeedMovementFinal - m_highSpeedWingMovement;
        float speed = airbrakeMovementSpeed * updateInfo.deltaTime;
        m_highSpeedWingMovement += diff < speed ? diff : speed;
    }

    if (m_highSpeedWingMovementSimple >= highSpeedWingMovementSimpleFinal)
    {
        float diff = highSpeedWingMovementSimpleFinal - m_highSpeedWingMovement;
        float speed = -(airbrakeMovementSpeed * updateInfo.deltaTime);
        m_highSpeedWingMovementSimple += diff > speed ? diff : speed;
    }
    else
    {
        float diff = highSpeedWingMovementSimpleFinal - m_highSpeedWingMovement;
        float speed = airbrakeMovementSpeed * updateInfo.deltaTime;
        m_highSpeedWingMovementSimple += diff < speed ? diff : speed;
    }

    if (m_airbrakeMovementSimple >= airbrakeMovementSimpleFinal)
    {
        float diff = airbrakeMovementSimpleFinal - m_airbrakeMovementSimple;
        float speed = -(airbrakeMovementSpeed * updateInfo.deltaTime);
        m_airbrakeMovementSimple += diff > speed ? diff : speed;
    }
    else
    {
        float diff = airbrakeMovementSimpleFinal - m_airbrakeMovementSimple;
        float speed = airbrakeMovementSpeed * updateInfo.deltaTime;
        m_airbrakeMovementSimple += diff < speed ? diff : speed;
    }

    const float airbrakeSimplePlusHighSpeedSimple = clamp(m_airbrakeMovementSimple + m_highSpeedWingMovementSimple, 0.f, 1.f);
    if (m_airbrakeSimplePlusHighSpeedSimple >= airbrakeSimplePlusHighSpeedSimple)
    {
        float diff = airbrakeSimplePlusHighSpeedSimple - m_airbrakeSimplePlusHighSpeedSimple;
        float speed = -(airbrakeMovementSpeed * updateInfo.deltaTime);
        m_airbrakeSimplePlusHighSpeedSimple += diff > speed ? diff : speed;
    }
    else
    {
        float diff = airbrakeSimplePlusHighSpeedSimple - m_airbrakeSimplePlusHighSpeedSimple;
        float speed = airbrakeMovementSpeed * updateInfo.deltaTime;
        m_airbrakeSimplePlusHighSpeedSimple += diff < speed ? diff : speed;
    }

    const float airbrakeSimplePlusHighSpeed = clamp(m_airbrakeMovementSimple + highSpeedMovementFinal, 0.f, 1.f);
    if (m_airbrakeSimplePlusHighSpeed >= airbrakeSimplePlusHighSpeed)
    {
        float diff = airbrakeSimplePlusHighSpeed - m_airbrakeSimplePlusHighSpeed;
        float speed = -(airbrakeMovementSpeed * updateInfo.deltaTime);
        m_airbrakeSimplePlusHighSpeed += diff > speed ? diff : speed;
    }
    else
    {
        float diff = airbrakeSimplePlusHighSpeed - m_airbrakeSimplePlusHighSpeed;
        float speed = airbrakeMovementSpeed * updateInfo.deltaTime;
        m_airbrakeSimplePlusHighSpeed += diff < speed ? diff : speed;
    }

    const float airbrakePlusHighSpeedSimple = clamp(airbrakeMovementFinal + m_highSpeedWingMovementSimple, 0.f, 1.f);
    if (m_airbrakePlusHighSpeedSimple >= airbrakePlusHighSpeedSimple)
    {
        float diff = airbrakePlusHighSpeedSimple - m_airbrakePlusHighSpeedSimple;
        float speed = -(airbrakeMovementSpeed * updateInfo.deltaTime);
        m_airbrakePlusHighSpeedSimple += diff > speed ? diff : speed;
    }
    else
    {
        float diff = airbrakePlusHighSpeedSimple - m_airbrakePlusHighSpeedSimple;
        float speed = airbrakeMovementSpeed * updateInfo.deltaTime;
        m_airbrakePlusHighSpeedSimple += diff < speed ? diff : speed;
    }

    const float airbrakePlusHighSpeed = clamp(airbrakeMovementFinal + highSpeedMovementFinal, 0.f, 1.f);
    if (m_airbrakePlusHighSpeed >= airbrakePlusHighSpeed)
    {
        float diff = airbrakePlusHighSpeed - m_airbrakePlusHighSpeed;
        float speed = -(airbrakeMovementSpeed * updateInfo.deltaTime);
        m_airbrakePlusHighSpeed += diff > speed ? diff : speed;
    }
    else
    {
        float diff = airbrakePlusHighSpeed - m_airbrakePlusHighSpeed;
        float speed = airbrakeMovementSpeed * updateInfo.deltaTime;
        m_airbrakePlusHighSpeed += diff < speed ? diff : speed;
    }

    applyInterpolation(updateInfo.interpolationFactor);
}

void ClientAnimationComponent::onFrameInterpolationUpdate(const UpdateInfo& updateInfo)
{
    applyInterpolation(updateInfo.interpolationFactor);
}

void ClientAnimationComponent::ApplyPatches()
{
    // increase size of fb::ClientAnimationComponent from 0x2E8 to 0x308
    // with this we can add extra fields for interpolation
    {
        uint8_t patch1[] = { 0x08, 0x03 };
        PatchBytes(0x141FAB44A, patch1, sizeof(patch1));
        uint8_t patch2[] = { 0x08, 0x03 };
        PatchBytes(0x140866F0A, patch2, sizeof(patch2));
    }

    // mid function hook into ClientAnimationComponent's ComponentEntityCreator
    static SafetyHookMid animationComponentPatch1{};
    animationComponentPatch1 = safetyhook::create_mid(reinterpret_cast<void*>(0x140866F59),
        [](SafetyHookContext& ctx)
    {
        ClientAnimationComponent* component = reinterpret_cast<ClientAnimationComponent*>(ctx.rbx);

        if (component)
        {
            // have to do this in here because hooking the constructor crashes the game
            component->m_highSpeedWingMovementPrev = 0.f;
            component->m_highSpeedWingMovementSimplePrev = 0.f;
            component->m_airbrakeMovementPrev = 0.f;
            component->m_airbrakeMovementSimplePrev = 0.f;
            component->m_airbrakeSimplePlusHighSpeedSimplePrev = 0.f;
            component->m_airbrakeSimplePlusHighSpeedPrev = 0.f;
            component->m_airbrakePlusHighSpeedSimplePrev = 0.f;
            component->m_airbrakePlusHighSpeedPrev = 0.f;
        }
    });

    // redirect fb::ClientAnimationComponent::componentFrameInterpolationUpdateRequired vfptr
    // returns true so the component opts into the interpolation pass
    {
        uint8_t patch[] = { 0x40, 0x98, 0x75, 0x40, 0x01, 0x00, 0x00, 0x00 };
        PatchBytes(0x14154E718, patch, sizeof(patch));
    }

    // redirect fb::ClientAnimationComponent::onPostPhysicsUpdateSync vfptr to our patched version
    {
        auto f = &ClientAnimationComponent::onPostPhysicsUpdateSync;
        union { void* ptr; uint8_t arr[8]; } patch{};
        patch.ptr = *(void**)&f;
        PatchBytes(0x14154E780, patch.arr, sizeof(patch));
    }

    // redirect fb::ClientAnimationComponent::onFrameInterpolationUpdate vfptr to our patched version
    {
        auto f = &ClientAnimationComponent::onFrameInterpolationUpdate;
        union { void* ptr; uint8_t arr[8]; } patch{};
        patch.ptr = *(void**)&f;
        PatchBytes(0x14154E7A0, patch.arr, sizeof(patch));
    }
}

}