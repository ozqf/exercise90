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
    switch (type)
    {
        case LOCAL_ENT_TYPE_IMPACT:
        {
            e->updaterIndex = LOCAL_ENT_UPDATER_DYNAMIC;
            #if 0
            RendObj_SetAsBillboard(&e->rend, 0.3f, 0.3f, 0.3f,
                AppGetTextureIndexByName("textures\\white.bmp")
            );
            #endif
            RendObj_SetAsMesh(&e->rend, 
                Assets_GetMeshDataByName("Cube"),
                1.0f, 1.0f, 0.0f,
                AppGetTextureIndexByName("textures\\white.bmp")
            );
            e->originalScale = { 0.4f, 0.4f, 0.4f };
            e->targetScale = { 0.05f, 0.05f, 0.05f };
            e->tick = 0.0f;
            e->tickMax = 0.5f + (1.1f * COM_STDRandf32());

            e->startColour = { 1, 1, 1, 1 };
            e->endColour = { 0.1f, 0.1f, 0.1f };

            // apply launch
            if (dir != NULL)
            {
                e->dir = *dir;
                e->speed = power;
            }
        } break;

        case LOCAL_ENT_TYPE_SPAWN:
        {
            e->updaterIndex = LOCAL_ENT_UPDATER_STATIC;
            RendObj_SetAsMesh(&e->rend, 
                Assets_GetMeshDataByName("Cube"),
                1.0f, 1.0f, 0.0f,
                AppGetTextureIndexByName("textures\\white_v_lines.bmp")
            );
            
            e->tick = 0;
            e->tickMax = 0.35f;
            e->startColour = { 1, 1, 0, 1 };
            e->endColour = { 0.2f, 0.2f, 0 };
            e->originalScale.x = 3.0f;
            e->originalScale.y = 10.0f;
            e->originalScale.z = 3.0f;
            e->targetScale.x = 0.1f;
            e->targetScale.y = 10.0f;
            e->targetScale.z = 0.1f;
            e->scale = e->originalScale;
        } break;

        case LOCAL_ENT_TYPE_EXPLOSION:
        {
            e->updaterIndex = LOCAL_ENT_UPDATER_STATIC;
            RendObj_SetAsMesh(&e->rend, 
                Assets_GetMeshDataByName("Cube"),
                1.0f, 1.0f, 0.0f,
                AppGetTextureIndexByName("textures\\white.bmp")
            );
            e->tick = 0;
            e->tickMax = 0.2f;
            e->startColour = { 1, 1, 0, 1 };
            e->endColour = { 0.3f, 0.2f, 0 };
            e->originalScale = { 0.5f, 0.5f, 0.5f };
            e->targetScale = { 5.5f, 3.5f, 5.5f };

            for (i32 i = 0; i < 30; ++i)
            {
                Vec3 offset = Game_RandomSpawnOffset(2.5f, 0.5f, 2.5f);
                offset.x += x;
                offset.y += y;
                offset.z += z;

                Game_SpawnLocalEntity(
                    offset.x, offset.y, offset.z,
                    NULL, 0, LOCAL_ENT_TYPE_IMPACT);
            }

        } break;
    }
    
}

void LocalEnt_InterpolateColour(LocalEnt* e, f32 dt)
{
	f32 dr = e->endColour.red - e->startColour.red;
	f32 dg = e->endColour.green - e->startColour.green;
	f32 db = e->endColour.blue - e->startColour.blue;
	//f32 da = e->endColour.alpha - e->startColour.alpha;
    e->rend.SetColour(
        COM_LinearEase(e->tick, e->startColour.red, dr, e->tickMax),
        COM_LinearEase(e->tick, e->startColour.green, dg, e->tickMax),
        COM_LinearEase(e->tick, e->startColour.blue, db, e->tickMax)
    );
}

void LocalEnt_InterpolateScale(LocalEnt* e, f32 dt)
{
	f32 dx = e->targetScale.x - e->originalScale.x;
    f32 dy = e->targetScale.y - e->originalScale.y;
    f32 dz = e->targetScale.z - e->originalScale.z;
    e->scale.x = COM_LinearEase(e->tick, e->originalScale.x, dx, e->tickMax);
    e->scale.y = COM_LinearEase(e->tick, e->originalScale.y, dy, e->tickMax);
    e->scale.z = COM_LinearEase(e->tick, e->originalScale.z, dz, e->tickMax);
}

void LocalEnt_Tick_Impact(LocalEnt *e, f32 dt, u8 verbose)
{
    if (e->tick > e->tickMax)
    {
        e->status = LOCAL_ENT_STATUS_FREE;
        return;
    }
    else
    {
        LocalEnt_InterpolateScale(e, dt);
	    LocalEnt_InterpolateColour(e, dt);
        Vec3 vel;
        Vec3_Set(&vel,
                 e->dir.x * e->speed,
                 e->dir.y * e->speed,
                 e->dir.z * e->speed
        );
        e->pos.x += vel.x * dt;
        e->pos.y += vel.y * dt;
        e->pos.z += vel.z * dt;
        e->tick += dt;
    }
}

void LocalEnt_Tick_Static(LocalEnt *e, f32 dt, u8 verbose)
{
    LocalEnt_InterpolateScale(e, dt);
	LocalEnt_InterpolateColour(e, dt);
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
        switch (e->updaterIndex)
        {
        case LOCAL_ENT_UPDATER_DYNAMIC:
            LocalEnt_Tick_Impact(e, dt, verbose);
            break;
        case LOCAL_ENT_TYPE_SPAWN:
            LocalEnt_Tick_Static(e, dt, verbose);
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
