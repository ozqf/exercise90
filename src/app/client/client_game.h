#pragma once

#include "client.h"

#define CLG_DEFINE_ENT_UPDATE(entityTypeName) internal void \
    CLG_Update##entityTypeName##(SimScene* sim, SimEntity* ent, f32 deltaTime)

CLG_DEFINE_ENT_UPDATE(Wanderer)
{
    /*
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
    */
}

CLG_DEFINE_ENT_UPDATE(Projectile)
{
	i32 numSteps = 1 + ent->fastForwardTicks;
    ent->fastForwardTicks = 0;
    //APP_LOG(64, "CL Prj %d steps: %d\n", ent->id.serial, numSteps);
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

// Return 1 if the command was successfully executed.
internal i32 CLG_SyncEntity(SimScene* sim, S2C_EntitySync* cmd)
{
    i32 executed = 0;
    SimEntity* ent = Sim_GetEntityBySerial(&g_sim, cmd->networkId);
    if (!ent)
    {
        APP_PRINT(128, "CL No ent %d for sync\n", cmd->networkId);
    }
    else
    {
        if (cmd->networkId == g_avatarSerial)
        {
            C2S_Input* input = CL_RecallSentInputCommand(g_sentCommands, cmd->header.tick);
            #if 0
            if (input == NULL)
            {
                printf("CL Sent input for tick %d not found!\n", cmd->header.tick);
                return 1;
            }
            Vec3 pos = ent->t.pos;
            printf("CL Sync avatar tick %d Local: %.2f, %.2f, %.2f to %.2f, %.2f, %.2f\n",
                cmd->header.tick,
                pos.x, pos.y, pos.z,
                cmd->pos.x, cmd->pos.y, cmd->pos.z
                );
            #endif
            return 1;
        }
        //APP_LOG(64, "CL Sync ent %d\n", cmd->networkId);
        ent->previousPos = ent->t.pos;
        ent->t.pos = cmd->pos;
        executed = 1;
    }
    return executed;
}

CLG_DEFINE_ENT_UPDATE(Actor)
{
    if (ent->id.serial == g_avatarSerial)
    {

    }
    else
    {
        
    }
    
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
}

internal void CLG_TickEntity(SimScene* sim, SimEntity* ent, f32 deltaTime)
{
    switch (ent->entType)
    {
        case SIM_ENT_TYPE_WANDERER: { CLG_UpdateWanderer(sim, ent, deltaTime); } break;
        case SIM_ENT_TYPE_PROJECTILE: { CLG_UpdateProjectile(sim, ent, deltaTime); } break;
		case SIM_ENT_TYPE_ACTOR: { CLG_UpdateActor(sim, ent, deltaTime); } break;
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

internal void CLG_HandlePlayerInput(SimEntity* ent, SimActorInput* input)
{
    if (input->buttons & ACTOR_INPUT_MOVE_FORWARD)
    {
        printf("Move Forward\n");
    }
    if (input->buttons & ACTOR_INPUT_MOVE_BACKWARD)
    {
        printf("Move Backward\n");
    }
    //printf("CL Cam angles: %.1f, %.1f\n", input->degrees.y, input->degrees.x);
}
