#pragma once

#include "com_module.h"

inline void Transform_ToM4x4(Transform* t, f32* m4x4);
inline void Transform_FromM4x4(Transform* t, f32* m4x4);

/////////////////////////////////////////////////////
inline void Transform_RotateX(Transform* t, f32 radiansX);
inline void Transform_RotateY(Transform* t, f32 radiansY);
inline void Transform_RotateZ(Transform* t, f32 radiansZ);

inline void Transform_SetRotation(Transform* t, f32 radiansX, f32 radiansY, f32 radiansZ);

inline Vec3 Transform_GetEulerAnglesDegrees(Transform* t);
inline Vec3 Transform_GetEulerAngles(Transform* t);

/////////////////////////////////////////////////////
inline void Transform_SetScaleX(Transform* t, f32 scaleX);
inline void Transform_SetScaleY(Transform* t, f32 scaleY);
inline void Transform_SetScaleZ(Transform* t, f32 scaleZ);

inline f32 Transform_GetScaleX(Transform* t);
inline f32 Transform_GetScaleY(Transform* t);
inline f32 Transform_GetScaleZ(Transform* t);
