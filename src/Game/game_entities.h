#pragma once

#include "game.h"


/**
 * Find a free slot in the entity list
 */
Ent* Ent_GetFreeEntity()
{
    Ent* e = NULL;
    for (u16 i = 0; i < GAME_MAX_ENTITIES; ++i)
    {
        e = &g_entities[i];
        if (e->inUse == 0)
        {
            // new iteration of this entity
            u16 iteration = e->entId.iteration;
            *e = {};
            Transform_SetToIdentity(&e->transform);
            e->inUse = 1;
            e->entId.iteration = iteration + 1;
            e->entId.index = i;
			return e;
        }
    }
    
    Assert(e != NULL);
    return NULL;
}

inline void Ent_Free(Ent* ent)
{
    ent->inUse = 0;
    ent->componentFlags = 0;
}

inline Ent* Ent_GetEntityByIndex(u16 index)
{
    Assert(index < GAME_MAX_ENTITIES)
    return &g_entities[index];
}

void Ent_InitEntityList()
{
    for (u16 i = 0; i < GAME_MAX_ENTITIES; ++i)
    {
        g_entities[i].entId.iteration = 0;
        g_entities[i].entId.index = i;
    }
}
