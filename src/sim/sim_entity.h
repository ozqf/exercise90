#pragma once

#include "sim.h"

////////////////////////////////////////////////////////////////////
// Entity assignment
////////////////////////////////////////////////////////////////////

i32 Sim_InBounds(SimEntity* ent, Vec3* min, Vec3* max)
{
    Vec3* p = &ent->t.pos;
    if (p->x < min->x) { return 0; }
    if (p->x > max->x) { return 0; }
    if (p->y < min->y) { return 0; }
    if (p->y > max->y) { return 0; }
    if (p->z < min->z) { return 0; }
    if (p->z > max->z) { return 0; }
    return 1;
}

void Sim_BoundaryBounce(SimEntity* ent, Vec3* min, Vec3* max)
{
    Vec3* p = &ent->t.pos;
    if (p->x < min->x) { p->x = min->x; ent->velocity.x = -ent->velocity.x; }
    if (p->x > max->x) { p->x = max->x; ent->velocity.x = -ent->velocity.x; }

    if (p->y < min->y) { p->y = min->y; ent->velocity.y = -ent->velocity.y; }
    if (p->y > max->y) { p->y = max->y; ent->velocity.y = -ent->velocity.y; }

    if (p->z < min->z) { p->z = min->z; ent->velocity.z = -ent->velocity.z; }
    if (p->z > max->z) { p->z = max->z; ent->velocity.z = -ent->velocity.z; }
}
#if 0
internal void Sim_UpdateWanderer(SimScene* scene, SimEntity* ent, f32 deltaTime)
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
    
    Sim_BoundaryBounce(ent, &scene->boundaryMin, &scene->boundaryMax);
}

internal void Sim_UpdateTurret(SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    if (ent->thinkTick <= 0.0f)
    {
        ent->thinkTick += ent->thinkTime;
        // think

        SimCmdProjectileSpawn cmd = {};
        Sim_PrepareCommand(sim, &cmd.header);
        cmd.def.projType = SIM_PROJ_TYPE_TEST;
        cmd.def.pos = ent->t.pos;
        cmd.def.seedIndex = 0;
        cmd.def.forward = { 1, 0, 0 };
        Sim_EnqueueCommand(sim, (u8*)&cmd);
    }
    else
    {
        ent->thinkTick -= deltaTime;
    }
}

internal void Sim_UpdateEntity(SimScene* scene, SimEntity* ent, f32 deltaTime)
{
    switch (ent->entType)
    {
        case SIM_ENT_TYPE_WANDERER: { Sim_UpdateWanderer(scene, ent, deltaTime); } break;
        case SIM_ENT_TYPE_WORLD: { } break;
        case SIM_ENT_TYPE_NONE: { } break;
        default: { ILLEGAL_CODE_PATH } break;
    }
}
#endif
