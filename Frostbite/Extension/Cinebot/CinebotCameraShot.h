#pragma once
#include "Core/Math/Vec3.h"
#include "Core/Math/Vec2.h"


namespace fb
{

struct LinearTransform;

struct CinebotCameraShot
{
	Vec3 position;
	Vec3 target;
	Vec3 focus;
	Vec3 up;
	Vec3 hudShake;
	float hudScale;
	Vec2 viewPortOffset;
	float nearPlane;
	float farPlane;
	float roll;
	float fov;
	float focalPlaneOffset;
	float fStop;
	float minimumNearBlur;
	float timeScale;
	float surfaceNoise;
	bool valid;
	Vec3 safeTarget;
	bool safeTargetValid;

	void calcTransform(LinearTransform& transform) const
	{
		auto native = reinterpret_cast<void(__fastcall*)(const CinebotCameraShot*, const LinearTransform&)>(0x1400F4560);
		native(this, transform);
	}

};

}