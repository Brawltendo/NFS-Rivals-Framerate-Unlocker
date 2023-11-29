#pragma once
#include "Game/Common/Gameplay/Camera/TargetCamera.h"


namespace fb
{

struct CinebotCameraShot;
class ICinebotDirector;
class CinebotCameraData;


class CinebotCamera : public TargetCamera
{
public:
	static void ApplyPatches();

	// fake constructor
	CinebotCamera* ctor(CreationInfo& info, const CinebotCameraData& cameraData);

	void postSimUpdate(const CameraUpdateInfo& info);
	void postFrameUpdate(const CameraUpdateInfo& info);

	void setNearPlaneOverride(float nearPlaneOverride)
	{
		typedef void(__fastcall* native)(CinebotCamera*, float);
		reinterpret_cast<native>(__vftable[30])(this, nearPlaneOverride);
	}

	void setFarPlaneOverride(float farPlaneOverride)
	{
		typedef void(__fastcall* native)(CinebotCamera*, float);
		reinterpret_cast<native>(__vftable[31])(this, farPlaneOverride);
	}

	typedef CinebotCamera*(__fastcall* t_ctor)(CinebotCamera*, CreationInfo&, const CinebotCameraData&);
	static t_ctor o_ctor;

	typedef void (__fastcall* t_postFrameUpdate)(CinebotCamera*, const CameraUpdateInfo&);
	static t_postFrameUpdate o_postFrameUpdate;

	typedef void (__fastcall* t_commitShot)(CinebotCamera*, const CinebotCameraShot&);
	static t_commitShot o_commitShot;

private:
	void commitShot(const CinebotCameraShot& shot);

	char gap_D0[1192];
	ICinebotDirector* m_director;
	char gap_580[240];

	// this is badddddddd but I ran into problems when they were actual fields
	// or maybe something else was crashing the game that I fixed
	static Quat m_prevOrientation;
	static Quat m_currentOrientation;
	static Vec3 m_prevPosition;
	static Vec3 m_currentPosition;
	static float m_prevFov;
	static float m_currentFov;
	static bool m_cameraCut;
	static bool m_useFrameInterpolation;
};
static_assert(sizeof(CinebotCamera) == 0x670, "Wrong size!");

}
