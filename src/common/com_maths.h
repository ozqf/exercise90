#pragma once

#include "com_module.h"

struct Vec3;
struct Vec4;
struct M3x3;
struct M4x4;

//internal i32 g_z_inf = 0x7F800000;
inline f32 ZINFINITY();

//internal i32 g_z_nan = 0x7F800001;
inline f32 ZNaN();

/////////////////////////////////////////////////////////////////////////////
// VECTOR 3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////
inline f32 Vec3_Magnitude(Vec3* v);
inline void Vec3_Normalise(Vec3* v);
inline void Vec3_SetMagnitude(Vec3* v, f32 newMagnitude);
inline void Vec3_CrossProduct(Vec3* a, Vec3* b, Vec3* result);
inline f32 Vec3_DotProduct(Vec3* a, Vec3* b);
inline void Vec3_NormaliseOrForward(Vec3* v);


/////////////////////////////////////////////////////////////////////////////
// VECTOR 3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////

inline f32 Vec4_Magnitude(Vec4* v);
inline void Vec4_Normalise(Vec4* v);
inline void Vec4_SetMagnitude(Vec4* v, f32 newMagnitude);
/////////////////////////////////////////////////////////////////////////////
// M3x3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////
inline void M3x3_SetToIdentity(f32* m);
inline void M3x3_SetX(f32* m, f32 x0, f32 x1, f32 x2);
inline void M3x3_SetY(f32* m, f32 y0, f32 y1, f32 y2);
inline void M3x3_SetZ(f32* m, f32 z0, f32 z1, f32 z2);
inline void M3x3_Multiply(f32* a, f32* b, f32* result);
inline void M3x3_Copy(f32* src, f32* tar);
inline void M3x3_RotateX(f32* m, f32 degreesY);
inline void M3x3_RotateY(f32* m, f32 degreesY);
inline void M3x3_RotateZ(f32* m, f32 degreesY);
inline f32 M3x3_GetAngleX(f32* m);
inline f32 M3x3_GetAngleY(f32* m);
inline f32 M3x3_GetAngleZ(f32* m);
inline Vec4 M3x3_GetEulerAnglesRadians(f32* m);
inline Vec4 M3x3_GetEulerAnglesDegrees(f32* m);
inline void M3x3_SetEulerAnglesByRadians(f32* m, f32 roll, f32 pitch, f32 yaw);

/////////////////////////////////////////////////////////////////////////////
// M4x4 OPERATIONS
/////////////////////////////////////////////////////////////////////////////
inline void M4x4_SetToIdentity(f32* m);
inline void M4x4_SetX(f32* m, f32 x0, f32 x1, f32 x2, f32 x3);
inline void M4x4_SetY(f32* m, f32 y0, f32 y1, f32 y2, f32 y3);
inline void M4x4_SetZ(f32* m, f32 z0, f32 z1, f32 z2, f32 z3);
inline void M4x4_SetW(f32* m, f32 w0, f32 w1, f32 w2, f32 w3);
inline void M4x4_Multiply(f32* m0, f32* m1, f32* result);
inline void M4x4_Copy(f32* src, f32* tar);
inline void M4x4_RotateX(f32* m, f32 degreesY);
inline void M4x4_RotateY(f32* m, f32 degreesY);
inline void M4x4_RotateZ(f32* m, f32 degreesY);
inline f32 M4x4_GetAngleX(f32* m);
inline f32 M4x4_GetAngleY(f32* m);
inline f32 M4x4_GetAngleZ(f32* m);
inline void M4x4_SetPosition(f32* m, f32 x, f32 y, f32 z);
inline Vec4 M4x4_GetPosition(f32* m);
inline Vec4 M4x4_GetEulerAnglesRadians(f32* m);
inline Vec4 M4x4_GetEulerAnglesDegrees(f32* m);
inline void M4x4_SetEulerAnglesByRadians(f32* m, f32 roll, f32 pitch, f32 yaw);