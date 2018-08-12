#pragma once

#include "game.h"

inline void LocalEnt_Reset(LocalEnt* e)
{
    *e = {};
    e->scale = { 1, 1, 1 };
}

inline void Game_ResetLocalEntities()
{
    for (i32 i = 0; i < GAME_MAX_LOCAL_ENTITIES; ++i)
    {
        LocalEnt_Reset(&g_localEntities[i]);
    }
}

inline LocalEnt* Game_GetFreeLocalEntitiy()
{
	for (i32 i = 0; i < GAME_MAX_LOCAL_ENTITIES; ++i)
	{
		LocalEnt* e = &g_localEntities[i];
		if (e->status != LOCAL_ENT_STATUS_FREE) { continue; }
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
    Vec3* dir,
    f32 power
)
{
	LocalEnt* e = Game_GetFreeLocalEntitiy();
	if (e == NULL) { return; }
	Vec3_Set(&e->pos, x, y, z);
	RendObj_SetAsBillboard(&e->rend, 1, 1, 1,
        AppGetTextureIndexByName("textures\\white.bmp")
    );
    Vec3_Set(&e->scale, 0.2f, 0.2f, 0.2f);
	e->rend.flags = 0 | RENDOBJ_FLAG_DEBUG;
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
}

void Game_TickLocalEntities(f32 dt, u8 verbose)
{
    for (i32 i = 0; i < GAME_MAX_LOCAL_ENTITIES; ++i)
    {
        LocalEnt* e = &g_localEntities[i];
        if (e->status != LOCAL_ENT_STATUS_IN_USE) { continue; }
        if (e->tick <= 0)
        {
            if (verbose)
            {
                printf("GAME Removing local ent %d\n", i);
            }
            e->status = LOCAL_ENT_STATUS_FREE;
            continue;
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
            if (verbose)
            {
                printf("GAME Tick local ent %d: %.2f\n", i, e->tick);
            }
        }
    }
}

void Game_AddLocalEntitiesToRender(RenderScene* scene)
{
    for (i32 i = 0; i < GAME_MAX_LOCAL_ENTITIES; ++i)
    {
        LocalEnt* e = &g_localEntities[i];
        if (e->status != LOCAL_ENT_STATUS_IN_USE) { continue; }

        RScene_AddRenderItem(scene, &e->rend,
            e->pos.x, e->pos.y, e->pos.z,
            e->scale.x, e->scale.y, e->scale.z
        );
    }
}
