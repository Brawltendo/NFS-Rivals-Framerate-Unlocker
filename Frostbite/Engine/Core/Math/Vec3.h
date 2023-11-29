#pragma once
#include "Vec.h"


namespace fb
{

struct Vec3
{
	typedef const Vec3& Vec3Ref;

	union
	{
		__m128 simdValue;
		struct
		{
			float x, y, z, pad;
		};
		float data[4];
	};

	// Initializes this Vec3 to zero
	Vec3()
	{
		simdValue = _mm_setzero_ps();
	}

	// Initializes all members of this vector with the value of in
	Vec3(float in)
	{
		simdValue = _mm_set1_ps(in);
	}

	// Initializes xyz and sets pad to zero
	Vec3(float inX, float inY, float inZ)
	{
		simdValue = _mm_setr_ps(inX, inY, inZ, 0.f);
	}

	// Initializes vector with the values of another Vec3
	Vec3(const Vec3& v)
	{
		simdValue = v.simdValue;
	}

	// Initializes this Vec3 with a SIMD value
	Vec3(const __m128& simdIn)
	{
		simdValue = simdIn;
	}

	float& operator[](int i)
	{
		return simdValue.m128_f32[i];
	}

	Vec3  operator +  (const Vec3& b) { return simdValue + b.simdValue; }

	Vec3  operator +  (const Vec3& b) const { return simdValue + b.simdValue; }

	Vec3& operator += (const Vec3& b) { simdValue = simdValue + b.simdValue; return *this; }

	Vec3  operator -  (const Vec3& b) { return simdValue - b.simdValue; }

	Vec3  operator -  (const Vec3& b) const { return simdValue - b.simdValue; }

	Vec3& operator -= (const Vec3& b) { simdValue = simdValue - b.simdValue; return *this; }

	Vec3  operator *  (const Vec3& b) { return simdValue * b.simdValue; }

	Vec3  operator *  (const Vec3& b) const { return simdValue * b.simdValue; }

	Vec3& operator *= (const Vec3& b) { simdValue = simdValue * b.simdValue; return *this; }

	Vec3  operator /  (const Vec3& b) { return simdValue / b.simdValue; }

	Vec3  operator /  (const Vec3& b) const { return simdValue / b.simdValue; }

	Vec3& operator /= (const Vec3& b) { simdValue = simdValue / b.simdValue; return *this; }

	operator __m128() { return simdValue; }

	operator __m128() const { return simdValue; }

};

typedef Vec3::Vec3Ref Vec3Ref;

}
