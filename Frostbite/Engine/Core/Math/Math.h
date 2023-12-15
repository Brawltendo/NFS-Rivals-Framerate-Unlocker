#pragma once


namespace fb
{

template <class T>
__forceinline T min(T a, T b)
{
	return static_cast<T>(b <= a ? b : a);
}

template <class T>
__forceinline T max(T a, T b)
{
	return static_cast<T>(a <= b ? b : a);
}

template <class T>
__forceinline T clamp(T val, T low, T high)
{
	return static_cast<T>(min(max(low, val), high));
}

template <class T>
__forceinline T lerp(T a, T b, T t)
{
	return static_cast<T>((1.f - t) * a + t * b);
}

}