#pragma once

#include "server.cpp"
#include <math.h>

// Returns target if found
internal SimEntity* SVG_FindAndValidateTarget(
    SimScene* sim, SimEntity* ent)
{
    /*
    > if no target Id => get a target Id and retrieve Ent
    > if target Id => retrieve Ent
    > Validate Target Ent. 
    */
   SimEntity* target = NULL;
   if (ent->relationships.targetId.serial == SIM_ENT_NULL_SERIAL)
   {
       // Try to find a new target
       target = Sim_FindTargetForEnt(sim, ent);
       if (target == NULL) { return NULL; }
       ent->relationships.targetId = target->id;
       return target;
   }
   else
   {
       // Check current target and clear if invalidated
       target = Sim_GetEntityByIndex(sim, ent->relationships.targetId.slot);
       // check target is still okay
       if (Sim_IsEntInPlay(target) == NO)
       {
           ent->relationships.targetId = {};
           return NULL;
       }
       return target;
   }
   
}

internal void SVG_HandleEntityDeath(
    SimScene* sim, SimEntity* victim, SimEntity* attacker, i32 style)
{
	APP_LOG(128, "SV Remove ent %d\n", victim->id.serial);
    
    S2C_RemoveEntity cmd = {};
    Cmd_InitRemoveEntity(&cmd, g_ticks, 0, victim->id.serial);
    SVU_EnqueueCommandForAllUsers(&g_users, &cmd.header);

    SimEntity* parent = Sim_GetEntityBySerial(
        sim, victim->relationships.parentId.serial);
    if (parent != NULL)
    {
        parent->relationships.liveChildren--;
    }
    SVU_RemoveEntityLinkForAllUsers(&g_users, victim->id.serial);
	// Remove Ent AFTER command as sim may
	// clear entity details immediately
	Sim_RemoveEntity(sim, victim->id.serial);
}

internal void SVG_SpawnLineSegment(SimScene* sim, Vec3 origin, Vec3 dest)
{
    SimEntSpawnData def = {};
    def.factoryType = SIM_FACTORY_TYPE_LINE_TRACE;
    def.serial = Sim_ReserveEntitySerial(sim, 1);
    def.isLocal = 1;
    def.pos = origin;
    def.destination = dest;
    Sim_RestoreEntity(sim, &def);
}

#define SVG_DEFINE_ENT_UPDATE(entityTypeName) internal void \
    SVG_Update##entityTypeName##(SimScene* sim, SimEntity* ent, f32 deltaTime)

