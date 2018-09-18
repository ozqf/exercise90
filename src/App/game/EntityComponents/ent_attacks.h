#pragma once

#include "../game.h"

void SV_SpawnTestBullet(
    GameState *gs,
    EntId source,
    i32 team,
    f32 x,
    f32 y,
    f32 z,
    f32 pitchDegrees,
    f32 yawDegrees,
    f32 speed)
{
    EntitySpawnOptions options = {};
    options.pos.x = x;
    options.pos.y = y;
    options.pos.z = z;

    options.source = source;
    options.flags |= ENT_OPTION_FLAG_TEAM;
    options.team = team;

    options.vel = Vec3_ForwardFromAngles(yawDegrees, pitchDegrees, speed);

    //Ent_WriteSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, &options);
    static EntityState state;
    if (Ent_PrepareSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, &state, &options))
    {
        state.renderState.colourRGB[0] = 1;
        state.renderState.colourRGB[1] = 0;
        state.renderState.colourRGB[2] = 1;
        Ent_WriteEntityStateCmd(NULL, &state);
    }
}

// Player rapid
inline void SV_FireAttack_01(GameState *gs, AttackInfo *info)
{
    EntitySpawnOptions options = {};

    EntityState state;
    Ent_SetProjectileSpawnOptions(
        &options,
        info->source,
        info->team,
        info->origin,
        info->yawDegrees,
        info->pitchDegrees,
        250);
    if (Ent_PrepareSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, &state, &options))
    {
        state.renderState.colourRGB[0] = 0.3f;
        state.renderState.colourRGB[1] = 0.3f;
        state.renderState.colourRGB[2] = 1;
        Ent_WriteEntityStateCmd(NULL, &state);
    }
}

// Enemy regular
inline void SV_FireAttack_02(GameState *gs, AttackInfo *info)
{
    EntitySpawnOptions options = {};
    EntityState state;
    Ent_SetProjectileSpawnOptions(
        &options,
        info->source,
        info->team,
        info->origin,
        info->yawDegrees,
        info->pitchDegrees,
        15);
    if (Ent_PrepareSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, &state, &options))
    {
        state.projectileState.ticker.tickMax = 2.0f;
        state.projectileState.ticker.tick = 2.0f;
        state.renderState.colourRGB[0] = 1;
        state.renderState.colourRGB[1] = 1;
        state.renderState.colourRGB[2] = 0;
        Ent_WriteEntityStateCmd(NULL, &state);
    }
}

// player shotgun
inline void SV_FireAttack_03(GameState *gs, AttackInfo *info)
{
    EntitySpawnOptions options = {};

    EntityState state;
    Ent_SetProjectileSpawnOptions(
        &options,
        info->source,
        info->team,
        info->origin,
        info->yawDegrees,
        info->pitchDegrees,
        250);
    if (Ent_PrepareSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, &state, &options))
    {
        state.renderState.colourRGB[0] = 0.3f;
        state.renderState.colourRGB[1] = 0.3f;
        state.renderState.colourRGB[2] = 1;
        Ent_WriteEntityStateCmd(NULL, &state);
    }
    f32 spreadX = 3.0f;
    f32 spreadY = 1.5f;// 3f;
    for (i32 i = 0; i < 12; ++i)
    {
        f32 randOffsetX = Game_RandomInRange(spreadX);
        f32 randOffsetY = Game_RandomInRange(spreadY);

        Ent_SetProjectileSpawnOptions(
            &options,
            info->source,
            info->team,
            info->origin,
            info->yawDegrees + randOffsetX,
            info->pitchDegrees + randOffsetY,
            250);
        if (Ent_PrepareSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, &state, &options))
        {
            state.renderState.colourRGB[0] = 0.3f;
            state.renderState.colourRGB[1] = 0.3f;
            state.renderState.colourRGB[2] = 1;
            Ent_WriteEntityStateCmd(NULL, &state);
        }
    }
}

inline void SV_FireAttack_04(GameState *gs, AttackInfo *info)
{
    
}

void SV_FireAttack(GameState *gs, AttackInfo *info)
{
    switch (info->type)
    {
        case 1:
        {
            SV_FireAttack_01(gs, info);
        }
        break;

        case 2:
        {
            SV_FireAttack_02(gs, info);
        } break;

        case 3:
        {
            SV_FireAttack_03(gs, info);
        } break;

        default:
        {
            printf("GAME: Unknown attack %d\n", info->type);
        } break;
    }
}
