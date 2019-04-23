#pragma once

#include "client.h"

internal void CLG_SpawnLineSegment(SimScene* sim, Vec3 origin, Vec3 dest)
{
    SimEntSpawnData def = {};
    def.factoryType = SIM_FACTORY_TYPE_LINE_TRACE;
    def.serial = Sim_ReserveEntitySerial(sim, 1);
    def.isLocal = 1;
    def.pos = origin;
    def.destination = dest;
    Sim_RestoreEntity(sim, &def);
}

internal void CLG_HandleEntityDeath(SimScene* sim, i32 serial)
{
    SimEntity* ent = Sim_GetEntityBySerial(sim, serial);
    if (!ent) { return; }
    switch (ent->deathType)
    {
        case SIM_DEATH_GFX_EXPLOSION:
        {
            SimEntSpawnData def = {};
            def.factoryType = SIM_FACTORY_TYPE_EXPLOSION;
            def.birthTick = sim->tick;
            def.pos = ent->body.t.pos;
            def.serial = Sim_ReserveEntitySerial(sim, 1);
            Sim_RestoreEntity(sim, &def);
        } break;
    }
}

#define CLG_DEFINE_ENT_UPDATE(entityTypeName) internal void \
    CLG_Update##entityTypeName##(SimScene* sim, SimEntity* ent, f32 deltaTime)

CLG_DEFINE_ENT_UPDATE(Explosion)
{
    if (sim->tick >= ent->timing.nextThink)
    {
        Sim_RemoveEntity(sim, ent->id.serial);
    }
}

CLG_DEFINE_ENT_UPDATE(Projectile)
{
	i32 numSteps = 1 + ent->timing.fastForwardTicks;
    ent->timing.fastForwardTicks = 0;
    //APP_LOG(64, "CL Prj %d steps: %d\n", ent->id.serial, numSteps);
	if (numSteps < 1) { numSteps = 1; }
	while (numSteps)
	{
		numSteps--;
		Sim_SimpleMove(ent, deltaTime);
        
		if (sim->tick >= ent->timing.nextThink)
		{
			Sim_RemoveEntity(sim, ent->id.serial);
			return;
		}
	}
}

// Return 1 if the command was successfully executed.
internal i32 CLG_SyncEntity(SimScene* sim, S2C_EntitySync* cmd)
{
    i32 executed = 0;
    SimEntity* ent = Sim_GetEntityBySerial(&g_sim, cmd->networkId);
    if (!ent)
    {
        APP_PRINT(128, "CL No ent %d for sync\n", cmd->networkId);
        // Must return executed or this dead command will stay
        // in the buffer!
        executed = 1;
    }
    else
    {
        if (cmd->networkId == g_avatarSerial)
        {
            // Do NOT sync the client's avatar here.
            // There's a special command for that!
            return 1;
        }
        //APP_LOG(64, "CL Sync ent %d\n", cmd->networkId);
        ent->body.previousPos = ent->body.t.pos;
        ent->body.t.pos = cmd->pos;
        ent->priority = cmd->priority;
        ent->body.velocity = cmd->vel;
        ent->relationships.targetId.serial = cmd->targetId;
        executed = 1;
    }
    return executed;
}

internal void CLG_StepActor(
    SimScene* sim,
    SimEntity* ent,
    SimActorInput* input,
    f32 deltaTime)
{
    Vec3 move = {};
	f32 speed = ent->body.speed;//5.0f;
	if (input->buttons & ACTOR_INPUT_MOVE_FORWARD)
	{
		move.z -= speed * deltaTime;
	}
	if (input->buttons & ACTOR_INPUT_MOVE_BACKWARD)
	{
		move.z += speed * deltaTime;
	}
	if (input->buttons & ACTOR_INPUT_MOVE_LEFT)
	{
		move.x -= speed * deltaTime;
	}
	if (input->buttons & ACTOR_INPUT_MOVE_RIGHT)
	{
		move.x += speed * deltaTime;
	}
	ent->body.previousPos = ent->body.t.pos;
	ent->body.t.pos.x += move.x;
	ent->body.t.pos.y += move.y;
	ent->body.t.pos.z += move.z;

}