//////////////////////////////////////////////////////
// ENEMIES
//////////////////////////////////////////////////////
//internal void SVG_UpdateWanderer(SimScene* sim, SimEntity* ent, f32 deltaTime)
SVG_DEFINE_ENT_UPDATE(Wanderer)
{
    if (ent->timing.nextThink >= sim->tick)
    {
        ent->timing.lastThink = ent->timing.nextThink;
        
        ent->timing.nextThink += (i32)COM_STDRandomInRange(
            (f32)App_CalcTickInterval(1),
            (f32)App_CalcTickInterval(6)
        );
        f32 radians = COM_STDRandomInRange(0, 360) * DEG2RAD;
        ent->body.velocity.x = cosf(radians) * ent->body.speed;
        ent->body.velocity.z = sinf(radians) * ent->body.speed;
    }
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

SVG_DEFINE_ENT_UPDATE(Seeker)
{
    SimEntity* target = Sim_FindTargetForEnt(sim, ent);
    if (target)
    {
        Vec3 toTarget = 
        {
            target->body.t.pos.x - ent->body.t.pos.x,
            target->body.t.pos.y - ent->body.t.pos.y,
            target->body.t.pos.z - ent->body.t.pos.z,
        };
        Vec3_Normalise(&toTarget);
        Vec3 avoid = Sim_BuildAvoidVector(sim, ent);
        toTarget.x += avoid.x;
        toTarget.y += avoid.y;
        toTarget.z += avoid.z;
        Vec3_Normalise(&toTarget);
        ent->body.velocity =
        {
            toTarget.x * ent->body.speed,
            toTarget.y * ent->body.speed,
            toTarget.z * ent->body.speed,
        };
    }
    else
    {
        ent->body.velocity = { 0, 0, 0 };
    }
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

SVG_DEFINE_ENT_UPDATE(Bouncer)
{
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

SVG_DEFINE_ENT_UPDATE(Dart)
{
    Vec3 previousPos = ent->body.t.pos;
    Sim_SimpleMove(ent, deltaTime);
    if (!Sim_InBounds(ent, &sim->boundaryMin, &sim->boundaryMax))
    {
        ent->body.velocity.x *= -1;
        ent->body.velocity.y *= -1;
        ent->body.velocity.z *= -1;
        ent->body.t.pos = previousPos;
    }
}

//////////////////////////////////////////////////////
// Spawner
//////////////////////////////////////////////////////
SVG_DEFINE_ENT_UPDATE(Spawner)
{
    i32 spawnSpaces = ent->relationships.maxLiveChildren - 
        ent->relationships.liveChildren;
    if (spawnSpaces < ent->relationships.childSpawnCount)
    { return; }

    if (sim->tick >= ent->timing.nextThink)
    {
        ent->timing.lastThink = ent->timing.nextThink;
        ent->timing.nextThink += App_CalcTickInterval(2);
        // think
        // Spawn projectiles
        #if 1
        SimBulkSpawnEvent event = {};
        event.factoryType = ent->relationships.childFactoryType;
        event.base.firstSerial = Sim_ReserveEntitySerials(
            sim, 0, ent->relationships.childSpawnCount);
        event.base.pos = ent->body.t.pos;
        event.patternDef.numItems = ent->relationships.childSpawnCount;
        event.patternDef.patternId = SIM_PATTERN_RADIAL;
        event.patternDef.radius = 6.0f;
        event.base.seedIndex = COM_STDRandU8();
        event.base.forward = { 0, 0, 1 };
        // frame the event occurred on is recorded
        event.base.tick = sim->tick;
        event.base.sourceSerial = ent->id.serial;
        i32 flags;
        f32 priority;
		Sim_ExecuteBulkSpawn(
			sim, &event, 0, &flags, &priority);

        ent->relationships.liveChildren += 
            ent->relationships.childSpawnCount;

        // Replicate!
        S2C_BulkSpawn prj = {};
        Cmd_InitBulkSpawn(&prj, &event, g_ticks, 0);
		SVU_EnqueueCommandForAllUsers(&g_users, &prj.header);
        if (flags & SIM_ENT_FLAG_POSITION_SYNC)
        {
            SVU_AddBulkEntityLinksForAllUsers(
                &g_users, event.base.firstSerial, event.patternDef.numItems, priority);
        }
        #endif
        #if 0
        SimEnemySpawnEvent event {};
        event.enemyType = SIM_FACTORY_TYPE_WANDERER;
        event.base.firstSerial = Sim_ReserveEntitySerials(
            sim, 0, 8);
        event.base.pos = ent->body.t.pos;
        event.base.forward = ent->body.t.pos;
        #endif
    }
}

//////////////////////////////////////////////////////
// Spawner
//////////////////////////////////////////////////////
SVG_DEFINE_ENT_UPDATE(Spawn)
{
    if (ent->timing.nextThink >= sim->tick)
    {
        ent->flags &= ~SIM_ENT_FLAG_OUT_OF_PLAY;
        ent->tickType = ent->coreTickType;
    }
    else
    {
        ent->flags |= SIM_ENT_FLAG_OUT_OF_PLAY;
        f32 time = (f32)ent->timing.lastThink / (f32)ent->timing.nextThink;
    }
    
    // if (ent->thinkTick <= 0)
    // {
    //     ent->flags &= ~SIM_ENT_FLAG_OUT_OF_PLAY;
    //     ent->tickType = ent->coreTickType;
    // }
    // else
    // {
    //     ent->flags |= SIM_ENT_FLAG_OUT_OF_PLAY;
    //     ent->thinkTick -= deltaTime;
    // }
    
}

internal i32 SVG_StepProjectile(
    SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    Sim_SimpleMove(ent, deltaTime);

    // find victims
    Vec3 halfSize =
    {
        ent->body.t.scale.x / 2,
        ent->body.t.scale.y / 2,
        ent->body.t.scale.z / 2
    };
    Vec3 p = ent->body.t.pos;
    Vec3 min;
    min.x = p.x - halfSize.x;
    min.y = p.y - halfSize.y;
    min.z = p.z - halfSize.z;
    Vec3 max;
    max.x = p.x + halfSize.x;
    max.y = p.y + halfSize.y;
    max.z = p.z + halfSize.z;

    SimEntity* ents[16];
    i32 overlaps = Sim_FindByAABB(
        sim, min, max, ent->id.serial, ents, 16);
    
    for (i32 i = 0; i < overlaps; ++i)
    {
        // TODO: For now just hit first valid entity in list
        SimEntity* victim = ents[i];
        if (Sim_IsEntTargetable(victim) == NO) { continue; }
		COM_ASSERT(victim->id.serial, "SV overlap victim serial is 0")
        SVG_HandleEntityDeath(sim, victim, ent, 0);
        ent->timing.nextThink = sim->tick;
        break;
    }
    
	if (sim->tick >= ent->timing.nextThink)
	{
        SVG_HandleEntityDeath(sim, ent, NULL, 0);
        return 0;
	}
    return 1;
}

SVG_DEFINE_ENT_UPDATE(Projectile)
{
    while(ent->timing.fastForwardTicks > 0)
    {
        ent->timing.fastForwardTicks--;
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
    const i32 verbose = 0;

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
        if (verbose)
        {
            printf(
                "Prj ping %.3f, ticksEllapsed - %d ticks (diff %d)\n",
                u->ping, ticksEllapsed, diff);
        }
        
    }

    // Declare when the event took place:
    i32 eventTick = g_ticks - fastForwardTicks;
    if (verbose)
    {
        printf("SV CurTick %d eventTick %d fastforward %d\n",
            g_ticks, eventTick, fastForwardTicks);
    }

    i32 numProjectiles = 3;
    
    SimBulkSpawnEvent event = {};
    event.factoryType = SIM_FACTORY_TYPE_PROJ_PLAYER;
    event.base.firstSerial = Sim_ReserveEntitySerials(
        sim, 0, numProjectiles);
    event.patternDef.patternId = SIM_PATTERN_SPREAD;
    event.patternDef.numItems = numProjectiles;
    event.patternDef.radius = 0;
    event.patternDef.arc = 0.25f;
    event.base.pos = ent->body.t.pos;
    event.base.forward = *dir;
    event.base.tick = g_ticks;
    event.base.seedIndex = COM_STDRandU8();
    i32 flags;
    f32 priority;
    Sim_ExecuteBulkSpawn(sim, &event, fastForwardTicks, &flags, &priority);

    // Replicate
    S2C_BulkSpawn prj = {};
    Cmd_Prepare(&prj.header, eventTick, 0);
    prj.def = event;
    prj.header.type = CMD_TYPE_S2C_SPAWN_PROJECTILE;
    prj.header.size = sizeof(prj);
    SVU_EnqueueCommandForAllUsers(&g_users, &prj.header);

    if (flags & SIM_ENT_FLAG_POSITION_SYNC)
    {
        SVU_AddBulkEntityLinksForAllUsers(
            &g_users,
            event.base.firstSerial,
            event.patternDef.numItems, priority);
    }


    /* Debug - Create a line trace */
    Vec3 origin = ent->body.t.pos;
    Vec3 dest {};
    dest.x = (origin.x + (dir->x * 10));
    dest.y = (origin.y + (dir->y * 10));
    dest.z = (origin.z + (dir->z * 10));
    SVG_SpawnLineSegment(sim, origin, dest);
}

SVG_DEFINE_ENT_UPDATE(LineTrace)
{
    if (sim->tick >= ent->timing.nextThink)
    {
        Sim_RemoveEntity(sim, ent->id.serial);
    }
}

SVG_DEFINE_ENT_UPDATE(Actor)
{
	Vec3 move = {};
	f32 speed = ent->body.speed;//5.0f;
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
	ent->body.previousPos = ent->body.t.pos;
	ent->body.t.pos.x += move.x;
	ent->body.t.pos.y += move.y;
	ent->body.t.pos.z += move.z;

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
        case SIM_TICK_TYPE_DART:
        { SVG_UpdateDart(sim, ent, deltaTime); } break;
		case SIM_TICK_TYPE_ACTOR:
        { SVG_UpdateActor(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_SPAWNER:
        { SVG_UpdateSpawner(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_LINE_TRACE:
        { SVG_UpdateLineTrace(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_SPAWN:
        //{ SVG_UpdateSpawn(sim, ent, deltaTime); } break;
        { Sim_TickSpawner(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_WORLD: { } break;
        case SIM_TICK_TYPE_NONE: { } break;
        //case SIM_TICK_TYPE_NONE: { } break;
        default: { COM_ASSERT(0, "Unknown Ent Tick Type"); } break;
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
    sim->tick++;
}
