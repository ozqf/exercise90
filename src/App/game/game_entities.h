#pragma once

#include "game.h"

inline void Ent_Reset(Ent* ent)
{
    ent->factoryType = 0;
    ent->componentBits = 0;
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
            if (g_verbose)
            {
                printf("SV Reserving Ent %d/%d\n", e->entId.iteration, e->entId.index);
            }
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
 * Clear ids and set all iterations back to 1
 * Iteration 0 == an unused (NULL) entity
 * Must be run before starting!
 */
void Ent_ResetEntityIds(EntList* ents)
{
	for (u16 i = 0; i < ents->max; ++i)
	{
		ents->items[i].inUse = ENTITY_STATUS_FREE;
		ents->items[i].entId.iteration = 1;
        ents->items[i].entId.index = i;
        ents->items[i].componentBits = 0 | EC_BIT0_ENTITY;
	}
}

void Ent_ClearComponents(GameState* gs, Ent* ent)
{
    EC_RemoveTransform(gs, ent);
    EC_RemoveSingleRendObj(gs, ent);
    EC_RemoveCollider(gs, ent);
    EC_RemoveAIController(gs, ent);
    EC_RemoveActorMotor(gs, ent);
    EC_RemoveProjectile(gs, ent);
    EC_RemoveHealth(gs, ent);
    EC_RemoveMultiRendObj(gs, ent);
}

inline void Ent_MarkForFree(Ent* ent)
{
    ent->inUse = ENTITY_STATUS_DEAD;
}

inline void Ent_WriteRemoveCmd(Ent* ent, f32* gfxNormalVec3, u8 verbose)
{
    Ent_MarkForFree(ent);
    Cmd_RemoveEntity cmd = {};
    cmd.entId = ent->entId;
    cmd.gfxNormal = COM_PackVec3NormalToI32(gfxNormalVec3);
    if (verbose)
    {
        // printf("Packed GFX Normal %.2f, %.2f, %.2f to %d\n",
        //     gfxNormalVec3[0],
        //     gfxNormalVec3[1],
        //     gfxNormalVec3[2],
        //     cmd.gfxNormal
        // );
        printf("GAME Delete ent %d/%d\n",
		    cmd.entId.iteration,
		    cmd.entId.index
        );
	}
    APP_WRITE_CMD(0, CMD_TYPE_REMOVE_ENT, 0, cmd);
    //App_WriteGameCmd((u8*)&cmd, CMD_TYPE_REMOVE_ENT, sizeof(Cmd_RemoveEntity));
}

inline void Ent_Free(GameState* gs, Ent* ent)
{
    Ent_ClearComponents(gs, ent);
	ent->entId.iteration += 1;
    // avoiding using iteration 0 if iteration wraps
    if (ent->entId.iteration == 0)
    {
        ent->entId.iteration = 1;
    }
    ent->inUse = ENTITY_STATUS_FREE;
    ent->componentBits = 0 | EC_BIT0_ENTITY;
}

/**
 * Retrieve a free or reserved entity and mark it as ready for use
 * Do NOT call for an entity that is in use or it will error
*/
inline Ent* Ent_GetAndAssign(EntList* ents, EntId* queryId)
{
	Ent* ent = &ents->items[queryId->index];
	if (ent->inUse != ENTITY_STATUS_IN_USE)
	{
		// if a client machine, may not know iteration
		ent->entId.iteration = queryId->iteration;
        ent->inUse = ENTITY_STATUS_IN_USE;
		return ent;
	}
	else
	{
		printf("!ERROR Attempting to  assign to an in-use entity: %d/%d\n", ent->entId.iteration, ent->entId.index);
		ILLEGAL_CODE_PATH
		return NULL;
	}
}

// Will NOT return entities that are free or merely reserved. Only ones that are active
inline Ent* Ent_GetEntityById(EntList* ents, EntId* id)
{
    Assert(id->index < ents->max)
    Ent* ent = &ents->items[id->index];
    if (ent->inUse == ENTITY_STATUS_IN_USE && ent->entId.iteration == id->iteration)
    {
        return ent;
    }
	else
	{
		return NULL;
	}
}

inline Ent* Ent_GetEntityByIdValue(EntList* ents, u32 entIdValue)
{
    EntId id {};
    id.value = entIdValue;
    return Ent_GetEntityById(ents, &id);
}

inline Ent* Ent_GetEntityById(GameState* gs, EntId* id)
{
    return Ent_GetEntityById(&gs->entList, id);
}

inline Ent* Ent_GetEntityToRemoveById(EntList* ents, EntId* id)
{
    Assert(id->index < ents->max)
    Ent* ent = &ents->items[id->index];
    if (ent->entId.iteration != id->iteration)
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

//////////////////////////////////////////////////////
// Entity Component Utilities
//////////////////////////////////////////////////////

EC_Collider* EC_ColliderGetByShapeId(EC_ColliderList* list, i32 shapeId);
