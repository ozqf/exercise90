#pragma once

#include "game.h"


/**
 * Find a free slot in the entity list
 */
Ent* Ent_GetFreeEntity(EntList* ents)
{
    Ent* e = NULL;
    for (u16 i = 0; i < ents->max; ++i)
    {
        e = &ents->items[i];
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

void Ent_ClearComponents(GameState* gs, Ent* ent)
{
    EC_RemoveAIController(ent, gs);
    EC_RemoveCollider(ent, gs);
    EC_RemoveRenderer(ent, gs);
    EC_RemoveActorMotor(ent, gs);
    EC_RemoveProjectile(ent, gs);
}

inline void Ent_Free(GameState* gs, Ent* ent)
{
    Ent_ClearComponents(gs, ent);
    ent->inUse = 0;
    ent->componentFlags = 0;
}

inline Ent* Ent_GetEntity(EntList* ents, EntId* entId)
{
	for (u32 i = 0; i < ents->max; ++i)
	{
		if (EntId_Equals(&ents->items[i].entId, entId))
		{
			return &ents->items[i];
		}
	}
	return NULL;
}

inline Ent* Ent_GetEntityByIndex(EntList* ents, u16 index)
{
    Assert(index < ents->max)
    return &ents->items[index];
}

inline Ent* Ent_GetEntityByTag(EntList* ents, i32 tag)
{
    if (tag == 0) { return NULL; }
    for (u32 i = 0; i < ents->max; ++i)
    {
        if (ents->items[i].tag == tag) { return &ents->items[i]; }
    }
    return NULL;
}

// void Ent_InitEntityList()
// {
//     for (u16 i = 0; i < GAME_MAX_ENTITIES; ++i)
//     {
//         g_entities[i].entId.iteration = 0;
//         g_entities[i].entId.index = i;
//     }
// }
