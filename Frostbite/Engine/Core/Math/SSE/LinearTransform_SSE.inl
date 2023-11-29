#pragma once
#include "../Quat.h"


namespace fb
{

__forceinline LinearTransform::LinearTransform(LinearTransform::IdentityType type)
{
    Vec one = _mm_set_ss(1.f);
    left = one;
    up = VecShuffle<k_yxyy>(one);
    forward = VecShuffle<k_yyxy>(one);
    trans = VecShuffle<k_yyyx>(one);
}

__forceinline LinearTransform::LinearTransform(RotationAxisX axis, float radians)
{
    float cos_ = cosf(radians);
    float sin_ = sinf(radians);
    left    = vec(1.f,  0.f,  0.f);
    up      = vec(0.f,  cos_, sin_);
    forward = vec(0.f, -sin_, cos_);
    trans   = vec(0.f,  0.f,  0.f);
}

__forceinline LinearTransform::LinearTransform(RotationAxisY axis, float radians)
{
    float cos_ = cosf(radians);
    float sin_ = sinf(radians);
    left    = vec(cos_, 0.f, -sin_);
    up      = vec(0.f,  1.f,  0.f);
    forward = vec(sin_, 0.f,  cos_);
    trans   = vec(0.f,  0.f,  0.f);
}

__forceinline LinearTransform::LinearTransform(RotationAxisZ axis, float radians)
{
    float cos_ = cosf(radians);
    float sin_ = sinf(radians);
    left    = vec(cos_,  sin_, 0.f);
    up      = vec(-sin_, cos_, 0.f);
    forward = vec(0.f,   0.f,  1.f);
    trans   = vec(0.f,   0.f,  0.f);
}

__forceinline LinearTransform::LinearTransform(const LinearTransform& t)
{
    *this = t;
}

__forceinline LinearTransform::LinearTransform(Vec3Ref inLeft, Vec3Ref inUp, Vec3Ref inForward, Vec3Ref inTrans)
    : left(inLeft)
    , up(inUp)
    , forward(inForward)
    , trans(inTrans)
{
}

__forceinline LinearTransform::LinearTransform(const Quat& r, Vec3Ref t)
{
    Vec quat = r.vec;
    Vec xyz_2 = VecMul(VecRecipX(VecDot4(quat, quat)), VecAdd(quat, quat));
    Vec quatYZX = VecShuffle<k_yzxy>(quat);
    Vec quatW = VecShuffle<k_wwww>(quat);
    Vec quatZXY = VecShuffle<k_zxyz>(quat);
    Vec yzx_2 = VecShuffle<k_yzxy>(xyz_2);
    Vec zxy_2 = VecShuffle<k_zxyz>(xyz_2);
    Vec v0 = VecMul(yzx_2, quatYZX);
    Vec v1 = VecMul(yzx_2, quatW);
    Vec v2 = VecMul(quatYZX, xyz_2);
    v0 = VecSub(VecSub(vecOne(), v0), VecMul(quatZXY, zxy_2));
    v1 = VecAdd(v1, VecMul(quatZXY, xyz_2));
    v2 = VecSub(v2, VecMul(zxy_2, quatW));
    left = VecShuffle<k_xyzw>(_mm_move_ss(v1, v0), v2);
    up = VecShuffle<k_xyzw>(_mm_move_ss(v0, v2), v1);
    forward = VecShuffle<k_xyzw>(_mm_move_ss(v2, v1), v0);
    trans = t;
}

__forceinline LinearTransform LinearTransform::operator*(const LinearTransform& T) const
{
    Vec aRow0 = VecShuffle<k_xxxx>(left);
    Vec aRow1 = VecShuffle<k_xxxx>(up);
    Vec aRow2 = VecShuffle<k_xxxx>(forward);
    Vec aRow3 = VecShuffle<k_xxxx>(trans);

    Vec outRow0 = VecMul(aRow0, T.left);
    Vec outRow1 = VecMul(aRow1, T.left);
    Vec outRow2 = VecMul(aRow2, T.left);
    Vec outRow3 = VecAdd(VecMul(aRow3, T.left), T.trans);

    aRow0 = VecShuffle<k_yyyy>(left);
    aRow1 = VecShuffle<k_yyyy>(up);
    aRow2 = VecShuffle<k_yyyy>(forward);
    aRow3 = VecShuffle<k_yyyy>(trans);

    outRow0 = VecAdd(VecMul(aRow0, T.up), outRow0);
    outRow1 = VecAdd(VecMul(aRow1, T.up), outRow1);
    outRow2 = VecAdd(VecMul(aRow2, T.up), outRow2);
    outRow3 = VecAdd(VecMul(aRow3, T.up), outRow3);

    aRow0 = VecShuffle<k_zzzz>(left);
    aRow1 = VecShuffle<k_zzzz>(up);
    aRow2 = VecShuffle<k_zzzz>(forward);
    aRow3 = VecShuffle<k_zzzz>(trans);

    outRow0 = VecAdd(VecMul(aRow0, T.forward), outRow0);
    outRow1 = VecAdd(VecMul(aRow1, T.forward), outRow1);
    outRow2 = VecAdd(VecMul(aRow2, T.forward), outRow2);
    outRow3 = VecAdd(VecMul(aRow3, T.forward), outRow3);

    return LinearTransform(outRow0, outRow1, outRow2, outRow3);
}

__forceinline Vec3 invRotate(Vec3Ref u, const LinearTransform& T)
{

    Vec s = VecRecipX(VecDot3(T.left, T.left));
    Vec uByLeft = VecMul(u, T.left);
    Vec uByUp = VecMul(u, T.up);
    Vec uByFwd = VecMul(u, T.forward);
    Vec xyxy = VecShuffle<k_xyxy>(uByLeft, uByUp);
    Vec zwzw = VecShuffle<k_zwzw>(uByLeft, uByUp);
    Vec rot = VecShuffle<k_xzxx>(xyxy, uByFwd);
    rot = VecAdd(rot, VecShuffle<k_ywyy>(xyxy, uByFwd));
    rot = VecAdd(rot, VecShuffle<k_xzzz>(zwzw, uByFwd));
    rot = VecMul(rot, s);

    return rot;
}

__forceinline bool LinearTransform::operator==(const LinearTransform& T) const
{
    Vec maskLeft  = _mm_cmpeq_ps(left, T.left);
    Vec maskUp    = _mm_cmpeq_ps(up, T.up);
    Vec maskFwd   = _mm_cmpeq_ps(forward, T.forward);
    Vec maskTrans = _mm_cmpeq_ps(trans, T.trans);
    Vec compMask  = VecAnd(VecAnd(maskLeft, maskUp), VecAnd(maskFwd, maskTrans));
    // compare xyz
    return (_mm_movemask_ps(compMask) & 0x7) == 0x7;
}

__forceinline bool LinearTransform::operator!=(const LinearTransform& T) const
{
    return !(*this == T);
}

__forceinline LinearTransform lerp(const LinearTransform& T0, const LinearTransform& T1, float t)
{
	auto native = reinterpret_cast<LinearTransform* (__fastcall*)(LinearTransform* retval, const LinearTransform&,const LinearTransform&, float)>(0x140139110);
	LinearTransform TOut;
	native(&TOut, T0, T1, t);
	return TOut;
}

}