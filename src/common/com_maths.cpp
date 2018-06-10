#pragma once

#include "com_module.h"
#include <stdlib.h>
#include <math.h>

internal i32 g_z_inf = 0x7F800000;
inline f32 ZINFINITY() { return *(float*)&g_z_inf; }

internal i32 g_z_nan = 0x7F800001;
inline f32 ZNaN() { return *(float*)&g_z_nan; }



static inline f32 COM_CapAngleDegrees(f32 angle)
{
    u32 loopCount = 0;
    while (angle > 360)
    {
        angle -= 360;
        loopCount++;
        Assert(loopCount < 99999);
    }
    loopCount = 0;
    while (angle < 0)
    {
        angle += 360;
        loopCount++;
        Assert(loopCount < 99999);
    }
    return angle;
}

static inline f32 COM_Randf32()
{
    return (f32)rand() / (f32)RAND_MAX;
}

/////////////////////////////////////////////////////////////////////////////
// VECTOR 3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////

inline f32 Vec3_Magnitude(Vec3* v)
{
    return (f32)sqrt((f32)(v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

inline f32 Vec3_Magnitudef(f32 x, f32 y, f32 z)
{
	return (f32)sqrt((f32)(x * x) + (y * y) + (z * z));
}

inline void Vec3_Normalise(Vec3* v)
{
    f32 vectorMagnitude = Vec3_Magnitude(v);
    if (vectorMagnitude == 0) { return; }
    v->x /= vectorMagnitude;
    v->y /= vectorMagnitude;
    v->z /= vectorMagnitude;
}

inline void Vec3_SetMagnitude(Vec3* v, f32 newMagnitude)
{
    Vec3_Normalise(v);
    v->x = v->x * newMagnitude;
    v->y = v->y * newMagnitude;
    v->z = v->z * newMagnitude;
}

inline void Vec3_CrossProduct(Vec3* a, Vec3* b, Vec3* result)
{
    result->x = (a->y * b->z) - (a->z * b->y);
    result->z = (a->x * b->y) - (a->y * b->x);
    result->y = (a->z * b->x) - (a->x * b->z);
}

inline f32 Vec3_DotProduct(Vec3* a, Vec3* b)
{
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

inline void Vec3_NormaliseOrForward(Vec3* v)
{
    f32 mag = Vec3_Magnitude(v);
    if (mag == 0)
    {
        v->x = 0;
        v->y = 0;
        v->z = 1;
        return;
    }
    v->x = v->x /= mag;
    v->y = v->y /= mag;
    v->z = v->z /= mag;
}


/////////////////////////////////////////////////////////////////////////////
// VECTOR 3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////

inline f32 Vec4_Magnitude(Vec4* v)
{
    return (f32)sqrt((f32)(v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

inline void Vec4_Normalise(Vec4* v)
{
    f32 vectorMagnitude = Vec4_Magnitude(v);
    v->x /= vectorMagnitude;
    v->y /= vectorMagnitude;
    v->z /= vectorMagnitude;
}

inline void Vec4_SetMagnitude(Vec4* v, f32 newMagnitude)
{
    Vec4_Normalise(v);
    v->x = v->x * newMagnitude;
    v->y = v->y * newMagnitude;
    v->z = v->z * newMagnitude;
}

/////////////////////////////////////////////////////////////////////////////
// M3x3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////
inline void M3x3_SetToIdentity(f32* m)
{
    m[M3x3_X0] = 1;
    m[M3x3_X1] = 0;
    m[M3x3_X2] = 0;

    m[M3x3_Y0] = 0;
    m[M3x3_Y1] = 1;
    m[M3x3_Y2] = 0;

    m[M3x3_Z0] = 0;
    m[M3x3_Z1] = 0;
    m[M3x3_Z2] = 1;
}

inline void M3x3_SetX(f32* m, f32 x0, f32 x1, f32 x2)
{
    m[M3x3_X0] = x0; m[M3x3_X1] = x1; m[M3x3_X2] = x2;
}

inline void M3x3_SetY(f32* m, f32 y0, f32 y1, f32 y2)
{
    m[M3x3_Y0] = y0; m[M3x3_Y1] = y1; m[M3x3_Y2] = y2;
}

inline void M3x3_SetZ(f32* m, f32 z0, f32 z1, f32 z2)
{
    m[M3x3_Z0] = z0; m[M3x3_Z1] = z1; m[M3x3_Z2] = z2;
}

inline void M3x3_Multiply(f32* a, f32* b, f32* result)
{
    /*
                0   3   6
                1   4   7 
                2   5   8
                |   |   | 
    0   3   6 -
    1   4   7 -
    2   5   8-

    result 3 = (0 x 3) + (3 x 4) + (6 x 5)
    result 4 = (1 x 3) + (4 x 4) + (7 x 5)
    result 5 = (2 x 3) + (5 x 4) + (8 x 5)

    result 6 = (0 x 6) + (3 x 7) + (6 x 8)
    result 7 = (1 x 6) + (4 x 7) + (7 x 8)
    result 8 = (2 x 6) + (5 x 7) + (8 x 8)

    */
    // 0 = 
    // Copy original
    f32 r[9];
    for (i32 i = 0; i < 9; ++i)
    {
        r[i] = a[i];
    }
    r[0] = (a[0] * b[0]) + (a[3] * b[1]) + (a[6] * b[2]);
    r[1] = (a[1] * b[0]) + (a[4] * b[1]) + (a[7] * b[2]);
    r[2] = (a[2] * b[0]) + (a[5] * b[1]) + (a[8] * b[2]);

    r[3] = (a[0] * b[3]) + (a[3] * b[4]) + (a[6] * b[5]);
    r[4] = (a[1] * b[3]) + (a[4] * b[4]) + (a[7] * b[5]);
    r[5] = (a[2] * b[3]) + (a[5] * b[4]) + (a[8] * b[5]);

    r[6] = (a[0] * b[6]) + (a[3] * b[7]) + (a[6] * b[8]);
    r[7] = (a[1] * b[6]) + (a[4] * b[7]) + (a[7] * b[8]);
    r[8] = (a[2] * b[6]) + (a[5] * b[7]) + (a[8] * b[8]);
    
    for (i32 i = 0; i < 9; ++i)
    {
        result[i] = r[i];
    }
}

inline void M3x3_Copy(f32* src, f32* tar)
{
    tar[M3x3_X0] = src[M3x3_X0];  tar[M3x3_X1] = src[M3x3_X1];  tar[M3x3_X2] = src[M3x3_X2];
    tar[M3x3_Y0] = src[M3x3_Y0];  tar[M3x3_Y1] = src[M3x3_Y1];  tar[M3x3_Y2] = src[M3x3_Y2];
    tar[M3x3_Z0] = src[M3x3_Z0];  tar[M3x3_Z1] = src[M3x3_Z1];  tar[M3x3_Z2] = src[M3x3_Z2];
}

inline void M3x3_RotateX(f32* m, f32 radiansX)
{
	M3x3 rotM = {};
    rotM.xAxisX = 1;
    rotM.yAxisY = (f32)cos(radiansX);
    rotM.yAxisZ = (f32)sin(radiansX);
    rotM.zAxisY = (f32)-sin(radiansX);
    rotM.zAxisZ = (f32)cos(radiansX);
    M3x3_Multiply(m, rotM.cells, m);
}

inline void M3x3_RotateY(f32* m, f32 radiansY)
{
	M3x3 rotM = {};
    rotM.yAxisY = 1;
    rotM.xAxisX = (f32)cos(radiansY);
    rotM.xAxisZ = (f32)-sin(radiansY);
    rotM.zAxisX = (f32)sin(radiansY);
    rotM.zAxisZ = (f32)cos(radiansY);
    M3x3_Multiply(m, rotM.cells, m);
}

inline void M3x3_RotateZ(f32* m, f32 radiansZ)
{
	M3x3 rotM = {};
    rotM.zAxisZ = 1;
    rotM.xAxisX = (f32)cos(radiansZ);
    rotM.xAxisY = (f32)sin(radiansZ);
    rotM.yAxisX = (f32)-sin(radiansZ);
    rotM.yAxisY = (f32)cos(radiansZ);
    M3x3_Multiply(m, rotM.cells, m);
}

inline f32 M3x3_GetAngleX(f32* m)
{
    //return (f32)atan2(m[8], m[10]);
    return (f32)-asinf(m[9]);
}

inline f32 M3x3_GetAngleY(f32* m)
{
    return (f32)atan2(m[8], m[10]);
    //return (f32)-asinf(m[9]);
}

inline f32 M3x3_GetAngleZ(f32* m)
{
    return (f32)atan2(m[1], m[5]);
}

inline Vec3 M3x3_GetEulerAnglesRadians(f32* m)
{
    Vec3 result;
    /*result.x = (f32)-asinf(m[9]);
    result.y = (f32)atan2(m[8], m[10]);
    result.z = (f32)atan2(m[1], m[5]);*/
	result.x = (f32)-asinf(m[M3x3_Z1]);
	result.y = (f32)atan2(m[M3x3_Z0], m[M3x3_Z2]);
	result.z = (f32)atan2(m[M3x3_X1], m[M3x3_Y1]);
    //result.w = 1;
    return result;
}

inline Vec3 M3x3_GetEulerAnglesDegrees(f32* m)
{
    Vec3 result;
    /*result.x = (f32)-asinf(m[9]) * RAD2DEG;
    result.y = ((f32)atan2(m[8], m[10])) * RAD2DEG;
    result.z = ((f32)atan2(m[1], m[5])) * RAD2DEG;*/

    if (m[M3x3_Z1]> 1.0)
    {
        m[M3x3_Z1] = 1.0f;
    }
    if (m[M3x3_Z1] < -1.0)
    {
        m[M3x3_Z1] = -1.0f;
    }
	result.x = (f32)-asinf(m[M3x3_Z1]) * RAD2DEG;
	result.y = ((f32)atan2(m[M3x3_Z0], m[M3x3_Z2])) * RAD2DEG;
	result.z = ((f32)atan2(m[M3x3_X1], m[M3x3_Y1])) * RAD2DEG;

    // result.y = (f32)-asinf(m[9]) * RAD2DEG;
    // result.x = ((f32)atan2(m[8], m[10])) * RAD2DEG;
    // result.z = ((f32)atan2(m[1], m[5])) * RAD2DEG;
    //result.w = 1;
    return result;
}

inline void M3x3_SetEulerAnglesByRadians(f32* m, f32 roll, f32 pitch, f32 yaw)
{
    // TODO: Implement
}

inline void M3x3_CopyFromM4x4(f32* m3x3, f32* m4x4)
{
    m3x3[M3x3_X0] = m4x4[M4x4_X0];
    m3x3[M3x3_X1] = m4x4[M4x4_X1];
    m3x3[M3x3_X2] = m4x4[M4x4_X2];

    m3x3[M3x3_Y0] = m4x4[M4x4_Y0];
    m3x3[M3x3_Y1] = m4x4[M4x4_Y1];
    m3x3[M3x3_Y2] = m4x4[M4x4_Y2];

    m3x3[M3x3_Z0] = m4x4[M4x4_Z0];
    m3x3[M3x3_Z1] = m4x4[M4x4_Z1];
    m3x3[M3x3_Z2] = m4x4[M4x4_Z2];
}

inline void M3x3_ClearTinyValues(f32* m, f32 cap)
{
    if (ZABS(m[M3x3_X0]) < cap) { m[M3x3_X0] = 0; }
    if (ZABS(m[M3x3_X1]) < cap) { m[M3x3_X1] = 0; }
    if (ZABS(m[M3x3_X2]) < cap) { m[M3x3_X2] = 0; }

    if (ZABS(m[M3x3_Y0]) < cap) { m[M3x3_Y0] = 0; }
    if (ZABS(m[M3x3_Y1]) < cap) { m[M3x3_Y1] = 0; }
    if (ZABS(m[M3x3_Y2]) < cap) { m[M3x3_Y2] = 0; }

    if (ZABS(m[M3x3_Z0]) < cap) { m[M3x3_Z0] = 0; }
    if (ZABS(m[M3x3_Z1]) < cap) { m[M3x3_Z1] = 0; }
    if (ZABS(m[M3x3_Z2]) < cap) { m[M3x3_Z2] = 0; }
}

/////////////////////////////////////////////////////////////////////////////
// M4x4 OPERATIONS
/////////////////////////////////////////////////////////////////////////////
inline void M4x4_SetToIdentity(f32* m)
{
    *m = {};
    m[M4x4_X0] = 1;
    m[M4x4_X1] = 0;
    m[M4x4_X2] = 0;
    m[M4x4_X3] = 0;

    m[M4x4_Y0] = 0;
    m[M4x4_Y1] = 1;
    m[M4x4_Y2] = 0;
    m[M4x4_Y3] = 0;

    m[M4x4_Z0] = 0;
    m[M4x4_Z1] = 0;
    m[M4x4_Z2] = 1;
    m[M4x4_Z3] = 0;

    m[M4x4_W0] = 0;
    m[M4x4_W1] = 0;
    m[M4x4_W2] = 0;
    m[M4x4_W3] = 1;
}

inline void M4x4_Transpose(f32* m)
{
    /*
                    0   4   8   12
                    1   5   9   13
                    2   6   10  14
                    3   7   11  15
                    |   |   |   |
    0   1   2   3-
    4   5   6   7-
    8   9   10  11-
    12  13  14  15-
    */
    // Create a clone to copy values from
    f32 c[16];
    M4x4_Copy(m, c);
    m[1] = c[4];
    m[4] = c[1];
    m[2] = c[8];
    m[8] = c[2];
    m[3] = c[12];
    m[12] = c[3];
    
    m[6] = c[9];
    m[9] = c[6];
    m[7] = c[13];
    m[13] = c[7];
    m[11] = c[14];
    m[14] = c[11];
}

inline void M4x4_SetX(f32* m, f32 x0, f32 x1, f32 x2, f32 x3)
{
    m[M4x4_X0] = x0; m[M4x4_X1] = x1; m[M4x4_X2] = x2; m[M4x4_X3] = x3;
}

inline void M4x4_SetY(f32* m, f32 y0, f32 y1, f32 y2, f32 y3)
{
    m[M4x4_Y0] = y0; m[M4x4_Y1] = y1; m[M4x4_Y2] = y2; m[M4x4_Y3] = y3;
}

inline void M4x4_SetZ(f32* m, f32 z0, f32 z1, f32 z2, f32 z3)
{
    m[M4x4_Z0] = z0; m[M4x4_Z1] = z1; m[M4x4_Z2] = z2; m[M4x4_Z3] = z3;
}

inline void M4x4_SetW(f32* m, f32 w0, f32 w1, f32 w2, f32 w3)
{
    m[M4x4_W0] = w0; m[M4x4_W1] = w1; m[M4x4_W2] = w2; m[M4x4_W3] = w3;
}

inline void M4x4_Multiply(f32* m0, f32* m1, f32* result)
{
    /*
                    0   4   8   12
                    1   5   9   13
                    2   6   10  14
                    3   7   11  15
                    |   |   |   |
    0   4   8   12-
    1   5   9   13-
    2   6   10  14-
    3   7   11  15-
    */
    // 0 = 
    // Copy original
    f32 r[16];
    for (i32 i = 0; i < 16; ++i)
    {
        r[i] = m0[i];
    }
    r[0] = (m0[0] * m1[0]) + (m0[4] * m1[1]) + (m0[8] * m1[2]) + (m0[12] * m1[3]);
    r[1] = (m0[1] * m1[0]) + (m0[5] * m1[1]) + (m0[9] * m1[2]) + (m0[13] * m1[3]);
    r[2] = (m0[2] * m1[0]) + (m0[6] * m1[1]) + (m0[10] * m1[2]) + (m0[14] * m1[3]);
    r[3] = (m0[3] * m1[0]) + (m0[7] * m1[1]) + (m0[11] * m1[2]) + (m0[15] * m1[3]);

    r[4] = (m0[0] * m1[4]) + (m0[4] * m1[5]) + (m0[8] * m1[6]) + (m0[12] * m1[7]);
    r[5] = (m0[1] * m1[4]) + (m0[5] * m1[5]) + (m0[9] * m1[6]) + (m0[13] * m1[7]);
    r[6] = (m0[2] * m1[4]) + (m0[6] * m1[5]) + (m0[10] * m1[6]) + (m0[14] * m1[7]);
    r[7] = (m0[3] * m1[4]) + (m0[7] * m1[5]) + (m0[11] * m1[6]) + (m0[15] * m1[7]);
    
    r[8] = (m0[0] * m1[8]) + (m0[4] * m1[9]) + (m0[8] * m1[10]) + (m0[12] * m1[11]);
    r[9] = (m0[1] * m1[8]) + (m0[5] * m1[9]) + (m0[9] * m1[10]) + (m0[13] * m1[11]);
    r[10] = (m0[2] * m1[8]) + (m0[6] * m1[9]) + (m0[10] * m1[10]) + (m0[14] * m1[11]);
    r[11] = (m0[3] * m1[8]) + (m0[7] * m1[9]) + (m0[11] * m1[10]) + (m0[15] * m1[11]);
    
    r[12] = (m0[0] * m1[12]) + (m0[4] * m1[13]) + (m0[8] * m1[14]) + (m0[12] * m1[15]);
    r[13] = (m0[1] * m1[12]) + (m0[5] * m1[13]) + (m0[9] * m1[14]) + (m0[13] * m1[15]);
    r[14] = (m0[2] * m1[12]) + (m0[6] * m1[13]) + (m0[10] * m1[14]) + (m0[14] * m1[15]);
    r[15] = (m0[3] * m1[12]) + (m0[7] * m1[13]) + (m0[11] * m1[14]) + (m0[15] * m1[15]);

    for (i32 i = 0; i < 16; ++i)
    {
        result[i] = r[i];
    }
}

// Flip rotation and position. Keep others intact
inline void M4x4_Invert(f32* src)
{
    src[M4x4_X0] = -src[M4x4_X0];
    src[M4x4_X1] = -src[M4x4_X1];
    src[M4x4_X2] = -src[M4x4_X2];

    src[M4x4_Y0] = -src[M4x4_Y0];
    src[M4x4_Y1] = -src[M4x4_Y1];
    src[M4x4_Y2] = -src[M4x4_Y2];

    src[M4x4_Z0] = -src[M4x4_Z0];
    src[M4x4_Z1] = -src[M4x4_Z1];
    src[M4x4_Z2] = -src[M4x4_Z2];

    src[M4x4_W0] = -src[M4x4_W0];
    src[M4x4_W1] = -src[M4x4_W1];
    src[M4x4_W2] = -src[M4x4_W2];
}

inline void M4x4_Copy(f32* src, f32* tar)
{
    tar[M4x4_X0] = src[M4x4_X0];  tar[M4x4_X1] = src[M4x4_X1];  tar[M4x4_X2] = src[M4x4_X2];  tar[M4x4_X3] = src[M4x4_X3];
    tar[M4x4_Y0] = src[M4x4_Y0];  tar[M4x4_Y1] = src[M4x4_Y1];  tar[M4x4_Y2] = src[M4x4_Y2];  tar[M4x4_Y3] = src[M4x4_Y3];
    tar[M4x4_Z0] = src[M4x4_Z0];  tar[M4x4_Z1] = src[M4x4_Z1];  tar[M4x4_Z2] = src[M4x4_Z2];  tar[M4x4_Z3] = src[M4x4_Z3];
    tar[M4x4_W0] = src[M4x4_W0];  tar[M4x4_W1] = src[M4x4_W1];  tar[M4x4_W2] = src[M4x4_W2];  tar[M4x4_W3] = src[M4x4_W3];
}

// Clear axis columns
inline void M4x4_ClearRotation(f32* src)
{
    src[M4x4_X0] = 1;
    src[M4x4_X1] = 0;
    src[M4x4_X2] = 0;
    
    src[M4x4_Y0] = 0;
    src[M4x4_Y1] = 1;
    src[M4x4_Y2] = 0;
    
    src[M4x4_Z0] = 0;
    src[M4x4_Z1] = 0;
    src[M4x4_Z2] = 1;
}

inline void M4x4_ClearPosition(f32* src)
{
    src[M4x4_W0] = 0;
    src[M4x4_W1] = 0;
    src[M4x4_W2] = 0;
    src[M4x4_W3] = 1;
}

// inline f32 M4x4_GetScaleX(f32* m)
// {
//     f32 x = fabsf(m[X0] * m[X0]);
//     f32 y = fabsf(m[Y0] * m[Y0]);
//     f32 z = fabsf(m[Z0] * m[Z0]);
//     return (f32)sqrt(x + y + z);
// }

// inline f32 M4x4_GetScaleY(f32* m)
// {
//     f32 x = fabsf(m[X1] * m[X1]);
//     f32 y = fabsf(m[Y1] * m[Y1]);
//     f32 z = fabsf(m[Z1] * m[Z1]);
//     return (f32)sqrt(x + y + z);
// }

// inline f32 M4x4_GetScaleZ(f32* m)
// {
//     f32 x = fabsf(m[X2] * m[X2]);
//     f32 y = fabsf(m[Y2] * m[Y2]);
//     f32 z = fabsf(m[Z2] * m[Z2]);
//     return (f32)sqrt(x + y + z);
// }

// inline Vec4 M4x4_GetScale(f32* m)
// {
//     Vec4 result;
//     result.x = fabsf((f32)sqrt(m[X0] * m[X0] + m[Y0] * m[Y0] + m[Z0] * m[Z0]));
//     result.y = fabsf((f32)sqrt(m[X1] * m[X1] + m[Y1] * m[Y1] + m[Z1] * m[Z1]));
//     result.z = fabsf((f32)sqrt(m[X2] * m[X2] + m[Y2] * m[Y2] + m[Z2] * m[Z2]));
//     result.w = 1;
//     return result;
// }

// inline void M4x4_SetScale(f32* m, f32 x, f32 y, f32 z)
// {
//     M4x4* mat = (M4x4*)m;
//     Vec4_SetMagnitude(&mat->xAxis, x);
//     Vec4_SetMagnitude(&mat->yAxis, y);
//     Vec4_SetMagnitude(&mat->zAxis, z);
// }

inline void M4x4_RotateX(f32* m, f32 radiansX)
{
	M4x4 rotM = {};
    rotM.xAxisX = 1;
    rotM.yAxisY = (f32)cos(radiansX);
    rotM.yAxisZ = (f32)sin(radiansX);
    rotM.zAxisY = (f32)-sin(radiansX);
    rotM.zAxisZ = (f32)cos(radiansX);
    rotM.posW = 1;
    M4x4_Multiply(m, rotM.cells, m);
}

inline void M4x4_RotateY(f32* m, f32 radiansY)
{
	M4x4 rotM = {};
    rotM.yAxisY = 1;
    rotM.xAxisX = (f32)cos(radiansY);
    rotM.xAxisZ = (f32)-sin(radiansY);
    rotM.zAxisX = (f32)sin(radiansY);
    rotM.zAxisZ = (f32)cos(radiansY);
    rotM.posW = 1;
    M4x4_Multiply(m, rotM.cells, m);
}

inline void M4x4_RotateZ(f32* m, f32 radiansZ)
{
	M4x4 rotM = {};
    rotM.zAxisZ = 1;
    rotM.xAxisX = (f32)cos(radiansZ);
    rotM.xAxisY = (f32)sin(radiansZ);
    rotM.yAxisX = (f32)-sin(radiansZ);
    rotM.yAxisY = (f32)cos(radiansZ);
    rotM.posW = 1;
    M4x4_Multiply(m, rotM.cells, m);
}

inline f32 M4x4_GetAngleX(f32* m)
{
    //return (f32)atan2(m[8], m[10]);
    return (f32)-asinf(m[9]);
}

inline f32 M4x4_GetAngleY(f32* m)
{
    return (f32)atan2(m[8], m[10]);
    //return (f32)-asinf(m[9]);
}

inline f32 M4x4_GetAngleZ(f32* m)
{
    return (f32)atan2(m[1], m[5]);
}

inline void M4x4_SetPosition(f32* m, f32 x, f32 y, f32 z)
{
    m[M4x4_W0] = x, m[M4x4_W1] = y; m[M4x4_W2] = z;
}

inline Vec3 M4x4_GetPositionV3(f32* m)
{
    Vec3 result;
    result.x = m[M4x4_W0];
    result.y = m[M4x4_W1];
    result.z = m[M4x4_W2];
    return result;
}

inline Vec4 M4x4_GetPositionV4(f32* m)
{
    Vec4 result;
    result.x = m[M4x4_W0];
    result.y = m[M4x4_W1];
    result.z = m[M4x4_W2];
    result.w = 1;
    return result;
}

inline Vec3 M4x4_GetEulerAnglesRadians(f32* m)
{
    Vec3 result;
    result.x = (f32)-asinf(m[M4x4_Z1]);
    result.y = (f32)atan2(m[M4x4_Z0], m[M4x4_Z2]);
    result.z = (f32)atan2(m[M4x4_X1], m[M4x4_Y1]);
    return result;
}

inline Vec3 M4x4_GetEulerAnglesDegrees(f32* m)
{
    Vec3 result;
	/*result.x = (f32)-asinf(m[9]) * RAD2DEG;
	result.y = ((f32)atan2(m[8], m[10])) * RAD2DEG;
	result.z = ((f32)atan2(m[1], m[5])) * RAD2DEG;*/
    result.x = (f32)-asinf(m[M4x4_Z1]) * RAD2DEG;
    result.y = ((f32)atan2(m[M4x4_Z0], m[M4x4_Z2])) * RAD2DEG;
    result.z = ((f32)atan2(m[M4x4_X1], m[M4x4_Y1])) * RAD2DEG;
    return result;
}

// inline void M4x4_SetEulerAnglesByRadians(f32* m, f32 roll, f32 pitch, f32 yaw)
// {
//     // TODO: Implement... or not. euler angles suck :/
// }


inline void M4x4_SetProjection(f32* m, f32 prjNear, f32 prjFar, f32 prjLeft, f32 prjRight, f32 prjTop, f32 prjBottom)
{
    m[0] = (2 * prjNear) / (prjRight - prjLeft);
	m[4] = 0;
	m[8] = (prjRight + prjLeft) / (prjLeft - prjRight);
	m[12] = 0;
	
	m[1] = 0;
	m[5] = (2 * prjNear) / (prjTop - prjBottom);
	m[9] = (prjTop + prjBottom) / (prjTop - prjBottom);
	m[13] = 0;
	
	m[2] = 0;
	m[6] = 0;
	m[10] = -(prjFar + prjNear) / (prjFar - prjNear);
	m[14] = (-2 * prjFar * prjNear) / (prjFar - prjNear);
	
	m[3] = 0;
	m[7] = 0;
	m[11] = -1;
	m[15] = 0;
}

void M4x4_SetOrthoProjection(f32* m, f32 left, f32 right, f32 top, f32 bottom, f32 prjNear, f32 prjFar)
{
    M4x4_SetToIdentity(m);
    m[0] = 2 / (right - left);
    m[5] = 2 / (top - bottom);
    m[10] = -2 / (prjFar - prjNear);

    m[12] = (right + left) / (right - left);
    m[13] = (top + bottom) / (top - bottom);
    m[14] = (prjFar + prjNear) / (prjFar - prjNear);
}
