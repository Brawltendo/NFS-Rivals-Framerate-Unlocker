#pragma once
#include <Engine/Core/Types.h>


namespace fb
{

class Camera;


class CameraManager
{
public:
	Camera* getActiveCamera(uint viewIndex)
	{
		typedef Camera*(__fastcall* native)(CameraManager*, uint);
		return reinterpret_cast<native>(__vftable[5])(this, viewIndex);
	}

protected:
	void** __vftable;
};


extern CameraManager** g_cameraManager;

}