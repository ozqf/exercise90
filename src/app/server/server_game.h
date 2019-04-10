#pragma once

#include "server.h"
#include <math.h>

// Returns target if found
internal SimEntity* SVG_FindAndValidateTarget(SimScene* sim, SimEntity* ent)
{
    /*
    > if no target Id => get a target Id and retrieve Ent
    > if target Id => retrieve Ent
    > Validate Target Ent.
    */
   SimEntity* target = NULL;
}

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
    def.factoryType = SIM_FACTORY_TYPE_LINE_TRACE;
    def.serial = Sim_ReserveEntitySerial(sim, 1);
    def.isLocal = 1;
    def.pos = origin;
    def.destination = dest;
    Sim_RestoreEntity(sim, &def);
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
        ent->velocity.x = cosf(radians) * ent->speed;
        ent->velocity.z = sinf(radians) * ent->speed;
        //printf("SV Wanderer move: %.3f, %.3f\n", ent->velocity.x, ent->velocity.z);
    }
    else
    {
        ent->thinkTick -= deltaTime;
    }
    
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

SVG_DEFINE_ENT_UPDATE(Seeker)
{
    SimEntity* target = Sim_FindTargetForEnt(sim, ent);
    if (target)
    {
        Sim_SimpleMove(ent, deltaTime);
        Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
    }
    else
    {
        ent->velocity = { 0, 0, 0 };
    }
}

SVG_DEFINE_ENT_UPDATE(Bouncer)
{
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

SVG_DEFINE_ENT_UPDATE(Spawner)
{
    if (ent->thinkTick <= 0.0f)
    {
        ent->thinkTick += ent->thinkTime;
        // think
        // Spawn projectiles
        #if 1
        SimProjectileSpawnEvent event = {};
        //vent.factoryType = SIM_FACTORY_TYPE_PROJ_PLAYER;
        //event.factoryType = SIM_FACTORY_TYPE_WANDERER;
        event.factoryType = ent->childFactoryType;
        event.base.firstSerial = Sim_ReserveEntitySerials(sim, 0, 4);
        event.base.pos = ent->t.pos;
        event.patternDef.numItems = 4;
        event.patternDef.patternId = SIM_PATTERN_SCATTER;
        event.patternDef.radius = 1.5f;
        event.base.seedIndex = COM_STDRandU8();
        event.base.forward = { 0, 0, 1 };
        // frame the event occurred on is recorded
        event.base.tick = g_ticks;
		Sim_ExecuteProjectileSpawn(
			sim, &event, 0
		);
        // Replicate!
        S2C_SpawnProjectiles prj = {};
        Cmd_InitProjectileSpawn(&prj, &event, g_ticks, 0);
		SV_EnqueueCommandForAllUsers(&g_users, &prj.header);
        #endif
        #if 0
        SimEnemySpawnEvent event {};
        event.enemyType = SIM_FACTORY_TYPE_WANDERER;
        event.base.firstSerial = Sim_ReserveEntitySerials(
            sim, 0, 8);
        event.base.pos = ent->t.pos;
        event.base.forward = ent->t.pos;
        #endif
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
		Assert(victim->id.serial)
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

    /*
	500ms round trip lag == (250ms / 16ms) or 15.625 ticks since player pressed button
		plus a few frames for player's jitter.
    Event occurred at client tick. Therefore server must fast-forward the projectile
        by (server tick) - (client tick) ticks.
	eg server tick 100, client event from tick 80:
	> Server must fast-forward 20 ticks.
	> All clients are informed the event occurred on tick 80 and will also fast-forward
	
	If Prj moves at 15 units a second, at 60fps, 0.25 per frame, 20 frames == 5 units
	
    */
    //printf("SVG Shoot %.3f, %.3f\n", dir->x, dir->z);

    i32 fastForwardTicks = 0;
    User* u = User_FindByAvatarSerial(&g_users, ent->id.serial);
    if (u && g_lagCompensateProjectiles)
    {
        // calculate fast-forward ticks
        // TODO: Tune these and figure out which of these is best
        i32 ticksEllapsed = 0;
        i32 diff = 0;


        // 1: By estimating current lag
        // Works well until jitter is introduced, and then becomes
        // inaccurate (though test jitter is excessive...)
        f32 time = u->ping;// * 0.5f;
        ticksEllapsed = (i32)(time / App_GetSimFrameInterval());
        //ticksEllapsed += APP_DEFAULT_JITTER_TICKS;
        #if 0
        fastForwardTicks = ticksEllapsed;
        #endif
 
        // 2: By diffing from client's last stated frame
        // Works well but is this exploitable...? Trust client's tick value
        diff = g_ticks - u->latestServerTick;
        #if 1
        fastForwardTicks = diff;
        #endif
        printf(
            "Prj ping %.3f, ticksEllapsed - %d ticks (diff %d)\n",
            u->ping, ticksEllapsed, diff);
    }

    // Declare when the event took place:
    i32 eventTick = g_ticks - fastForwardTicks;
    printf("SV CurTick %d eventTick %d fastforward %d\n",
        g_ticks, eventTick, fastForwardTicks);

    SimProjectileSpawnEvent def = {};
    def.factoryType = SIM_FACTORY_TYPE_PROJ_PLAYER;
    def.base.firstSerial = Sim_ReserveEntitySerial(sim, 0);
    def.base.pos = ent->t.pos;
    def.base.forward = *dir;
    def.base.tick = g_ticks;
    def.base.seedIndex = COM_STDRandU8();
    Sim_ExecuteProjectileSpawn(sim, &def, fastForwardTicks);

    // Replicate
    S2C_SpawnProjectiles prj = {};
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

internal void SVG_TickEntity(
    SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    switch (ent->tickType)
    {
		case SIM_TICK_TYPE_PROJECTILE:
        { SVG_UpdateProjectile(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_SEEKER:
        { SVG_UpdateSeeker(sim, ent, deltaTime); } break;
		case SIM_TICK_TYPE_WANDERER:
        { SVG_UpdateWanderer(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_BOUNCER:
        { SVG_UpdateBouncer(sim, ent, deltaTime); } break;
		case SIM_TICK_TYPE_ACTOR:
        { SVG_UpdateActor(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_TURRET:
        { SVG_UpdateSpawner(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_LINE_TRACE:
        { SVG_UpdateLineTrace(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_WORLD: { } break;
        //case SIM_TICK_TYPE_NONE: { } break;
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
