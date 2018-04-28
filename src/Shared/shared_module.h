#pragma once

#include "shared_types.h"

/////////////////////////////////////////////////////////////////////////////
// 4x4 MATRIX OPERATIONS
/////////////////////////////////////////////////////////////////////////////

void M4x4_SetToIdentity(f32* matrix);
void M4x4_SetAsScale(f32* matrix, f32 scaleX, f32 scaleY, f32 scaleZ);
void M4x4_SetMove(f32* matrix, f32 deltaX, f32 deltaY, f32 deltaZ);
void M4x4_SetRotation_Z(f32* matrix, f32 theta);
void M4x4_SetProjection(f32* m, f32 prjNear, f32 prjFar, f32 prjLeft, f32 prjRight, f32 prjTop, f32 prjBottom);
void M4x4_SetOrthoProjection(f32* m, f32 left, f32 right, f32 top, f32 bottom, f32 prjNear, f32 prjFar);
void Vec3_MultiplyByMatrix(f32* m, f32* v, f32* result);
f32 COM_CapAngleDegrees(f32 angle);
void M4x4_SetColumns3x3(f32* m, Vec3 x, Vec3 y, Vec3 z);
void M4x4_SetRows3x3(f32* m, Vec3 x, Vec3 y, Vec3 z);
void M4x4_SetCameraMatrix(f32* m, Vec3 xAxis, Vec3 yAxis, Vec3 zAxis, Vec3 pos);
void M4x4_SetRotation(f32* m, f32 a, f32 b, f32 c);
void M4x4_CreateRotation2(f32* matrix, Vec3* degrees);
void AngleToAxes(Vec3* angles, Vec3* left, Vec3* up, Vec3* forward);
inline void Calc_CameraForward(Vec3* angles, Vec3* result);
void Calc_AnglesToAxesZXY(Vec3* angles, Vec3* left, Vec3* up, Vec3* forward);
void Calc_AnglesToAxesZXY(Vec3* angles, AngleVectors* vectors);
void Calc_AnglesToAxesZYX(Vec3* angles, Vec3* left, Vec3* up, Vec3* forward);
void Calc_AnglesToAxesZYX(Vec3* angles, AngleVectors* vectors);
void LookAtToAxes(Vec3* pos, Vec3* target, Vec3* upDir, Vec3* left, Vec3* up, Vec3* forward);
void M4x4_LookAt(f32* m, Vec3* eye, Vec3* centre, Vec3* up);

//////////////////////////////////////////////

void COM_CopyMemory(u8* source, u8* target, u32 numBytes);

void COM_ZeroMemory(u8 *ptr, u32 numBytes);

void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val);

void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize);

void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes);

u32 COM_AlignSize(u32 value, u32 alignment);

///////////////////////////////////////////////////////////////////////
// Macros
///////////////////////////////////////////////////////////////////////
#ifndef COM_COPY
#define COM_COPY(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##))
#endif

#ifndef COM_COPY_STEP
#define COM_COPY_STEP(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##)); \
ptrDestination = (u8*)((u32)ptrDestination + (u32)sizeof(##structType##))
#endif
