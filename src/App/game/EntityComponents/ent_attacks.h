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
    
    //Game_WriteSpawnCmd(gs, ENTITY_TYPE_PROJECTILE, &options);
    static EntityState state;
    if (Game_PrepareSpawnCmd(gs, ENTITY_TYPE_PROJECTILE, &state, &options))
    {
		state.renderState.colourRGB[0] = 1;
		state.renderState.colourRGB[1] = 0;
		state.renderState.colourRGB[2] = 1;
        Ent_WriteEntityStateCmd(NULL, &state);
    }
}

inline void SV_FireAttack_01(GameState* gs, AttackInfo* info)
{
    EntitySpawnOptions options = {};
    
    EntityState state;
    Ent_SetProjectileSpawnOptions(
        &options,
        info->source,
        info->origin,
        info->yawDegrees,
        info->pitchDegrees,
        150
    );
    if (Game_PrepareSpawnCmd(gs, ENTITY_TYPE_PROJECTILE, &state, &options))
    {
        state.renderState.colourRGB[0] = 0.3f;
        state.renderState.colourRGB[1] = 0.3f;
        state.renderState.colourRGB[2] = 1;
        Ent_WriteEntityStateCmd(NULL, &state);
    }
    #if 0
    Ent_SetProjectileSpawnOptions(
        &options,
        info->source,
        info->origin,
        info->yawDegrees - 5,
        info->pitchDegrees,
        150
    );
    if (Game_PrepareSpawnCmd(gs, ENTITY_TYPE_PROJECTILE, &state, &options))
    {
        state.renderState.colourRGB[0] = 0.3f;
        state.renderState.colourRGB[1] = 0.3f;
        state.renderState.colourRGB[2] = 1;
        Ent_WriteEntityStateCmd(NULL, &state);
    }

    Ent_SetProjectileSpawnOptions(
        &options,
        info->source,
        info->origin,
        info->yawDegrees + 5,
        info->pitchDegrees,
        150
    );
    if (Game_PrepareSpawnCmd(gs, ENTITY_TYPE_PROJECTILE, &state, &options))
    {
        state.renderState.colourRGB[0] = 0.3f;
        state.renderState.colourRGB[1] = 0.3f;
        state.renderState.colourRGB[2] = 1;
        Ent_WriteEntityStateCmd(NULL, &state);
    }
    #endif

    #if 0
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
    #endif
}

inline void SV_FireAttack_02(GameState* gs, AttackInfo* info)
{
    EntitySpawnOptions options = {};
    EntityState state;
    Ent_SetProjectileSpawnOptions(
        &options,
        info->source,
        info->origin,
        info->yawDegrees,
        info->pitchDegrees,
        25
    );
    if (Game_PrepareSpawnCmd(gs, ENTITY_TYPE_PROJECTILE, &state, &options))
    {
        state.projectileState.ticker.tickMax = 2.0f;
        state.projectileState.ticker.tick = 2.0f;
        state.renderState.colourRGB[0] = 1;
        state.renderState.colourRGB[1] = 1;
        state.renderState.colourRGB[2] = 0;
        Ent_WriteEntityStateCmd(NULL, &state);
    }
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
            #if 1
            SV_FireAttack_02(gs, info);
            #endif

            #if 0
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
            #endif
        } break;
    }
}
