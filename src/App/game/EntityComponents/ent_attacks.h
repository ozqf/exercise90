#pragma once

#include "../game.h"

void SV_SpawnTestBullet(
    GameState* gs,
    EntId source,
    f32 x,
    f32 y,
    f32 z,
    f32 pitchDegrees,
    f32 yawDegrees,
    f32 speed
    )
{
    EntitySpawnOptions options = {};
    options.pos.x = x;
    options.pos.y = y;
    options.pos.z = z;
    options.source = source;
    
    options.vel = Vec3_ForwardFromAngles(yawDegrees, pitchDegrees, speed);
    
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_PROJECTILE, &options);
}

inline void SV_FireAttack_01(GameState* gs, AttackInfo* info)
{
    SV_SpawnTestBullet(
        gs,
        info->source,
        info->origin.x,
        info->origin.y,
        info->origin.z,
        info->pitchDegrees,
        info->yawDegrees,
        150
    );
    SV_SpawnTestBullet(
        gs,
        info->source,
        info->origin.x,
        info->origin.y,
        info->origin.z,
        info->pitchDegrees,
        info->yawDegrees - 5,
        150
    );
    SV_SpawnTestBullet(
        gs,
        info->source,
        info->origin.x,
        info->origin.y,
        info->origin.z,
        info->pitchDegrees,
        info->yawDegrees + 5,
        150
    );
}

void SV_FireAttack(GameState* gs, AttackInfo* info)
{
    switch(info->type)
    {
        case 1:
        {
            SV_FireAttack_01(gs, info);
        } break;

        case 2:
        {
            SV_SpawnTestBullet(
                gs,
                info->source,
                info->origin.x,
                info->origin.y,
                info->origin.z,
                info->pitchDegrees,
                info->yawDegrees,
                25
            );
        } break;
    }
}
