#pragma once

#include "sim.h"

#define SIM_MAX_ENTITIES 2048

internal i32 g_nextEntityId = 1;
internal Sim_Entity g_entities[SIM_MAX_ENTITIES];

internal Sim_Entity* Sim_GetFreeEntity()
{
    for (i32 i = 0; i < SIM_MAX_ENTITIES; ++i)
    {
        Sim_Entity* ent = &g_entities[i];
        if (ent->status == SIM_ENT_STATUS_FREE)
        {
            ent->status = SIM_ENT_STATUS_IN_USE;
            ent->id = g_nextEntityId++;
            return ent;
        }
    }
    ILLEGAL_CODE_PATH
    return NULL;
}

i32 Sim_AddEntity()
{
    Sim_Entity* ent = Sim_GetFreeEntity();

    f32 randX = (COM_STDRandf32() * 2) - 1;
    f32 randY = (COM_STDRandf32() * 2) - 1;
    ent->t.pos.x = 2 * randX;
    ent->t.pos.y = 2 * randY;
    printf("SIM Added entity at %.3f, %.3f\n", ent->t.pos.x, ent->t.pos.y);
    return ent->id;
}

void Sim_GetEntityList(Sim_Entity** ptr, i32* max)
{
    *ptr = g_entities;
    *max = SIM_MAX_ENTITIES;
}