internal void CLG_SyncAvatar(SimScene* sim, S2C_InputResponse* cmd)
{
    const i32 verbose = 0;
    if (g_latestUserInputAck >= cmd->lastUserInputSequence)
    {
        // APP_PRINT(64, "CL Ignore response %d (current %d)\n",
        //     cmd->lastUserInputSequence,
        //     g_latestUserInputAck
        // );
        return;
    }
    SimEntity* ent = Sim_GetEntityBySerial(
        &g_sim, g_avatarSerial);
    if (!ent) { return; }
    
    g_latestUserInputAck = cmd->lastUserInputSequence;
    g_latestAvatarPos = cmd->latestAvatarPos;
    i32 framesSinceResponse = g_userInputSequence - cmd->lastUserInputSequence;
    
    if (verbose)
    {
        APP_LOG(128, "CL Replay %d frames (%d to %d)\n",
            framesSinceResponse,
            cmd->lastUserInputSequence,
            g_userInputSequence
        );
    }
    
    // this is the input sequence matching the response. Replay will
    // occur from this point.
    C2S_Input* sourceInput = CL_RecallSentInputCommand(
        g_sentCommands, cmd->lastUserInputSequence);
    
    Vec3 originalLocalPos = sourceInput->avatarPos;
    Vec3 currentLocalPos = ent->body.t.pos;
    Vec3 remotePos = cmd->latestAvatarPos;
    
    if (Vec3_AreDifferent(&originalLocalPos, &remotePos, F32_EPSILON))
    {
        // Server disagrees with our recorded position at this time.
        // Therefore we must correct.
        ent->body.t.pos = remotePos;
        ent->body.previousPos = remotePos;
        if (verbose)
        {
            APP_PRINT(256,
                "  Correcting CL vs SV: %.3f, %.3f, %.3f vs %.3f, %.3f, %.3f\n",
                originalLocalPos.x, originalLocalPos.y, originalLocalPos.z,
                remotePos.x, remotePos.y, remotePos.z
            );
        }
    }
    else
    {
        if (verbose)
        {
            APP_LOG(256,
                "  No correction for local vs server positions: %.3f, %.3f, %.3f vs %.3f, %.3f, %.3f\n",
                originalLocalPos.x, originalLocalPos.y, originalLocalPos.z,
                remotePos.x, remotePos.y, remotePos.z
            );
        }
        
        ent->body.t.pos = originalLocalPos;
        ent->body.previousPos = originalLocalPos;
    }

    // Mark that this entity has been moved this frame already
    //ent->hasBeenPredicted = 1;
    
    // Replay frames
    i32 replaySequence = cmd->lastUserInputSequence;
    if (replaySequence < 0) { replaySequence = 0;  }
    i32 lastSequence = g_userInputSequence;
    //framesSinceResponse = 0;
    #if 1
    for (i32 i = 0; i <= framesSinceResponse; ++i)
    {
        replaySequence = cmd->lastUserInputSequence + i;
        C2S_Input* input = CL_RecallSentInputCommand(
            g_sentCommands, replaySequence);
        if (!input) { continue; }
		Vec3 before = ent->body.t.pos;
		CLG_StepActor(sim, ent, &input->input, input->deltaTime);
		Vec3 after = ent->body.t.pos;
        if (verbose)
        {
            APP_LOG(256, "\t\tSeq %d Buttons %d: %.3f, %.3f, %.3f to %.3f, %.3f, %.3f\n",
			    replaySequence,
                input->input.buttons,
			    before.x, before.y, before.z,
			    after.x, after.y, after.z
			);
        }
		
    }
    #endif
    #if 0
    // While loop version
    while (replaySequence < lastSequence)
    {
        C2S_Input* input = CL_RecallSentInputCommand(
            g_sentCommands, replaySequence);
		
		CLG_StepActor(ent, &input->input, input->deltaTime);
        
        replaySequence++;
    }
    #endif
    //printf("\n");
}

