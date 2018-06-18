#pragma once

#include "game.h"

inline void Ent_Reset(Ent* ent)
{
    Transform_SetToIdentity(&ent->transform);
    ent->factoryType = 0;
    ent->componentFlags = 0;
    ent->tag = 0;
}

#define ENTITY_STATUS_FREE 0
#define ENTITY_STATUS_RESERVED 1
#define ENTITY_STATUS_IN_USE 2
#define ENTITY_STATUS_DEAD 3

/**
 * Find a free Entity slot and set it to 'reserved'.
 * Actual config will occur later.
 * NOTE: This function is for the server ONLY.
 */
EntId Ent_ReserveFreeEntity(EntList* ents)
{
    Ent* e = NULL;
    for (u16 i = 0; i < ents->max; ++i)
    {
        e = &ents->items[i];
        if (e->inUse == ENTITY_STATUS_FREE)
        {
            e->inUse = ENTITY_STATUS_RESERVED;
            printf("SV Reserving Ent %d/%d\n", e->entId.iteration, e->entId.index);
            Ent_Reset(e);
            // Iteration remains same. Is iterated on free, not on reserve
			return e->entId;
        }
    }
    // Oops, no free entities
    ILLEGAL_CODE_PATH
    return { 0, 0 };
}

/**
 * Clear ids and set all iterations back to 0
 * Must be run before starting!
 */
void Ent_ResetEntityIds(EntList* ents)
{
	for (u16 i = 0; i < ents->max; ++i)
	{
		ents->items[i].entId.iteration = 0;
        ents->items[i].entId.index = i;
	}
}

/**
 * Find a free slot in the entity list
 */
#if 0
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
            //e->entId.iteration = iteration + 1;
            e->entId.index = i;
			return e;
        }
    }
    
    Assert(e != NULL);
    return NULL;
}
#endif

void Ent_ClearComponents(GameState* gs, Ent* ent)
{
    EC_RemoveAIController(gs, ent);
    EC_RemoveCollider(gs, ent);
    EC_RemoveRenderer(gs, ent);
    EC_RemoveActorMotor(gs, ent);
    EC_RemoveProjectile(gs, ent);
}

inline void Ent_MarkForFree(Ent* ent)
{
    ent->inUse = ENTITY_STATUS_DEAD;
}

inline void Ent_Free(GameState* gs, Ent* ent)
{
    Ent_ClearComponents(gs, ent);
	ent->entId.iteration += 1;
    ent->inUse = ENTITY_STATUS_FREE;
    ent->componentFlags = 0;
}

inline Ent* Ent_GetAndAssign(EntList* ents, EntId* entId)
{
	for (u32 i = 0; i < ents->max; ++i)
	{
		Ent* result = &ents->items[i];
		if (EntId_Equals(&result->entId, entId))
		{
			// If server, entity should have been Reserved.
            // If client, entity should be free.
            if (result->inUse != ENTITY_STATUS_IN_USE)
            {
                result->inUse = ENTITY_STATUS_IN_USE;
			    result->entId = *entId;
            }
            else
            {
                printf("GAME: Ent is marked in use! Aborting\n");
                ILLEGAL_CODE_PATH
            }
			return &ents->items[i];
		}
	}
	printf("GAME: No entity found matching Id %d/%d! Aborting\n", entId->iteration, entId->index);
    ILLEGAL_CODE_PATH
	return NULL;
}

inline Ent* Ent_GetEntity(EntList* ents, EntId* entId)
{
	for (u32 i = 0; i < ents->max; ++i)
	{
		if (EntId_Equals(&ents->items[i].entId, entId))
		{
            Assert(ents->items[i].inUse == ENTITY_STATUS_IN_USE);
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

inline Ent* Ent_GetEntityById(EntList* ents, EntId* id)
{
    Assert(id->index < ents->max)
    Ent* ent = &ents->items[id->index];
    if (ent->inUse == 0 || ent->entId.iteration != id->iteration)
    {
        return NULL;
    }
    return ent;
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
