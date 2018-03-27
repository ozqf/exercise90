#pragma once

#include "game.h"

/**
 * Find a free slot in the entity list
 */
Ent* Game_GetFreeEntity()
{
    Ent* e = NULL;
    for (u16 i = 0; i < WORLD_MAX_ENTITIES; ++i)
    {
        e = &g_entities[i];
        if (e->inUse == 0)
        {
            // new iteration of this entity
            u16 iteration = e->entId.iteration;
            *e = {};
            e->inUse = 1;
            e->entId.iteration = iteration + 1;
            e->entId.index = i;
			return e;
        }
    }
    
    Assert(e != NULL);
    return NULL;
}

void Game_InitEntityList()
{
    for (i32 i = 0; i < WORLD_MAX_ENTITIES; ++i)
    {
        g_entities[i].entId.iteration = 0;
    }
}

Ent* Game_GetEntityByIndex(u16 index)
{
    Assert(index < WORLD_MAX_ENTITIES)
    return &g_entities[index];
}

RendObj* Game_GetFreeRenderObj(u16 newOwnerIndex)
{
    RendObj* obj = NULL;
    for (u32 i = 0; i < R_MAX_RENDER_OBJECTS; ++i)
    {
        obj = &g_game_rendObjects[i];
        if (obj->inUse == 0)
        {
            obj->inUse = 1;
            obj->ownerIndex = newOwnerIndex;
            return obj;
        }
    }
    Assert(obj != NULL);
    return NULL;
}

void Game_Init()
{
    g_num_entities = 0;
    g_num_brains = 0;
    g_num_colliders = 0;
    g_numSceneRendComponents = 0;
    g_numUIRendComponents = 0;

    Ent* ent;
    RendObj* obj;

    // Ent 0, walls
    ent = Game_GetFreeEntity();

    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0;
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 6;
    ent->transform.scale.y = 2;
    ent->transform.scale.z = 6;

    obj = Game_GetFreeRenderObj(ent->entId.index);
    
    RendObj_SetAsMesh(obj, &g_meshInverseCube, 1, 1, 1, 5);

    // Ent 1, Octahedron
    ent = Game_GetFreeEntity();
    ent->transform.scale.x = 0.5;
    ent->transform.scale.y = 0.5;
    ent->transform.scale.z = 0.5;

    obj = Game_GetFreeRenderObj(ent->entId.index);
    RendObj_SetAsMesh(obj, &g_meshOctahedron, 1, 1, 1, 4);
}

void Game_BuildRenderList(RenderScene* scene)
{
    scene->numObjects = 0;
    int j = 0;
    for (u32 i = 0; i < R_MAX_RENDER_OBJECTS; ++i)
    {
        RendObj* obj = &g_game_rendObjects[i];
		if (obj->inUse == 1)
		{
			Ent* e = Game_GetEntityByIndex(obj->ownerIndex);
			RenderListItem* item = &scene->sceneItems[i];
			item->transform = e->transform;
			item->obj = *obj;
			scene->numObjects++;
		}
    }
}
