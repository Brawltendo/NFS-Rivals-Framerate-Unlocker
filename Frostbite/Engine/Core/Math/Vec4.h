#pragma once
#include "Vec.h"


namespace fb
{

struct Vec4
{
	typedef const Vec4& Vec4Ref;

	static const Vec4 kZero;
	static const Vec4 kQuarter;
	static const Vec4 kHalf;
	static const Vec4 kOne;
	static const Vec4 kMinusOne;
	static const Vec4 kEpsilon;
	static const Vec4 kUnitX;
	static const Vec4 kUnitY;
	static const Vec4 kUnitZ;
	static const Vec4 kUnitW;

	union
	{
		__m128 simdValue;
		struct
		{
			float x, y, z, w;
		};
		float data[4];
	};

	// Initializes this Vec4 to zero
	Vec4()
	{
		simdValue = _mm_setzero_ps();
	}

	// Initializes all members of this vector with the value of in
	Vec4(float in)
	{
		simdValue = _mm_set1_ps(in);
	}

	// Initializes xyz and sets w to zero
	Vec4(float inX, float inY, float inZ)
	{
		simdValue = _mm_setr_ps(inX, inY, inZ, 0.f);
	}

	// Initializes xyzw
	Vec4(float inX, float inY, float inZ, float inW)
	{
		simdValue = _mm_setr_ps(inX, inY, inZ, inW);
	}

	// Initializes vector with the values of another Vec4
	Vec4(const Vec4& v)
	{
		simdValue = v.simdValue;
	}

	// Initializes this Vec4 with a SIMD value
	Vec4(const __m128& simdIn)
	{
		simdValue = simdIn;
	}

	float& operator[](int i)
	{
		return simdValue.m128_f32[i];
	}

	Vec4  operator +  (const Vec4& b) { return simdValue + b.simdValue; }

	Vec4  operator +  (const Vec4& b) const { return simdValue + b.simdValue; }

	Vec4& operator += (const Vec4& b) { simdValue = simdValue + b.simdValue; return *this; }

	Vec4  operator -  (const Vec4& b) { return simdValue - b.simdValue; }

	Vec4  operator -  (const Vec4& b) const { return simdValue - b.simdValue; }

	Vec4& operator -= (const Vec4& b) { simdValue = simdValue - b.simdValue; return *this; }

	Vec4  operator *  (const Vec4& b) { return simdValue * b.simdValue; }

	Vec4  operator *  (const Vec4& b) const { return simdValue * b.simdValue; }

	Vec4& operator *= (const Vec4& b) { simdValue = simdValue * b.simdValue; return *this; }

	Vec4  operator /  (const Vec4& b) { return simdValue / b.simdValue; }

	Vec4  operator /  (const Vec4& b) const { return simdValue / b.simdValue; }

	Vec4& operator /= (const Vec4& b) { simdValue = simdValue / b.simdValue; return *this; }

	operator __m128() { return simdValue; }

	operator __m128() const { return simdValue; }

};

typedef Vec4::Vec4Ref Vec4Ref;

}