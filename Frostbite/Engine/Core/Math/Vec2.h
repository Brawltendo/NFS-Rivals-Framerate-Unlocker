#pragma once


namespace fb
{

struct Vec2
{
	typedef const Vec2& Vec2Ref;

	union
	{
		struct
		{
			float x, y;
		};
		float data[2];
	};

	// Initializes this Vec2 to zero
	Vec2()
	: x(0.f)
	, y(0.f)
	{
	}

	// Initializes all members of this vector with the value of in
	Vec2(float in)
		: x(in)
		, y(in)
	{
	}

	// Initializes xy
	Vec2(float inX, float inY)
		: x(inX)
		, y(inY)
	{
	}

	// Initializes vector with the values of another Vec2
	Vec2(Vec2Ref v)
		: x(v.x)
		, y(v.y)
	{
	}

	Vec2 operator + (Vec2Ref b)
	{
		Vec2 v;
		v.x = x + b.x;
		v.y = y + b.y;
		return v;
	}

	Vec2 operator * (float b)
	{
		Vec2 v;
		v.x = x * b;
		v.y = y * b;
		return v;
	}

};
static_assert(sizeof(Vec2) == 0x8, "Wrong size!");

typedef Vec2::Vec2Ref Vec2Ref;

}