internal void CLG_FireActorAttack(SimScene* sim, SimEntity* ent, Vec3* dir)
{
    /* Debug - Fire a local projectile to see how it matches the server */
    #if 0
    SimBulkSpawnEvent def = {};
    def.factoryType = SIM_FACTORY_TYPE_PROJ_PREDICTION;
    def.base.firstSerial = Sim_ReserveEntitySerial(sim, 1);
    def.base.pos = ent->body.t.pos;
    def.base.seedIndex = 0;
    def.base.forward = *dir;
    def.base.tick = g_ticks;
    //Sim_ExecuteEnemySpawn()
    Sim_ExecuteBulkSpawn(sim, &def, 0);
    #endif
	
    /* Debug - Create a line trace */
    #if 0
    Vec3 origin = ent->body.t.pos;
    Vec3 dest {};
    dest.x = (origin.x + (dir->x * 10));
    dest.y = (origin.y + (dir->y * 10));
    dest.z = (origin.z + (dir->z * 10));
    CLG_SpawnLineSegment(sim, origin, dest);
    #endif
}

CLG_DEFINE_ENT_UPDATE(Actor)
{
    // Movement
    CLG_StepActor(sim, ent, &ent->input, deltaTime); 
    
    //////////////////////////////////////////////////////////////
    // Predicted shooting
    //////////////////////////////////////////////////////////////
    
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
			CLG_FireActorAttack(sim, ent, &shoot);
        }
    }
    else
    {
        ent->attackTick -= deltaTime;
    }
}

CLG_DEFINE_ENT_UPDATE(LineTrace)
{
    if (sim->tick >= ent->timing.nextThink)
    {
        Sim_RemoveEntity(sim, ent->id.serial);
    }
}

CLG_DEFINE_ENT_UPDATE(Spawn)
{
    /*
    if (ent->fastForwardTicks > 0)
    {
        printf("CL Fast forward ent by %d!\n", ent->fastForwardTicks);
    }
    if (ent->thinkTick <= 0)
    {
        ent->tickType = ent->coreTickType;
    }
    else
    {
        ent->thinkTick -= deltaTime;
    }
    */
}


//////////////////////////////////////////////////////
// ENEMIES
//////////////////////////////////////////////////////
CLG_DEFINE_ENT_UPDATE(Wanderer)
{
    if (!g_tickEnemies) { return; }
    /*
    Vec3* pos = &ent->body.t.pos;
    ent->previousPos.x = pos->x;
    ent->previousPos.y = pos->y;
    ent->previousPos.z = pos->z;
    Vec3 move =
    {
        ent->velocity.x * deltaTime,
        ent->velocity.y * deltaTime,
        ent->velocity.z * deltaTime
    };
    
    ent->body.t.pos.x += move.x;
    ent->body.t.pos.y += move.y;
    ent->body.t.pos.z += move.z;
    
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
    */
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

CLG_DEFINE_ENT_UPDATE(Seeker)
{
    if (!g_tickEnemies) { return; }
    i32 targetId = ent->relationships.targetId.serial;
    if (targetId != 0)
    {
        SimEntity* target = Sim_GetEntityBySerial(sim, targetId);
        if (target != NULL)
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
    }
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

CLG_DEFINE_ENT_UPDATE(Bouncer)
{
    if (!g_tickEnemies) { return; }
    Sim_SimpleMove(ent, deltaTime);
    Sim_BoundaryBounce(ent, &sim->boundaryMin, &sim->boundaryMax);
}

CLG_DEFINE_ENT_UPDATE(Dart)
{
    if (!g_tickEnemies) { return; }
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

internal void CLG_TickEntity(SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    switch (ent->tickType)
    {
        case SIM_TICK_TYPE_PROJECTILE:
        { CLG_UpdateProjectile(sim, ent, deltaTime); } break;
		case SIM_TICK_TYPE_BOUNCER:
        { CLG_UpdateBouncer(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_SEEKER:
        { CLG_UpdateSeeker(sim, ent, deltaTime); } break;
		case SIM_TICK_TYPE_WANDERER:
        { CLG_UpdateWanderer(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_DART:
        { CLG_UpdateDart(sim, ent, deltaTime); } break;
		case SIM_TICK_TYPE_ACTOR:
        { CLG_UpdateActor(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_SPAWN:
        //{ CLG_UpdateSpawn(sim, ent, deltaTime); } break;
        { Sim_TickSpawner(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_LINE_TRACE:
        { CLG_UpdateLineTrace(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_EXPLOSION:
        { CLG_UpdateExplosion(sim, ent, deltaTime); } break;
        case SIM_TICK_TYPE_WORLD: { } break;
        case SIM_TICK_TYPE_NONE: { } break;
        default: { COM_ASSERT(0, "Client - unknown entity tick type") } break;
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
    sim->tick++;
}
