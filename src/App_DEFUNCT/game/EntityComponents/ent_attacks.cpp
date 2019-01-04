#pragma once

#include "../game.h"

// Player rapid
inline void SV_FireAttack_01(GameScene *gs, AttackInfo *info)
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
    if (Ent_PrepareSpawnCmd(
        gs, ENTITY_TYPE4_PROJECTILE, ENTITY_CATAGORY_REPLICATED, &state, &options))
    {
        //state.projectileState.move = options.vel;
        state.renderState.colourRGB[0] = 0.3f;
        state.renderState.colourRGB[1] = 0.3f;
        state.renderState.colourRGB[2] = 1;
        Ent_WriteStateCmdToAppBuffer(NULL, &state);
    }
}

// Enemy regular
inline void SV_FireAttack_02(GameScene *gs, AttackInfo *info)
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
        5);
    if (Ent_PrepareSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, ENTITY_CATAGORY_REPLICATED, &state, &options))
    {
        state.projectileState.ticker.tickMax = 2.0f;
        state.projectileState.ticker.tick = 2.0f;
        state.renderState.colourRGB[0] = 1;
        state.renderState.colourRGB[1] = 1;
        state.renderState.colourRGB[2] = 0;
        Ent_WriteStateCmdToAppBuffer(NULL, &state);
    }
}

//#define ATK_PLAYER_SHOTGUN_SPEED 250
#define ATK_PLAYER_SHOTGUN_SPEED 50
// player shotgun
inline void SV_FireAttack_03(GameScene *gs, AttackInfo *info)
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
        ATK_PLAYER_SHOTGUN_SPEED);
    if (Ent_PrepareSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, ENTITY_CATAGORY_REPLICATED, &state, &options))
    {
        state.renderState.colourRGB[0] = 0.3f;
        state.renderState.colourRGB[1] = 0.3f;
        state.renderState.colourRGB[2] = 1;
        Ent_WriteStateCmdToAppBuffer(NULL, &state);
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
            ATK_PLAYER_SHOTGUN_SPEED);
        if (Ent_PrepareSpawnCmd(gs, ENTITY_TYPE4_PROJECTILE, ENTITY_CATAGORY_REPLICATED, &state, &options))
        {
            state.renderState.colourRGB[0] = 0.3f;
            state.renderState.colourRGB[1] = 0.3f;
            state.renderState.colourRGB[2] = 1;
            Ent_WriteStateCmdToAppBuffer(NULL, &state);
        }
    }
}

inline void SV_FireAttack_04(GameScene *gs, AttackInfo *info)
{
    
}

void SV_FireAttack(GameScene *gs, AttackInfo *info)
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
