#pragma once

#include "server.h"
#include <math.h>

internal void SVG_HandleEntityDeath(
    SimScene* sim, SimEntity* victim, SimEntity* attacker, i32 style)
{
	APP_LOG(128, "SV Remove ent %d\n", victim->id.serial);
    
    S2C_RemoveEntity cmd = {};
    Cmd_InitRemoveEntity(&cmd, g_ticks, 0, victim->id.serial);
    SV_EnqueueCommandForAllUsers(&g_users, &cmd.header);

	// Remove Ent AFTER command as sim may
	// clear entity details immediately
	Sim_RemoveEntity(sim, victim->id.serial);
}

internal void SVG_SpawnLineSegment(SimScene* sim, Vec3 origin, Vec3 dest)
{
    SimEntityDef def = {};
    def.entType = SIM_ENT_TYPE_LINE_TRACE;
    def.serial = Sim_ReserveEntitySerial(sim, 1);
    def.isLocal = 1;
    def.pos[0] = origin.x;
    def.pos[1] = origin.y;
    def.pos[2] = origin.z;
    def.destination = dest;
    i32 serial = Sim_AddEntity(sim, &def);
}

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
        //printf("SV Wanderer move: %.3f, %.3f\n", ent->velocity.x, ent->velocity.z);
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

internal i32 SVG_StepProjectile(
    SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    Sim_SimpleMove(ent, deltaTime);

    // find victims
    f32 width = 0.5f;
    Vec3 p = ent->t.pos;
    Vec3 min;
    min.x = p.x - width;
    min.y = p.y - width;
    min.z = p.z - width;
    Vec3 max;
    max.x = p.x + width;
    max.y = p.y + width;
    max.z = p.z + width;

    SimEntity* ents[16];
    i32 overlaps = Sim_FindByAABB(
        sim, min, max, ent->id.serial, ents, 16);
    
    for (i32 i = 0; i < overlaps; ++i)
    {
        SimEntity* victim = ents[i];
        SVG_HandleEntityDeath(sim, victim, ent, 0);
        ent->lifeTime = 0;
    }

	ent->lifeTime -= deltaTime;
	if (ent->lifeTime < 0)
	{
        SVG_HandleEntityDeath(sim, ent, NULL, 0);
        return 0;
	}
    return 1;
}

SVG_DEFINE_ENT_UPDATE(Projectile)
{
    while(ent->fastForwardTicks > 0)
    {
        ent->fastForwardTicks--;
        if (!SVG_StepProjectile(sim, ent, deltaTime))
        {
            return;
        }
    }
	SVG_StepProjectile(sim, ent, deltaTime);
}

internal void SVG_FireActorAttack(SimScene* sim, SimEntity* ent, Vec3* dir)
{
    /*
    > identify if player...
        ...If not just spawn with no faff, otherwise:
    > Calculate the frame command came from - how?
         eg if server is on 12203 and CL on 11994:
         Client is running 5 ticks of time + 4 ticks of jitter
            behind server
         > Client has simulated forward 9 frames to match it's server
            position at this moment
    > Retrieve entity position at that point
    > Spawn projectiles and enqueue replication command
    > fast forward projectiles to present
    */
    //printf("SVG Shoot %.3f, %.3f\n", dir->x, dir->z);

    i32 fastForwardTicks = 0;
    User* u = User_FindByAvatarSerial(&g_users, ent->id.serial);
    if (u)
    {
        // calculate fast-forward ticks
        f32 time = u->ping;// * 0.5f;
        fastForwardTicks = (i32)(time / App_GetSimFrameInterval());
        fastForwardTicks += APP_DEFAULT_JITTER_TICKS;
        printf("Prj fastforward - %d ticks\n", fastForwardTicks);
    }
    u32 eventTick = g_ticks - fastForwardTicks;

    SimProjectileSpawnDef def = {};
    def.projType = SIM_PROJ_TYPE_NONE;
    def.firstSerial = Sim_ReserveEntitySerial(sim, 0);
    def.pos = ent->t.pos;
    def.seedIndex = 0;
    def.forward = *dir;
    def.tick = g_ticks;
    Sim_ExecuteProjectileSpawn(sim, &def, fastForwardTicks / 2);

    S2C_SpawnProjectile prj = {};
    Cmd_Prepare(&prj.header, eventTick, 0);
    prj.def = def;
    prj.header.type = CMD_TYPE_S2C_SPAWN_PROJECTILE;
    prj.header.size = sizeof(prj);
    SV_EnqueueCommandForAllUsers(&g_users, &prj.header);

    /* Debug - Create a line trace */
    Vec3 origin = ent->t.pos;
    Vec3 dest {};
    dest.x = (origin.x + (dir->x * 10));
    dest.y = (origin.y + (dir->y * 10));
    dest.z = (origin.z + (dir->z * 10));
    SVG_SpawnLineSegment(sim, origin, dest);
}

SVG_DEFINE_ENT_UPDATE(LineTrace)
{
    ent->lifeTime -= deltaTime;
    if (ent->lifeTime <= 0)
    {
        Sim_RemoveEntity(sim, ent->id.serial);
    }
}

SVG_DEFINE_ENT_UPDATE(Actor)
{
	Vec3 move = {};
	f32 speed = 5.0f;
	if (ent->input.buttons & ACTOR_INPUT_MOVE_FORWARD)
	{
		move.z -= speed * deltaTime;
	}
	if (ent->input.buttons & ACTOR_INPUT_MOVE_BACKWARD)
	{
		move.z += speed * deltaTime;
	}
	if (ent->input.buttons & ACTOR_INPUT_MOVE_LEFT)
	{
		move.x -= speed * deltaTime;
	}
	if (ent->input.buttons & ACTOR_INPUT_MOVE_RIGHT)
	{
		move.x += speed * deltaTime;
	}
	ent->previousPos = ent->t.pos;
	ent->t.pos.x += move.x;
	ent->t.pos.y += move.y;
	ent->t.pos.z += move.z;

    if (ent->attackTick <= 0)
    {
        Vec3 shoot {};
        if (ent->input.buttons & ACTOR_INPUT_SHOOT_LEFT)
        {
            shoot.x -= 1;
        }
        if (ent->input.buttons & ACTOR_INPUT_SHOOT_RIGHT)
        {
            shoot.x += 1;
        }
        if (ent->input.buttons & ACTOR_INPUT_SHOOT_UP)
        {
            shoot.z -= 1;
        }
        if (ent->input.buttons & ACTOR_INPUT_SHOOT_DOWN)
        {
            shoot.z += 1;
        }
        if (shoot.x != 0 || shoot.z != 0)
        {
            ent->attackTick = ent->attackTime;
            Vec3_Normalise(&shoot);
            SVG_FireActorAttack(sim, ent, &shoot);
        }
    }
    else
    {
        ent->attackTick -= deltaTime;
    }
}

internal void SVG_TickEntity(SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    switch (ent->entType)
    {
		case SIM_ENT_TYPE_PROJECTILE: { SVG_UpdateProjectile(sim, ent, deltaTime); } break;
		case SIM_ENT_TYPE_WANDERER: { SVG_UpdateWanderer(sim, ent, deltaTime); } break;
		case SIM_ENT_TYPE_ACTOR: { SVG_UpdateActor(sim, ent, deltaTime); } break;
        case SIM_ENT_TYPE_TURRET: { SVG_UpdateTurret(sim, ent, deltaTime); } break;
        case SIM_ENT_TYPE_LINE_TRACE: { SVG_UpdateLineTrace(sim, ent, deltaTime); } break;
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
