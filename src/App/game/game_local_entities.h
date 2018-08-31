#pragma once

#include "game.h"

inline void LocalEnt_Reset(LocalEnt *e)
{
    *e = {};
    e->scale = {1, 1, 1};
}

inline void Game_ResetLocalEntities()
{
    for (i32 i = 0; i < GAME_MAX_LOCAL_ENTITIES; ++i)
    {
        LocalEnt_Reset(&g_localEntities[i]);
    }
}

inline LocalEnt *Game_GetFreeLocalEntitiy()
{
    for (i32 i = 0; i < GAME_MAX_LOCAL_ENTITIES; ++i)
    {
        LocalEnt *e = &g_localEntities[i];
        if (e->status != LOCAL_ENT_STATUS_FREE)
        {
            continue;
        }
        LocalEnt_Reset(e);
        e->id = i;
        e->status = LOCAL_ENT_STATUS_IN_USE;
        return e;
    }
    return NULL;
}

// Dir can be null, which will force no launch movement
void Game_SpawnLocalEntity(
    f32 x, f32 y, f32 z,
    Vec3 *dir,
    f32 power,
    i32 type)
{
    LocalEnt *e = Game_GetFreeLocalEntitiy();
    if (e == NULL)
    {
        return;
    }
    Vec3_Set(&e->pos, x, y, z);
    e->type = type;
    switch (type)
    {
        case LOCAL_ENT_TYPE_IMPACT:
        {
            RendObj_SetAsBillboard(&e->rend, 0.3f, 0.3f, 0.3f,
                AppGetTextureIndexByName("textures\\white.bmp")
            );
            Vec3_Set(&e->scale, 0.2f, 0.2f, 0.2f);
            //e->rend.flags = 0 | RENDOBJ_FLAG_DEBUG;
            e->tick = 1.0f;

            // apply launch
            if (dir != NULL)
            {
                // printf("Spawn local dir: %.2f, %.2f, %.2f\n",
                //     e->dir.x, e->dir.y, e->dir.z
                // );
                e->dir = *dir;
                e->speed = power;
            }
        } break;

        case LOCAL_ENT_TYPE_SPAWN:
        {
            RendObj_SetAsMesh(&e->rend, 
                Assets_GetMeshDataByName("Cube"),
                1.0, 1.0, 0.0f,
                AppGetTextureIndexByName("textures\\white_v_lines.bmp")
            );
            
            e->tick = 0;
            e->tickMax = 0.35f;
            e->originalScale.x = 3.0f;
            e->originalScale.y = 10.0f;
            e->originalScale.z = 3.0f;
            e->targetScale.x = 0.1f;
            //e->targetScale.y = 0.1f;
            e->targetScale.z = 0.1f;
            e->scale = e->originalScale;
        } break;
    }
    
}

void LocalEnt_Tick_Impact(LocalEnt *e, f32 dt, u8 verbose)
{
    if (e->tick <= 0)
    {
        e->status = LOCAL_ENT_STATUS_FREE;
        return;
    }
    else
    {
        Vec3 vel;
        Vec3_Set(&vel,
                 e->dir.x * e->speed,
                 e->dir.y * e->speed,
                 e->dir.z * e->speed
        );
        e->pos.x += vel.x * dt;
        e->pos.y += vel.y * dt;
        e->pos.z += vel.z * dt;
        //e->scale.x = 1 * e->tick;
        //e->scale.y = 1 * e->tick;
        //e->scale.z = 1 * e->tick;
        e->tick -= dt;
    }
}

void LocalEnt_Tick_Spawn(LocalEnt *e, f32 dt, u8 verbose)
{
    //f32 lerp = (e->tickMax / 100.0f) * e->tick;
    //f32 lerp = e->tickMax / 100.0f;
    //lerp /= 100;
    f32 dx = e->targetScale.x - e->originalScale.x;
    f32 dy = e->targetScale.y - e->originalScale.y;
    f32 dz = e->targetScale.z - e->originalScale.z;
    e->scale.x = COM_LinearEase(e->tick, e->originalScale.x, dx, e->tickMax);
    //e->scale.y = COM_LinearEase(e->tick, e->originalScale.y, dy, e->tickMax);
    e->scale.z = COM_LinearEase(e->tick, e->originalScale.z, dz, e->tickMax);
    // printf("GFX Tick %.2f SCALE: %.2f, %.2f, %.2f\n",
    //     e->tick, e->scale.x, e->scale.y, e->scale.z);
    if (e->tick > e->tickMax)
    {
        e->status = LOCAL_ENT_STATUS_FREE;
    }
    e->tick += dt;
}

void Game_TickLocalEntities(f32 dt, u8 verbose)
{
    for (i32 i = 0; i < GAME_MAX_LOCAL_ENTITIES; ++i)
    {
        LocalEnt *e = &g_localEntities[i];
        if (e->status != LOCAL_ENT_STATUS_IN_USE)
        {
            continue;
        }
        switch (e->type)
        {
        case LOCAL_ENT_TYPE_IMPACT:
            LocalEnt_Tick_Impact(e, dt, verbose);
            break;
        case LOCAL_ENT_TYPE_SPAWN:
            LocalEnt_Tick_Spawn(e, dt, verbose);
            break;
        default:
            e->status = LOCAL_ENT_STATUS_FREE;
            break;
        }
    }
}

void Game_AddLocalEntitiesToRender(RenderScene *scene)
{
    for (i32 i = 0; i < GAME_MAX_LOCAL_ENTITIES; ++i)
    {
        LocalEnt *e = &g_localEntities[i];
        if (e->status != LOCAL_ENT_STATUS_IN_USE)
        {
            continue;
        }

        RScene_AddRenderItem(scene, &e->rend,
                             e->pos.x, e->pos.y, e->pos.z,
                             e->scale.x, e->scale.y, e->scale.z);
    }
}
