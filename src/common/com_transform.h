#pragma once

#include "com_module.h"

void Transform_SetToIdentity(Transform* t);

void Transform_ToM4x4(Transform* t, f32* m4x4);
void Transform_FromM4x4(Transform* t, f32* m4x4);

/////////////////////////////////////////////////////
void Transform_RotateX(Transform* t, f32 radiansX);
void Transform_RotateY(Transform* t, f32 radiansY);
void Transform_RotateZ(Transform* t, f32 radiansZ);

void Transform_SetRotation(Transform* t, f32 radiansX, f32 radiansY, f32 radiansZ);

Vec3 Transform_GetEulerAnglesDegrees(Transform* t);
Vec3 Transform_GetEulerAngles(Transform* t);

/////////////////////////////////////////////////////
void Transform_SetScaleX(Transform* t, f32 scaleX);
void Transform_SetScaleY(Transform* t, f32 scaleY);
void Transform_SetScaleZ(Transform* t, f32 scaleZ);

f32 Transform_GetScaleX(Transform* t);
f32 Transform_GetScaleY(Transform* t);
f32 Transform_GetScaleZ(Transform* t);
