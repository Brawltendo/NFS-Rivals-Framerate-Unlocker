#pragma once
#include "Vec3.h"


namespace fb
{

struct Quat;


struct LinearTransform
{
	enum IdentityType  { Identity };
	enum RotationAxisX { RotateX };
	enum RotationAxisY { RotateY };
	enum RotationAxisZ { RotateZ };

	LinearTransform()
	{
	}

	LinearTransform(IdentityType type);
	LinearTransform(RotationAxisX axis, float radians);
	LinearTransform(RotationAxisY axis, float radians);
	LinearTransform(RotationAxisZ axis, float radians);
	LinearTransform(const LinearTransform& t);
	LinearTransform(Vec3Ref inLeft, Vec3Ref inUp, Vec3Ref inForward, Vec3Ref inTrans);
	LinearTransform(const Quat& r, Vec3Ref t);

	LinearTransform operator*(const LinearTransform& T) const;
	bool operator==(const LinearTransform& T) const;
	bool operator!=(const LinearTransform& T) const;

	Vec3 left;
	Vec3 up;
	Vec3 forward;
	Vec3 trans;
};
static_assert(sizeof(LinearTransform) == 0x40, "LinearTransform");


LinearTransform lerp(const LinearTransform& T0, const LinearTransform& T1, float t);

}

#include "SSE/LinearTransform_SSE.inl"
