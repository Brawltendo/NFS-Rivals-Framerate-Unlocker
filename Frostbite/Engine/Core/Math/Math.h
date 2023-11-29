#pragma once


namespace fb
{

template <class T>
__forceinline T lerp(T a, T b, T t)
{
	return static_cast<T>((1.f - t) * a + t * b);
}

}