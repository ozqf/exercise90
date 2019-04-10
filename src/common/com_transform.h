#pragma once

#include "com_module.h"

#define TRANSFORM_CREATE(varName) Transform varName##; \
Transform_SetToIdentity(&##varName##);

com_internal void Transform_SetToIdentity(Transform* t);

com_internal void Transform_ToM4x4(Transform* t, M4x4* result);
com_internal void Transform_FromM4x4(Transform* t, f32* m4x4);

/////////////////////////////////////////////////////
com_internal void Transform_RotateX(Transform* t, f32 radiansX);
com_internal void Transform_RotateY(Transform* t, f32 radiansY);
com_internal void Transform_RotateZ(Transform* t, f32 radiansZ);

com_internal void Transform_SetRotation(
    Transform* t, f32 radiansX, f32 radiansY, f32 radiansZ);

com_internal Vec3 Transform_GetEulerAnglesDegrees(Transform* t);
com_internal Vec3 Transform_GetEulerAngles(Transform* t);

/////////////////////////////////////////////////////
com_internal void Transform_SetScaleX(Transform* t, f32 scaleX);
com_internal void Transform_SetScaleY(Transform* t, f32 scaleY);
com_internal void Transform_SetScaleZ(Transform* t, f32 scaleZ);
com_internal void Transform_SetScaleF(
    Transform* t, f32 scaleX, f32 scaleY, f32 scaleZ);
com_internal void Transform_SetScaleSafe(Transform* t, Vec3 scale);

com_internal f32 Transform_GetScaleX(Transform* t);
com_internal f32 Transform_GetScaleY(Transform* t);
com_internal f32 Transform_GetScaleZ(Transform* t);
com_internal Vec3 Transform_GetForward(Transform* t);
com_internal Vec3 Transform_GetLeft(Transform* t);
com_internal Vec3 Transform_GetUp(Transform* t);
