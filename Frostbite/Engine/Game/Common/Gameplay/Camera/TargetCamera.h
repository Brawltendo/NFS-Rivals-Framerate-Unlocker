#pragma once
#include "Camera.h"


namespace fb
{

class ViewFxData;

class TargetCamera : public Camera
{
protected:
	void* m_callback;
	ViewFxData* m_viewFxData;
};
static_assert(sizeof(TargetCamera) == 0xD0, "Wrong size!");

}
