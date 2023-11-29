#pragma once
#include "../../../../Shared/WeaponDataDefs.h"


namespace fb
{

class FovEffect
{
public:
	float m_fov;
	float m_fadeTime;
	float m_delay;
	FOVTransitionType style;
	float m_styleShape;
	float m_styleStartDelay;
	float m_styleStartJump;
	float m_styleStartEarly;
	bool m_styleInvert;
	bool m_active;
};
static_assert(sizeof(FovEffect) == 0x24, "Wrong size!");

}