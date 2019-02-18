#pragma once

#include "server.h"
#include <math.h>

#define SVG_DEFINE_ENT_UPDATE(entityTypeName) internal void \
    SVG_Update##entityTypeName##(SimScene* sim, SimEntity* ent, f32 deltaTime)

//internal void SVG_UpdateWanderer(SimScene* sim, SimEntity* ent, f32 deltaTime)
SVG_DEFINE_ENT_UPDATE(Wanderer)
{
    if (ent->thinkTick <= 0)
    {
        ent->thinkTick += ent->thinkTime;
        // random range of seconds between thinks
        ent->thinkTime = COM_STDRandomInRange(1, 4);
        // set a random movement vector
        f32 radians = COM_STDRandomInRange(0, 360) * DEG2RAD;
        ent->velocity.x = cosf(radians) * 3;
        ent->velocity.z = sinf(radians) * 3;
        printf("SV Wanderer move: %.3f, %.3f\n", ent->velocity.x, ent->velocity.z);
    }
    else
    {
        ent->thinkTick -= deltaTime;
    }
    
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

SVG_DEFINE_ENT_UPDATE(Turret)
{
    if (ent->thinkTick <= 0.0f)
    {
        ent->thinkTick += ent->thinkTime;
        // think
        SimProjectileSpawnDef def = {};
        def.projType = SIM_PROJ_TYPE_TEST;
        def.firstSerial = Sim_ReserveEntitySerialGroup(sim, 0, SIM_PROJ_TYPE_TEST);
        if (def.firstSerial == -1)
        {
            APP_PRINT(64, "SVG Turret failed to acquire ent serials\n");
            return;
        }
        def.pos = ent->t.pos;
        def.seedIndex = 0;
        def.forward = { 1, 0, 0 };
        // frame the event occurred is recorded
        def.tick = g_ticks;
		Sim_ExecuteProjectileSpawn(
			sim, &def, 0
		);
        S2C_SpawnProjectile prj = {};
        Cmd_Prepare(&prj.header, g_ticks, 0);
        prj.def = def;
        prj.header.type = CMD_TYPE_S2C_SPAWN_PROJECTILE;
        prj.header.size = sizeof(prj);
		SV_EnqueueCommandForAllUsers(&g_users, &prj.header);
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
