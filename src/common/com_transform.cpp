#pragma once

#include "common.h"

/////////////////////////////////////////////////////////////////////
// Position
/////////////////////////////////////////////////////////////////////
com_internal void Transform_SetToIdentity(Transform* t)
{
	*t = {};
    t->pos = { 0, 0, 0 };
    t->scale = { 1, 1, 1 };
    M3x3_SetToIdentity(t->rotation.cells);
}

/////////////////////////////////////////////////////////////////////
// Position
/////////////////////////////////////////////////////////////////////
com_internal void Transform_SetPosX(Transform* t, f32 posX)
{
    t->pos.x = posX;
}

com_internal void Transform_SetPosY(Transform* t, f32 posY)
{
    t->pos.y = posY;
}

com_internal void Transform_SetPosZ(Transform* t, f32 posZ)
{
    t->pos.z = posZ;
}

com_internal void Transform_SetPosition(Transform* t, f32 posX, f32 posY, f32 posZ)
{
    t->pos = { posX, posY, posZ };
}

/////////////////////////////////////////////////////////////////////
// Rotation
/////////////////////////////////////////////////////////////////////
com_internal void Transform_RotateX(Transform* t, f32 radiansX)
{
    M3x3_RotateX(t->rotation.cells, radiansX);
}

void Transform_RotateY(Transform* t, f32 radiansY)
{
    M3x3_RotateY(t->rotation.cells, radiansY);
}

com_internal void  Transform_RotateZ(Transform* t, f32 radiansZ)
{
    M3x3_RotateZ(t->rotation.cells, radiansZ);
}

com_internal void  Transform_SetRotationDegrees(Transform* t, f32 degreesX, f32 degreesY, f32 degreesZ)
{
    M3x3_RotateZ(t->rotation.cells, degreesZ * DEG2RAD);
    M3x3_RotateY(t->rotation.cells, degreesY * DEG2RAD);
	M3x3_RotateX(t->rotation.cells, degreesX * DEG2RAD);
}

com_internal void  Transform_SetRotation(Transform* t, f32 radiansX, f32 radiansY, f32 radiansZ)
{
    M3x3_RotateZ(t->rotation.cells, radiansZ);
    M3x3_RotateY(t->rotation.cells, radiansY);
	M3x3_RotateX(t->rotation.cells, radiansX);
}

com_internal void  Transform_ClearRotation(Transform* t)
{
	M3x3_SetToIdentity(t->rotation.cells);
}

/////////////////////////////////////////////////////////////////////
// Scale
/////////////////////////////////////////////////////////////////////
com_internal void  Transform_SetScaleF(Transform* t, f32 scaleX, f32 scaleY, f32 scaleZ)
{
	t->scale.x = scaleX;
	t->scale.y = scaleY;
	t->scale.z = scaleZ;
}

// Avoid zero scales
com_internal void  Transform_SetScaleSafe(Transform* t, Vec3 scale)
{
    if (scale.x != 0 && scale.y != 0 && scale.z != 0)
    {
        t->scale = scale;
    }
    else
    {
        t->scale = { 1, 1, 1 };
    }
}

/////////////////////////////////////////////////////////////////////
// Conversion
/////////////////////////////////////////////////////////////////////
com_internal void  Transform_ToM4x4(Transform* t, M4x4* result)
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

/*
DOES NOT AFFECT SCALE!
*/
com_internal void Transform_FromM4x4(Transform* t, M4x4* source)
{
    t->pos.x = source->w0;
    t->pos.y = source->w1;
    t->pos.z = source->w2;
    M3x3_CopyFromM4x4(t->rotation.cells, source->cells);
}

com_internal Vec3 Transform_GetEulerAnglesDegrees(Transform* t)
{
    return M3x3_GetEulerAnglesDegrees(t->rotation.cells);
}

com_internal Vec3 Transform_GetEulerAngles(Transform* t)
{
    return M3x3_GetEulerAnglesRadians(t->rotation.cells);
}

com_internal void  Transform_SetByPosAndDegrees(Transform* t, Vec3* pos, Vec3* degrees)
{
    Transform_ClearRotation(t);
	Transform_RotateZ(t, degrees->z * DEG2RAD);
	Transform_RotateY(t, degrees->y * DEG2RAD);
	Transform_RotateX(t, degrees->x * DEG2RAD);
    Transform_SetPosition(t, pos->x, pos->y, pos->z);
}

/////////////////////////////////////////////////////////////////////
// Retrieve Orientation info
/////////////////////////////////////////////////////////////////////

com_internal Vec3 Transform_GetForward(Transform* t)
{
    return {
        -t->rotation.zAxis.x,
        -t->rotation.zAxis.y,
        -t->rotation.zAxis.z
    };
}

com_internal Vec3 Transform_GetLeft(Transform* t)
{
    return {
        t->rotation.xAxis.x,
        t->rotation.xAxis.y,
        t->rotation.xAxis.z
    };
}

com_internal Vec3 Transform_GetUp(Transform* t)
{
    return {
        t->rotation.yAxis.x,
        t->rotation.yAxis.y,
        t->rotation.yAxis.z
    };
}
