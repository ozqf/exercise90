#pragma once

#include "com_module.h"

/////////////////////////////////////////////////////////////////////
// Position
/////////////////////////////////////////////////////////////////////
inline void Transform_SetToIdentity(Transform* t)
{
	*t = {};
    t->pos = { 0, 0, 0 };
    t->scale = { 1, 1, 1 };
    M3x3_SetToIdentity(t->rotation.cells);
}

/////////////////////////////////////////////////////////////////////
// Position
/////////////////////////////////////////////////////////////////////
inline void Transform_SetPosX(Transform* t, f32 posX)
{
    t->pos.x = posX;
}

inline void Transform_SetPosY(Transform* t, f32 posY)
{
    t->pos.y = posY;
}

inline void Transform_SetPosZ(Transform* t, f32 posZ)
{
    t->pos.z = posZ;
}

inline void Transform_SetPosition(Transform* t, f32 posX, f32 posY, f32 posZ)
{
    t->pos = { posX, posY, posZ };
}

/////////////////////////////////////////////////////////////////////
// Rotation
/////////////////////////////////////////////////////////////////////
inline void Transform_RotateX(Transform* t, f32 radiansX)
{
    M3x3_RotateX(t->rotation.cells, radiansX);
}

inline void Transform_RotateY(Transform* t, f32 radiansY)
{
    M3x3_RotateY(t->rotation.cells, radiansY);
}

inline void Transform_RotateZ(Transform* t, f32 radiansZ)
{
    M3x3_RotateZ(t->rotation.cells, radiansZ);
}

inline void Transform_SetRotationDegrees(Transform* t, f32 degreesX, f32 degreesY, f32 degreesZ)
{
	M3x3_RotateX(t->rotation.cells, degreesX * DEG2RAD);
	M3x3_RotateY(t->rotation.cells, degreesY * DEG2RAD);
	M3x3_RotateZ(t->rotation.cells, degreesZ * DEG2RAD);
}

inline void Transform_SetRotation(Transform* t, f32 radiansX, f32 radiansY, f32 radiansZ)
{
	M3x3_RotateX(t->rotation.cells, radiansX);
	M3x3_RotateY(t->rotation.cells, radiansY);
	M3x3_RotateZ(t->rotation.cells, radiansZ);
}

inline void Transform_ClearRotation(Transform* t)
{
	M3x3_SetToIdentity(t->rotation.cells);
}

/////////////////////////////////////////////////////////////////////
// Scale
/////////////////////////////////////////////////////////////////////
inline void Transform_SetScale(Transform* t, f32 scaleX, f32 scaleY, f32 scaleZ)
{
	t->scale.x = scaleX;
	t->scale.y = scaleY;
	t->scale.z = scaleZ;
}

/////////////////////////////////////////////////////////////////////
// Conversion
/////////////////////////////////////////////////////////////////////
inline void Transform_ToM4x4(Transform* t, M4x4* result)
{
    M4x4_SetToIdentity(result->cells);
    result->x0 = t->rotation.x0;
    result->x1 = t->rotation.x1;
    result->x2 = t->rotation.x2;

    result->y0 = t->rotation.y0;
    result->y1 = t->rotation.y1;
    result->y2 = t->rotation.y2;

    result->z0 = t->rotation.z0;
    result->z1 = t->rotation.z1;
    result->z2 = t->rotation.z2;

    result->w0 = t->pos.x;
    result->w1 = t->pos.y;
    result->w2 = t->pos.z;

    Vec4_SetMagnitude(&result->xAxis, t->scale.x);
    Vec4_SetMagnitude(&result->yAxis, t->scale.y);
    Vec4_SetMagnitude(&result->zAxis, t->scale.z);
}

inline void Transform_FromM4x4(Transform* t, M4x4* source)
{
    t->pos.x = source->w0;
    t->pos.y = source->w1;
    t->pos.z = source->w2;
    M3x3_CopyFromM4x4(t->rotation.cells, source->cells);
    t->scale.x = 1;
    t->scale.y = 1;
    t->scale.z = 1;
}

inline Vec3 Transform_GetEulerAnglesDegrees(Transform* t)
{
    return M3x3_GetEulerAnglesDegrees(t->rotation.cells);
}
