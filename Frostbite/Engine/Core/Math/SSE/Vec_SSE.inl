#pragma once
#include <cstdint>


namespace fb
{

#pragma region __m128 Operator Overloads

__forceinline Vec  operator +   (const Vec& l, const Vec& r) { return _mm_add_ps(l, r); }
__forceinline Vec  operator -   (const Vec& l, const Vec& r) { return _mm_sub_ps(l, r); }
__forceinline Vec  operator *   (const Vec& l, const Vec& r) { return _mm_mul_ps(l, r); }
__forceinline Vec  operator /   (const Vec& l, const Vec& r) { return _mm_div_ps(l, r); }
__forceinline Vec& operator +=  (Vec& l, const Vec& r) { l = l + r; return l; }
__forceinline Vec& operator -=  (Vec& l, const Vec& r) { l = l - r; return l; }
__forceinline Vec& operator *=  (Vec& l, const Vec& r) { l = l * r; return l; }
__forceinline Vec& operator /=  (Vec& l, const Vec& r) { l = l / r; return l; }
__forceinline Vec  operator &   (const Vec& l, const Vec& r) { return _mm_and_ps(l, r); }
__forceinline Vec  operator |   (const Vec& l, const Vec& r) { return _mm_or_ps(l, r); }
__forceinline Vec  operator ^   (const Vec& l, const Vec& r) { return _mm_xor_ps(l, r); }
__forceinline Vec  operator <   (const Vec& l, const Vec& r) { return _mm_cmplt_ps(l, r); }
__forceinline Vec  operator >   (const Vec& l, const Vec& r) { return _mm_cmpgt_ps(l, r); }
__forceinline Vec  operator <=  (const Vec& l, const Vec& r) { return _mm_cmple_ps(l, r); }
__forceinline Vec  operator >=  (const Vec& l, const Vec& r) { return _mm_cmpge_ps(l, r); }
__forceinline Vec  operator ==  (const Vec& l, const Vec& r) { return _mm_cmpeq_ps(l, r); }
__forceinline Vec  operator +   (const Vec& l, const float r) { return _mm_add_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator -   (const Vec& l, const float r) { return _mm_sub_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator *   (const Vec& l, const float r) { return _mm_mul_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator /   (const Vec& l, const float r) { return _mm_div_ps(l, _mm_set1_ps(r)); }
__forceinline Vec& operator +=  (Vec& l, const float r) { l = l + r; return l; }
__forceinline Vec& operator -=  (Vec& l, const float r) { l = l - r; return l; }
__forceinline Vec& operator *=  (Vec& l, const float r) { l = l * r; return l; }
__forceinline Vec& operator /=  (Vec& l, const float r) { l = l / r; return l; }
__forceinline Vec  operator &   (const Vec& l, const float r) { return _mm_and_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator |   (const Vec& l, const float r) { return _mm_or_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator ^   (const Vec& l, const float r) { return _mm_xor_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator <   (const Vec& l, const float r) { return _mm_cmplt_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator >   (const Vec& l, const float r) { return _mm_cmpgt_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator <=  (const Vec& l, const float r) { return _mm_cmple_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator >=  (const Vec& l, const float r) { return _mm_cmpge_ps(l, _mm_set1_ps(r)); }
__forceinline Vec  operator ==  (const Vec& l, const float r) { return _mm_cmpeq_ps(l, _mm_set1_ps(r)); }

#pragma endregion __m128 Operator Overloads



#pragma region Vector Initializers and Constants

// Creates vector with all values set to v
__forceinline Vec vec(float v)
{
	return _mm_set1_ps(v);
}

// Creates vector using xyz and sets w to zero
__forceinline Vec vec(float x_, float y_, float z_)
{
	Vec x = _mm_set_ss(x_);
	Vec y = _mm_set_ss(y_);
	Vec z = _mm_set_ss(z_);
	return _mm_unpacklo_ps(_mm_unpacklo_ps(x, z), y);
}

// Creates vector using xyzw
__forceinline Vec vec(float x_, float y_, float z_, float w_)
{
	Vec x = _mm_set_ss(x_);
	Vec y = _mm_set_ss(y_);
	Vec z = _mm_set_ss(z_);
	Vec w = _mm_set_ss(w_);
	return _mm_unpacklo_ps(_mm_unpacklo_ps(x, z), _mm_unpacklo_ps(y, w));
}

__forceinline Vec vecZero()     { return _mm_setzero_ps(); }
__forceinline Vec vecQuarter()  { return _mm_set1_ps(0.25f); }
__forceinline Vec vecHalf()     { return _mm_set1_ps(0.5f); }
__forceinline Vec vecOne()      { return _mm_set1_ps(1.f); }
__forceinline Vec vecMinusOne() { return _mm_set1_ps(-1.f); }
__forceinline Vec vecEpsilon()  { return _mm_set1_ps(0.00000011920929f); }
__forceinline Vec vecUnitX()    { return _mm_setr_ps(1.f, 0.f, 0.f, 0.f); }
__forceinline Vec vecUnitY()    { return _mm_setr_ps(0.f, 1.f, 0.f, 0.f); }
__forceinline Vec vecUnitZ()    { return _mm_setr_ps(0.f, 0.f, 1.f, 0.f); }
__forceinline Vec vecUnitW()    { return _mm_setr_ps(0.f, 0.f, 0.f, 1.f); }

	#pragma region Trig Constants

	__forceinline Vec vecNegOneOverPi() { return _mm_set1_ps(-0.15915494f); }
	__forceinline Vec vecCosXCoef()     { return _mm_set1_ps(6.2831855f); }
	__forceinline Vec vecCosX3Coef()    { return _mm_set1_ps(-41.341702f); }
	__forceinline Vec vecCosX5Coef()    { return _mm_set1_ps(81.605247f); }
	__forceinline Vec vecCosX7Coef()    { return _mm_set1_ps(-76.705856f); }
	__forceinline Vec vecCosX9Coef()    { return _mm_set1_ps(42.058693f); }
	__forceinline Vec vecCosX11Coef()   { return _mm_set1_ps(-15.094643f); }
	__forceinline Vec vecCosX13Coef()   { return _mm_set1_ps(3.8199525f); }
	__forceinline Vec vecCosX15Coef()   { return _mm_set1_ps(-0.7181223f); }
	__forceinline Vec vecCosX17Coef()   { return _mm_set1_ps(0.10422916f); }
	__forceinline Vec vecCosX19Coef()   { return _mm_set1_ps(-0.012031586f); }

	#pragma endregion Trig Constants


#pragma endregion Vector Initializers and Constants



#pragma region Vector Comparisons

// Returns if the input mask is true or false (all members)
__forceinline bool VecIsTrue(const Vec& v)
{
	return _mm_movemask_ps(v) == 0xF;
}

// Returns if the input mask is true or false (specified members)
// Use CmpMask to create the input mask
__forceinline bool VecIsTrue(const Vec& v, uint8_t cmpMask)
{
	return _mm_movemask_ps(v) == cmpMask;
}

// Returns if the input mask is true or false (any members)
__forceinline bool VecIsTrueAny(const Vec& v)
{
	return _mm_movemask_ps(v) != 0;
}

// Returns if the input mask is true or false (XYZ)
__forceinline bool VecIsTrueXYZ(const Vec& v)
{
	return (_mm_movemask_ps(v) & 0x7) == 0x7;
}

// Returns true if a < b (all members)
__forceinline bool VecCmpLT(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmplt_ps(a, b);
	return _mm_movemask_ps(cmp) == 0xF;
}

// Returns true if a < b (specified members)
// Use CmpMask to create the input mask
__forceinline bool VecCmpLT(const Vec& a, const Vec& b, uint8_t cmpMask)
{
	Vec cmp = _mm_cmplt_ps(a, b);
	return (_mm_movemask_ps(cmp) & cmpMask) == cmpMask;
}

// Returns true if a < b (any members)
__forceinline bool VecCmpLTAny(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmplt_ps(a, b);
	return _mm_movemask_ps(cmp) != 0;
}

// Returns true if a <= b (all members)
__forceinline bool VecCmpLE(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmple_ps(a, b);
	return _mm_movemask_ps(cmp) == 0xF;
}

// Returns true if a <= b (specified members)
// Use CmpMask to create the input mask
__forceinline bool VecCmpLE(const Vec& a, const Vec& b, uint8_t cmpMask)
{
	Vec cmp = _mm_cmple_ps(a, b);
	return (_mm_movemask_ps(cmp) & cmpMask) == cmpMask;
}

// Returns true if a <= b (any members)
__forceinline bool VecCmpLEAny(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmple_ps(a, b);
	return _mm_movemask_ps(cmp) != 0;
}

// Returns true if a > b (all members)
__forceinline bool VecCmpGT(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmpgt_ps(a, b);
	return _mm_movemask_ps(cmp) == 0xF;
}

// Returns true if a > b (specified members)
// Use CmpMask to create the input mask
__forceinline bool VecCmpGT(const Vec& a, const Vec& b, uint8_t cmpMask)
{
	Vec cmp = _mm_cmpgt_ps(a, b);
	return (_mm_movemask_ps(cmp) & cmpMask) == cmpMask;
}

// Returns true if a > b (any members)
__forceinline bool VecCmpGTAny(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmpgt_ps(a, b);
	return _mm_movemask_ps(cmp) != 0;
}

// Returns true if a >= b (all members)
__forceinline bool VecCmpGE(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmpge_ps(a, b);
	return _mm_movemask_ps(cmp) == 0xF;
}

// Returns true if a >= b (specified members)
// Use CmpMask to create the input mask
__forceinline bool VecCmpGE(const Vec& a, const Vec& b, uint8_t cmpMask)
{
	Vec cmp = _mm_cmpge_ps(a, b);
	return (_mm_movemask_ps(cmp) & cmpMask) == cmpMask;
}

// Returns true if a >= b (any members)
__forceinline bool VecCmpGEAny(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmpge_ps(a, b);
	return _mm_movemask_ps(cmp) != 0;
}

// Returns true if a == b (all members)
__forceinline bool VecCmpEQ(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmpeq_ps(a, b);
	return _mm_movemask_ps(cmp) == 0xF;
}

// Returns true if a == b (specified members)
// Use CmpMask to create the input mask
__forceinline bool VecCmpEQ(const Vec& a, const Vec& b, uint8_t cmpMask)
{
	Vec cmp = _mm_cmpeq_ps(a, b);
	return (_mm_movemask_ps(cmp) & cmpMask) == cmpMask;
}

// Returns true if a == b (any members)
__forceinline bool VecCmpEQAny(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmpeq_ps(a, b);
	return _mm_movemask_ps(cmp) != 0;
}

// Returns true if a != b (all members)
__forceinline bool VecCmpNEQ(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmpneq_ps(a, b);
	return _mm_movemask_ps(cmp) == 0xF;
}

// Returns true if a != b (specified members)
// Use CmpMask to create the input mask
__forceinline bool VecCmpNEQ(const Vec& a, const Vec& b, uint8_t cmpMask)
{
	Vec cmp = _mm_cmpneq_ps(a, b);
	return (_mm_movemask_ps(cmp) & cmpMask) == cmpMask;
}

// Returns true if a != b (any members)
__forceinline bool VecCmpNEQAny(const Vec& a, const Vec& b)
{
	Vec cmp = _mm_cmpneq_ps(a, b);
	return _mm_movemask_ps(cmp) != 0;
}

#pragma endregion Vector Comparisons



#pragma region Math Operations

// Adds 2 vectors
__forceinline Vec VecAdd(const Vec& l, const Vec& r)
{
	return _mm_add_ps(l, r);
}

// Subtracts 2 vectors
__forceinline Vec VecSub(const Vec& l, const Vec& r)
{
	return _mm_sub_ps(l, r);
}

// Multiplies 2 vectors
__forceinline Vec VecMul(const Vec& l, const Vec& r)
{
	return _mm_mul_ps(l, r);
}

// Divides 2 vectors
__forceinline Vec VecDiv(const Vec& l, const Vec& r)
{
	return _mm_div_ps(l, r);
}

#pragma endregion Math Operations



#pragma region Logical Operations

__forceinline Vec VecAnd(const Vec& l, const Vec& r)
{
	return _mm_and_ps(l, r);
}

__forceinline Vec VecAndNot(const Vec& l, const Vec& r)
{
	return _mm_andnot_ps(l, r);
}

__forceinline Vec VecOr(const Vec& l, const Vec& r)
{
	return _mm_or_ps(l, r);
}

__forceinline Vec VecXor(const Vec& l, const Vec& r)
{
	return _mm_xor_ps(l, r);
}

__forceinline Vec VecNot(const Vec& v)
{
	Vec notMask = _mm_castsi128_ps(_mm_set1_epi32(0xFFFFFFFF));
	return _mm_xor_ps(v, notMask);
}

#pragma endregion Logical Operations


#pragma region Swizzling

// the mask value has to be a compile-time constant
// therefore these need to be templates

template <EVecSwizzle InMask>
__forceinline Vec VecShuffle(const Vec& v)
{
	return _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), InMask));
}

template <EVecSwizzle InMask>
__forceinline Vec VecShuffle(const Vec& v1, const Vec& v2)
{
	return _mm_shuffle_ps(v1, v2, InMask);
}

#pragma endregion Swizzling


#pragma region Misc

// Returns the value in v.x
__forceinline float VecToFloat(const Vec& v)
{
	return _mm_cvtss_f32(v);
}

// Converts a bool input to an equivalent vector mask
__forceinline Vec BoolToVecMask(bool condition)
{
	union { uint32_t ui; float flt; } select[2];
	select[0].ui = 0; select[1].ui = 0xFFFFFFFF;
	return vec(select[condition ? 1 : 0].flt);
}

// Returns a vector based on the input condition
__forceinline Vec VecSelect(const Vec& valIfTrue, const Vec& valIfFalse, const Vec& condition)
{
	return _mm_xor_ps(_mm_and_ps(_mm_xor_ps(valIfTrue, valIfFalse), condition), valIfFalse);
}

// Returns the minimum of 2 vectors
__forceinline Vec VecMin(const Vec& a, const Vec& b)
{
	return _mm_min_ps(a, b);
}

// Returns the maximum of 2 vectors
__forceinline Vec VecMax(const Vec& a, const Vec& b)
{
	return _mm_max_ps(a, b);
}

// Clamps the input vector between a min and max vector
__forceinline Vec VecClamp(const Vec& v, const Vec& min, const Vec& max)
{
	return VecMin(VecMax(v, min), max);
}

// Returns the absolute value of the input vector
__forceinline Vec VecAbs(const Vec& v)
{
	return _mm_max_ps(v * vecMinusOne(), v);
	//return _mm_and_ps(v.simdValue, _mm_castsi128_ps(_mm_set_epi32(0, 0, 0, ~(1 << 31))));
}

// Linearly interpolates between a & b using interpolator t
__forceinline Vec VecLerp(const Vec& a, const Vec& b, const Vec& t)
{
	return VecAdd(VecSub(a, VecMul(a, t)), VecMul(b, t));
}

// Linearly interpolates between a & b using interpolator t
__forceinline Vec VecLerp(const Vec& a, const Vec& b, const float t)
{
	return VecLerp(a, b, vec(t));
}

// Returns a [0,1] interpolator from a given input and range
__forceinline Vec VecRamp(const Vec& val, const Vec& low, const Vec& high)
{
	Vec range = VecMax(high - low, vecEpsilon());
	return VecClamp((val - low) / range, vecZero(), vecOne());
}

// Returns the sign of the input vector
__forceinline Vec VecSign(const Vec& v)
{
	return _mm_or_ps(_mm_and_ps(vecZero() < v, vecOne()), _mm_and_ps(v < vecZero(), vecMinusOne()));
}

// Negates the input vector
__forceinline Vec VecNeg(const Vec& v)
{
	return vecZero() - v;
}

#pragma endregion Misc



#pragma region Vector Math

// Returns the reciprocal of the input vector
__forceinline Vec VecRecipEst(Vec const& r)
{
	return _mm_rcp_ps(r);
}

// Returns the reciprocal of the input vector
__forceinline Vec VecRecipEstX(Vec const& r)
{
	Vec v = _mm_rcp_ss(r);
	return VecShuffle<k_xxxx>(v, v);
}

// Returns the reciprocal of the input vector
__forceinline Vec VecRecipX(Vec const& r)
{
	Vec rcp = VecRecipEstX(r);
	Vec nr0 = VecAdd(VecMul(VecSub(vecOne(), VecMul(r, rcp)), rcp), rcp);
	Vec nr1 = VecAdd(VecMul(VecSub(vecOne(), VecMul(r, nr0)), nr0), nr0);
	return VecSelect(nr1, rcp, _mm_cmpeq_ps(nr0, nr0));
}

// Performs the dot product of 2 vectors
__forceinline Vec VecDot3(Vec const& a, Vec const& b)
{
	Vec mul = a * b;
	Vec dot = VecShuffle<k_xxxx>(mul) + VecShuffle<k_yyyy>(mul) + VecShuffle<k_zzzz>(mul);
	return dot;
}

// Performs the dot product of 2 vectors
__forceinline Vec VecDot4(Vec const& a, Vec const& b)
{
	Vec mul = a * b;
	Vec v0 = VecShuffle<k_yxwz>(mul) + mul;
	Vec dot = VecShuffle<k_zwxy>(v0) + v0;
	return dot;
}

// Performs the dot product of 2 vectors and returns a float
__forceinline float VecDot3ToFloat(Vec const& a, Vec const& b)
{
	return VecToFloat(VecDot3(a, b));
}

// Returns the length of the input vector
__forceinline Vec VecLength3(Vec& v)
{
	return _mm_sqrt_ps(VecDot3(v, v));
}

// Returns the length of the input vector as a float
__forceinline float VecLength3ToFloat(Vec& v)
{
	return VecToFloat(VecLength3(v));
}

// Normalizes the input vector
__forceinline Vec VecNormal3(Vec& v)
{
	Vec dp = VecDot3(v, v);
	Vec cond = dp == vecZero();
	//__m128 sel   = _mm_or_ps(_mm_andnot_ps(cmp1, dp), _mm_and_ps(vecOne.simdValue, cond));
	Vec sel = VecSelect(vecOne(), dp, cond);
	Vec rsqrt = _mm_rsqrt_ps(sel);
	Vec tmp1 = sel * rsqrt;
	//__m128 tmp2  = _mm_andnot_ps(cond, vecHalf.simdValue * tmp1 * (vecOne.simdValue - rsqrt * tmp1) + tmp1);
	//__m128 tmp3  = _mm_or_ps(tmp2, _mm_and_ps(vecZero.simdValue, cond));
	Vec tmp3 = VecSelect(vecZero(), vecHalf() * tmp1 * (vecOne() - rsqrt * tmp1) + tmp1, cond);
	return tmp3;
}

// Performs the cross product of 2 vectors
__forceinline Vec Cross(Vec const& a, Vec const& b)
{
	Vec tmp0 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1));
	Vec tmp1 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2));
	Vec tmp2 = tmp0 * b;
	Vec tmp3 = tmp0 * tmp1;
	Vec tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));
	return tmp3 - tmp4;
}

// Calculates the Taylor series cosine of the input vector
__forceinline Vec VecCos(Vec& in)
{
	Vec v1 = VecAbs(in) * vecNegOneOverPi();
	const Vec constVal = vec(1.2582912e7f);
	Vec v2 = (((v1 - constVal) + constVal) - v1) - vecHalf();

	Vec x = VecAbs(v2) - vecQuarter();
	Vec x2 = x * x;
	Vec accum = x * vecCosXCoef();
	x *= x2;
	accum += x * vecCosX3Coef();
	x *= x2;
	accum += x * vecCosX5Coef();
	x *= x2;
	accum += x * vecCosX7Coef();
	x *= x2;
	accum += x * vecCosX9Coef();
	x *= x2;
	accum += x * vecCosX11Coef();
	x *= x2;
	accum += x * vecCosX13Coef();
	x *= x2;
	accum += x * vecCosX15Coef();
	x *= x2;
	accum += x * vecCosX17Coef();
	x *= x2;
	accum += x * vecCosX19Coef();
	return _mm_max_ps(vecMinusOne(), _mm_min_ps(vecOne(), accum));
}

// Calculates the Taylor series sine of the input vector
__forceinline Vec VecSin(Vec& in)
{
	Vec v = in - vec(1.5707964f);
	return VecCos(v);
}

__forceinline Vec VecDistanceBetween(Vec const& a, Vec const& b)
{
	Vec diff = VecSub(a, b);
	Vec dot = VecDot3(diff, diff);
	return dot;
}

#pragma endregion Vector Math

}
