#pragma once

#include "server.h"

#define SVG_DEFINE_ENT_UPDATE(entityTypeName) internal void \
    SVG_Update##entityTypeName##(SimScene* sim, SimEntity* ent, f32 deltaTime)

//internal void SVG_UpdateWanderer(SimScene* sim, SimEntity* ent, f32 deltaTime)
SVG_DEFINE_ENT_UPDATE(Wanderer)
{
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

SVG_DEFINE_ENT_UPDATE(Turret)
{
    if (ent->thinkTick <= 0.0f)
    {
        ent->thinkTick += ent->thinkTime;
        // think
        printf("SVG Turret think\n");
        SimCmdProjectileSpawn cmd = {};
        Sim_PrepareCommand(sim, &cmd.header);
        cmd.def.projType = SIM_PROJ_TYPE_TEST;
        cmd.def.pos = ent->t.pos;
        cmd.def.seedIndex = 0;
        cmd.def.forward = { 1, 0, 0 };
		Sim_ExecuteProjectileSpawn(
			sim, 0, &cmd.def
		);
        //Sim_EnqueueCommand(sim, (u8*)&cmd);
    }
    else
    {
        ent->thinkTick -= deltaTime;
    }
}

SVG_DEFINE_ENT_UPDATE(Projectile)
{
	Sim_SimpleMove(ent, deltaTime);
	ent->lifeTime -= deltaTime;
	if (ent->lifeTime < 0)
	{
		Sim_RemoveEntity(sim, ent->id.serial);
	}
}

internal void SVG_TickEntity(SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    switch (ent->entType)
    {
        case SIM_ENT_TYPE_WANDERER: { SVG_UpdateWanderer(sim, ent, deltaTime); } break;
		case SIM_ENT_TYPE_PROJECTILE: { SVG_UpdateProjectile(sim, ent, deltaTime); } break;
        case SIM_ENT_TYPE_TURRET: { SVG_UpdateTurret(sim, ent, deltaTime); } break;
        case SIM_ENT_TYPE_WORLD: { } break;
        case SIM_ENT_TYPE_NONE: { } break;
        default: { ILLEGAL_CODE_PATH } break;
    }
}

internal void SVG_TickSim(SimScene* sim, f32 deltaTime)
{
    for (i32 i = 0; i < g_sim.maxEnts; ++i)
    {
        SimEntity* ent = &g_sim.ents[i];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }

        SVG_TickEntity(sim, ent, deltaTime);
    }
}
