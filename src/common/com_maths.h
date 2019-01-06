#pragma once

#include "com_module.h"

#define pi32 3.14159265359f
#define DEG2RAD 3.141593f / 180.0f
#define RAD2DEG 57.2958f


#define ZABS(value) (value = value >= 0 ? value : -value )
#define ZMIN(x, y) ((x) < (y) ? (x) : (y))
#define ZMAX(x, y) ((x) > (y) ? (x) : (y))


//#define ArrayCount(array) (sizeof(array) / sizeof(array)[0]))

#define VEC_X 0
#define VEC_Y 1
#define VEC_Z 2
#define VEC_W 3

/* Matrix use
OpenGL uses column major, y/x matrices
/   0   4   8   12  \
|   1   5   9   13  |
|   2   6   10  14  |
\   3   7   11  15  /

*/

// Access Matrix Arrays
#define M3x3_X0 0
#define M3x3_X1 1
#define M3x3_X2 2

#define M3x3_Y0 3
#define M3x3_Y1 4
#define M3x3_Y2 5

#define M3x3_Z0 6
#define M3x3_Z1 7
#define M3x3_Z2 8



#define M4x4_X0 0
#define M4x4_X1 1
#define M4x4_X2 2
#define M4x4_X3 3

#define M4x4_Y0 4
#define M4x4_Y1 5
#define M4x4_Y2 6
#define M4x4_Y3 7

#define M4x4_Z0 8
#define M4x4_Z1 9
#define M4x4_Z2 10
#define M4x4_Z3 11

#define M4x4_W0 12
#define M4x4_W1 13
#define M4x4_W2 14
#define M4x4_W3 15


struct Vec3;
struct Vec4;
struct M3x3;
struct M4x4;

//internal i32 g_z_inf = 0x7F800000;
f32 ZINFINITY();

//internal i32 g_z_nan = 0x7F800001;
f32 ZNaN();

com_internal f32 COM_STDRandf32();
com_internal void COM_ClampF32(f32* val, f32 min, f32 max);
com_internal void COM_ClampI32(i32* val, i32 min, i32 max);
com_internal f32 COM_LerpF32(f32 start, f32 end, f32 lerp);

/////////////////////////////////////////////////////////////////////////////
// VECTOR 3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////
f32 Vec3_Magnitude(Vec3* v);
void Vec3_Normalise(Vec3* v);
void Vec3_SetMagnitude(Vec3* v, f32 newMagnitude);
void Vec3_CrossProduct(Vec3* a, Vec3* b, Vec3* result);
f32 Vec3_DotProduct(Vec3* a, Vec3* b);
void Vec3_NormaliseOrForward(Vec3* v);


/////////////////////////////////////////////////////////////////////////////
// VECTOR 3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////

f32 Vec4_Magnitude(Vec4* v);
void Vec4_Normalise(Vec4* v);
void Vec4_SetMagnitude(Vec4* v, f32 newMagnitude);
/////////////////////////////////////////////////////////////////////////////
// M3x3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////
void M3x3_SetToIdentity(f32* m);
void M3x3_SetX(f32* m, f32 x0, f32 x1, f32 x2);
void M3x3_SetY(f32* m, f32 y0, f32 y1, f32 y2);
void M3x3_SetZ(f32* m, f32 z0, f32 z1, f32 z2);
void M3x3_Multiply(f32* a, f32* b, f32* result);
void M3x3_Copy(f32* src, f32* tar);
void M3x3_RotateX(f32* m, f32 degreesX);
void M3x3_RotateY(f32* m, f32 degreesY);
void M3x3_RotateZ(f32* m, f32 degreesZ);
f32 M3x3_GetAngleX(f32* m);
f32 M3x3_GetAngleY(f32* m);
f32 M3x3_GetAngleZ(f32* m);
Vec3 M3x3_GetEulerAnglesRadians(f32* m);
Vec3 M3x3_GetEulerAnglesDegrees(f32* m);
void M3x3_SetEulerAnglesByRadians(f32* m, f32 roll, f32 pitch, f32 yaw);

/////////////////////////////////////////////////////////////////////////////
// M4x4 OPERATIONS
/////////////////////////////////////////////////////////////////////////////
void M4x4_SetToIdentity(f32* m);
void M4x4_SetX(f32* m, f32 x0, f32 x1, f32 x2, f32 x3);
void M4x4_SetY(f32* m, f32 y0, f32 y1, f32 y2, f32 y3);
void M4x4_SetZ(f32* m, f32 z0, f32 z1, f32 z2, f32 z3);
void M4x4_SetW(f32* m, f32 w0, f32 w1, f32 w2, f32 w3);
void M4x4_Multiply(f32* m0, f32* m1, f32* result);
void M4x4_Copy(f32* src, f32* tar);
void M4x4_RotateX(f32* m, f32 degreesY);
void M4x4_RotateY(f32* m, f32 degreesY);
void M4x4_RotateZ(f32* m, f32 degreesY);
f32 M4x4_GetAngleX(f32* m);
f32 M4x4_GetAngleY(f32* m);
f32 M4x4_GetAngleZ(f32* m);
void M4x4_SetPosition(f32* m, f32 x, f32 y, f32 z);
Vec4 M4x4_GetPosition(f32* m);
Vec3 M4x4_GetEulerAnglesRadians(f32* m);
Vec3 M4x4_GetEulerAnglesDegrees(f32* m);
void M4x4_SetEulerAnglesByRadians(f32* m, f32 roll, f32 pitch, f32 yaw);
void M4x4_SetProjection(f32* m, f32 prjNear, f32 prjFar, f32 prjLeft, f32 prjRight, f32 prjTop, f32 prjBottom);
void M4x4_SetOrthoProjection(f32* m, f32 left, f32 right, f32 top, f32 bottom, f32 prjNear, f32 prjFar);

void M4x4_Invert(f32* src);
void M4x4_ClearPosition(f32* src);
void M4x4_ClearRotation(f32* src);
