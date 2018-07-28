#pragma once

#include "game.h"

inline Vec3 Game_RandomSpawnOffset(f32 rangeX, f32 rangeY, f32 rangeZ)
{
    Vec3 v;
    v.x = COM_Randf32() * (rangeX - -rangeX) + -rangeX;
    v.y = COM_Randf32() * (rangeY - -rangeY) + -rangeY;
    v.z = COM_Randf32() * (rangeZ - -rangeZ) + -rangeZ;
    return v;
}

// TODO: Maybe messy doing this via a Transform.
// Suitable for calculating velcoities and probably little else
// Consider doing properly
inline Vec3 Vec3_ForwardFromAngles(f32 yawDegrees, f32 pitchDegrees, f32 magnitude)
{
    Transform t;
    Transform_SetToIdentity(&t);
    Transform_RotateY(&t, yawDegrees * DEG2RAD);
    Transform_RotateX(&t, pitchDegrees * DEG2RAD);
    Vec3 v;
    v.x = -t.rotation.zAxis.x * magnitude;
    v.y = -t.rotation.zAxis.y * magnitude;
    v.z = -t.rotation.zAxis.z * magnitude;
    return v;
}
