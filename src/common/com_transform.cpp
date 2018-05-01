#pragma once

#include "com_module.h"

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

inline void Transform_SetRotation(Transform* t, f32 radiansX, f32 radiansY, f32 radiansZ)
{
	M3x3_RotateX(t->rotation.cells, radiansX);
	M3x3_RotateY(t->rotation.cells, radiansY);
	M3x3_RotateZ(t->rotation.cells, radiansZ);
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

}
