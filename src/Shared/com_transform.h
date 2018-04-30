#pragma once


struct Transform
{
	// Vec3 pos;
	// Vec3 rot;
    M4x4 matrix;
    Vec3 scale;
};

/****************************************************************
Global Functions
****************************************************************/
inline void Transform_SetScale(Transform* t, f32 x, f32 y, f32 z)
{
    t->scale.x = x;
    t->scale.y = y;
    t->scale.z = z;
}

inline void Transform_SetToIdentity(Transform* t)
{
    *t = {};
    t->scale.x = 1;
    t->scale.y = 1;
    t->scale.z = 1;

    t->matrix.x0 = 1;
    t->matrix.y1 = 1;
    t->matrix.z2 = 1;
    t->matrix.w3 = 1;
}

inline u32 SafeTruncateUInt64(u64 value)
{
	// TODO: Defines for max value
	Assert(value <= 0xFFFFFFFF);
	u32 result = (u32)value;
	return result;
}
