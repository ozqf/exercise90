#pragma once

#include "client.h"

internal void CLG_UpdateWanderer(SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    Vec3* pos = &ent->t.pos;
    ent->previousPos.x = pos->x;
    ent->previousPos.y = pos->y;
    ent->previousPos.z = pos->z;
    Vec3 move =
    {
        ent->velocity.x * deltaTime,
        ent->velocity.y * deltaTime,
        ent->velocity.z * deltaTime
    };
    
    ent->t.pos.x += move.x;
    ent->t.pos.y += move.y;
    ent->t.pos.z += move.z;
    
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

internal void CLG_TickEntity(SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    switch (ent->entType)
    {
        case SIM_ENT_TYPE_WANDERER: { CLG_UpdateWanderer(sim, ent, deltaTime); } break;
        case SIM_ENT_TYPE_WORLD: { } break;
        case SIM_ENT_TYPE_NONE: { } break;
        default: { ILLEGAL_CODE_PATH } break;
    }
}

internal void CLG_TickGame(SimScene* sim, f32 deltaTime)
{
    for (i32 i = 0; i < g_sim.maxEnts; ++i)
    {
        SimEntity* ent = &g_sim.ents[i];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }

        CLG_TickEntity(sim, ent, deltaTime);
    }
}
