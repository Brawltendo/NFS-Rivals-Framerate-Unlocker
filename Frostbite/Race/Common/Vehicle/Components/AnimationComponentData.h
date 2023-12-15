#pragma once
#include <Engine/Entity/EntityData.h>


namespace fb
{

class AnimationComponentData : public GameComponentData
{
public:
	float m_brakeScalar;
	float m_wingUpSpeedMph;
	float m_wingDownSpeedMph;
	float m_airbrakeUpSpeedMph;
	float m_airbrakeDownSpeedMph;
	float m_maxWingMovement;
	float m_wingMovementSpeed;

public:
	float getBrakeScalar() const { return m_brakeScalar; }
	float getWingUpSpeedMph() const { return m_wingUpSpeedMph; }
	float getWingDownSpeedMph() const { return m_wingDownSpeedMph; }
	float getAirbrakeUpSpeedMph() const { return m_airbrakeUpSpeedMph; }
	float getAirbrakeDownSpeedMph() const { return m_airbrakeDownSpeedMph; }
	float getMaxWingMovement() const { return m_maxWingMovement; }
	float getWingMovementSpeed() const { return m_wingMovementSpeed; }
};
static_assert(sizeof(AnimationComponentData) == 0x90, "Wrong size!");

}