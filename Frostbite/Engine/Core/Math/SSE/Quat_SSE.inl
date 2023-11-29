#pragma once
#include "../LinearTransform.h"
#include <math.h>


namespace fb
{

__forceinline Quat::Quat(Vec3Ref axis, float radians)
{
	float angSin = sinf(radians * 0.5f);
	float angCos = cosf(radians * 0.5f);
	vec = _mm_insert_ps(axis.simdValue * angSin, _mm_set1_ps(angCos), 0b00'11'0000); // insert angCos into w
}

__forceinline Quat::Quat(const LinearTransform& t)
{
	auto native = reinterpret_cast<Quat*(__fastcall*)(Quat*, const LinearTransform&)>(0x140134690);
	native(this, t);
}

__forceinline float dot(QuatRef q0, QuatRef q1)
{
    return VecToFloat(VecDot4(q0.vec, q1.vec));
}

__forceinline Quat slerp(QuatRef q0, QuatRef q1, float t)
{
    float a = 1.0f - t;
    float b = t;
    float d = dot(q0, q1);
    float s = d < 0.f ? -1.f : 1.f;
    float c = d * s;

    if ((1.f - c) > 0.000001f)
    {
        c  = acosf(c);
        b  = 1.f / sinf(c);
        a  = sinf(c * a) * b;
        b *= sinf(c * t);
    }
    b *= s;
    
    Vec interp = (q0.vec.simdValue * a) + (q1.vec.simdValue * b);

    return Quat(interp);
}

}