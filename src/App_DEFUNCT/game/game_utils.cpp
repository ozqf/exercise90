#pragma once

#include "game.h"

inline f32 Game_RandomInRange(f32 range)
{
    return COM_STDRandf32() * (range - -range) + -range;
}

inline f32 DotProduct2D(f32 vx0, f32 vy0, f32 vx1, f32 vy1)
{
    return vx0 * vx1 + vy0 * vy1;
}


inline u8 IsPointLeftOfLine(f32 lineX, f32 lineY, f32 lineVX, f32 lineVY, f32 pX, f32 pY)
{
    f32 vx = lineX - pX;
    f32 vy = lineY - pY;
    f32 normalX = lineVY;
    f32 normalY = -lineVX;
    f32 dp = DotProduct2D(vx, vy, normalX, normalY);
    if (dp <= 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

inline u8 Game_IsInfront2D(
    f32 selfX, f32 selfY,
    f32 selfDirX, f32 selfDirY,
    f32 targetX, f32 targetY)
{
    return 1;
}

// range 10 will result in a value between -10 and 10 on given axis
inline Vec3 Game_RandomSpawnOffset(f32 rangeX, f32 rangeY, f32 rangeZ)
{
    Vec3 v;
    v.x = COM_STDRandf32() * (rangeX - -rangeX) + -rangeX;
    v.y = COM_STDRandf32() * (rangeY - -rangeY) + -rangeY;
    v.z = COM_STDRandf32() * (rangeZ - -rangeZ) + -rangeZ;
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

inline EntId AI_FindNearestPlayer(ClientList* clients, GameScene* scene, Vec3 pos)
{
    EntId result = {};
	Client* cl = NULL;
    for (i32 i = 0; i < clients->max; ++i)
    {
        cl = &clients->items[0];
        if (cl->state != CLIENT_STATE_PLAYING) { continue; }
        // TODO: Scan scene for closest client avatar
        //printf("GUTIL Found nearest player ent %d/%d\n", cl->entId.iteration, cl->entId.index);
        return cl->entId;
    }
    return result;
}

inline void Ent_SetProjectileSpawnOptions(
    EntitySpawnOptions* options,
    EntId source,
    i32 team,
    Vec3 pos,
    f32 yawDegrees,
    f32 pitchDegrees,
    f32 speed)
{
    *options = {};
    options->pos = pos;
    options->source = source;
    options->flags = ENT_OPTION_FLAG_TEAM | ENT_OPTION_FLAG_VELOCITY;
    options->team = team;
    options->vel = Vec3_ForwardFromAngles(yawDegrees, pitchDegrees, speed);
}
