#pragma once

#include "client.h"

#define CLG_DEFINE_ENT_UPDATE(entityTypeName) internal void \
    CLG_Update##entityTypeName##(SimScene* sim, SimEntity* ent, f32 deltaTime)

CLG_DEFINE_ENT_UPDATE(Wanderer)
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

CLG_DEFINE_ENT_UPDATE(Projectile)
{
	i32 numSteps = 1 + ent->fastForwardTicks;
    ent->fastForwardTicks = 0;
    APP_LOG(64, "CL Prj %d steps: %d\n", ent->id.serial, numSteps);
	if (numSteps < 1) { numSteps = 1; }
	while (numSteps)
	{
		numSteps--;
		Sim_SimpleMove(ent, deltaTime);
		ent->lifeTime -= deltaTime;
		if (ent->lifeTime < 0)
		{
			Sim_RemoveEntity(sim, ent->id.serial);
			return;
		}
	}
}

internal void CLG_TickEntity(SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    switch (ent->entType)
    {
        case SIM_ENT_TYPE_WANDERER: { CLG_UpdateWanderer(sim, ent, deltaTime); } break;
        case SIM_ENT_TYPE_PROJECTILE: { CLG_UpdateProjectile(sim, ent, deltaTime); } break;
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
