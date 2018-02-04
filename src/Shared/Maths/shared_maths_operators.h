#pragma once

#if 0
#include "../shared_types.h"

inline Vec3 CreateVec3(f32 x, f32 y)
{
    Vec3 result;
    result.x = x;
    result.y = y;
    return result;
}

inline Vec3 operator*(f32 a, Vec3 b)
{
    Vec3 result;
    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    return result;
}

inline Vec3 operator*(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

inline Vec3 operator-(Vec3 a)
{
    Vec3 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    return result;
}

inline Vec3 operator-(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

inline Vec3 operator+(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

/*
Assumes Vec3 is an affine Vec4 where w = 1.
*/
inline Vec3 operator*(Vec3 a, M4x4 m)
{
    Vec3 result;
    result.x = (a.x * m.x0) + (a.y * m.x1) + (a.z * m.x2) + (1 * m.x3);
    result.y = (a.x * m.y0) + (a.y * m.y1) + (a.z * m.y2) + (1 * m.y3);
    result.z = (a.x * m.z0) + (a.y * m.z1) + (a.z * m.z2) + (1 * m.z3);
    //result.w = (a.x * m.w0) + (a.y * m.w1) + (a.z * m.w2) + (1 * m.w3);
    return result;
}
#endif
