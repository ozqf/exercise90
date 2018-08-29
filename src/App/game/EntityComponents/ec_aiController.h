#pragma once

#include "game.h"

#define AI_STATE_NULL 0
#define AI_STATE_TRACKING 1
#define AI_STATE_STUNNED 2
#define AI_STATE_BEGIN_ATTACK 3
#define AI_STATE_ATTACKING 4
#define AI_STATE_FINISH_ATTACK 5

struct AITargetInfo
{
    Vec3 selfPos;
    Vec3 targetPos;
    f32 dx, dy, dz;
    f32 pitchRadians, yawRadians;
    f32 flatMagnitude;
};

inline u8 AI_AcquireAndValidateTarget(GameState *gs, EntId* id)
{
    Ent* ent = Ent_GetEntityById(gs, id);
    if (!ent)
    {
        id->value = 0;
        *id = AI_FindNearestPlayer(gs, {0, 0, 0});
    }
    if (id->value == 0) { return 0; }
    //  printf("AI Acquired %d/%d\n", id->iteration, id->index);
    ent = Ent_GetEntityById(gs, id);
	if (!ent) { return 0; }
    if (ent->inUse != ENTITY_STATUS_IN_USE) { return 0; }
    if ((ent->componentBits & EC_FLAG_TRANSFORM) == 0) { return 0; }
    return 1;
}

inline void AI_BuildThinkInfo(GameState* gs, EC_AIController* ai, AITargetInfo* info)
{
    EC_Transform* selfTrans = EC_FindTransform(gs, &ai->header.entId);
    EC_Transform* targetTrans = EC_FindTransform(gs, &ai->state.target);
    Assert(selfTrans);
    Assert(targetTrans);

    info->selfPos = selfTrans->t.pos;
    info->targetPos = targetTrans->t.pos;

    ////////////////////////////////////
    // flat plane angle
    info->dx = info->selfPos.x - info->targetPos.x;
    info->dy = info->selfPos.y - info->targetPos.y;
    info->dz = info->selfPos.z - info->targetPos.z;
    info->flatMagnitude = Vec3_Magnitudef(info->dx, 0, info->dz);
    info->yawRadians = atan2f(info->dx, info->dz);            
    ////////////////////////////////////
    // Pitch, look up/down angle
    info->pitchRadians = atan2f(info->dy, info->flatMagnitude);
}

inline void AI_ClearInput(GameState* gs, EC_AIController* ai)
{
    EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
    motor->state.input.buttons = 0;
}

inline void AI_Reset(GameState* gs, EC_AIController* ai)
{
    AI_ClearInput(gs, ai);
    ai->state.state = AI_STATE_NULL;
}

inline void AI_Stun(GameState* gs, EC_AIController* ai, GameTime* time)
{
    AI_ClearInput(gs, ai);
    ai->state.state = AI_STATE_STUNNED;

}

inline void AI_ApplyLookAngles(EC_ActorMotor* m, f32 yawRadians, f32 pitchRadians)
{
    m->state.input.degrees.y = yawRadians * RAD2DEG;
    m->state.input.degrees.x = -(pitchRadians * RAD2DEG);
}

inline void AI_Think(GameState* gs, EC_AIController* ai, GameTime* time)
{
    //printf("AI Think %d\n", ai->state.state);
    switch (ai->state.state)
    {
        case AI_STATE_NULL:
        {
            if (AI_AcquireAndValidateTarget(gs, &ai->state.target))
            {
                ai->state.state = AI_STATE_TRACKING;
                //printf("Acquired target. State: %d\n", ai->state.state);
            }
            else if (ai->state.target.value != 0)
            {
                ai->state.target.value = 0;
                AI_Reset(gs, ai);
            }
        } break;

        case AI_STATE_TRACKING:
        {
			AITargetInfo info = {};
			if (!AI_AcquireAndValidateTarget(gs, &ai->state.target))
			{
				AI_Reset(gs, ai);
				return;
			}

            EC_Collider* col = EC_FindCollider(gs, &EC_GET_ID(ai));
            if (col != NULL && col->isGrounded == 0) { return; }

            AI_BuildThinkInfo(gs, ai, &info);
            if (info.flatMagnitude < 20)
            {
                ai->state.state = AI_STATE_ATTACKING;

                EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
                motor->state.input.buttons = 0;
                AI_ApplyLookAngles(motor, info.yawRadians, info.pitchRadians);
                ai->state.nextThink = time->sessionEllapsed + 0.3f;
            }
        } break;
        
        // case AI_STATE_BEGIN_ATTACK:
        // {
        //     EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
        //     motor->state.input.buttons = 0;
        //     ai->state.state = AI_STATE_ATTACKING;
            
        // } break;
        
        case AI_STATE_ATTACKING:
        {
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            EC_Transform* selfTrans = EC_FindTransform(gs, &EC_GET_ID(ai));
            Assert(motor);
            Assert(selfTrans);

            AttackInfo info = {};
            info.type = 2;//ai->state.attackIndex;
            info.origin = selfTrans->t.pos;
            info.source = EC_GET_ID(ai);
            info.yawDegrees = motor->state.input.degrees.y;
            info.pitchDegrees = motor->state.input.degrees.x;
            SV_FireAttack(gs, &info);
            ai->state.state = AI_STATE_TRACKING;
            ai->state.nextThink = time->sessionEllapsed + 1.0f;
        } break;
        
        case AI_STATE_STUNNED:
        {
            ai->state.state = AI_STATE_NULL;
        } break;


    }
}

inline void AI_Tick(GameState* gs, EC_AIController* ai, GameTime* time)
{
    //printf("AI tock. State: %d\n", ai->state.state);
    if (ai->state.nextThink <= time->sessionEllapsed)
    {
        AI_Think(gs, ai, time);
    }
    #if 1
    switch (ai->state.state)
    {
        case AI_STATE_TRACKING:
        {
            if (!AI_AcquireAndValidateTarget(gs, &ai->state.target))
            {
                printf("AI Failed to validate target\n");
                AI_Reset(gs, ai);
                return;
            }
            EC_Collider* col = EC_FindCollider(gs, &EC_GET_ID(ai));
            if (col != NULL && col->isGrounded == 0) { return; }

            AITargetInfo info = {};
            AI_BuildThinkInfo(gs, ai, &info);
            ////////////////////////////////////
            // Apply inputs and angles to motor
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            u8 applyMove = (info.flatMagnitude < ai->state.minApproachDistance);
            
            if (applyMove)
            {
                AI_ClearInput(gs, ai);
            }
            else
            {
                motor->state.input.buttons |= ACTOR_INPUT_MOVE_FORWARD;
            }
            AI_ApplyLookAngles(motor, info.yawRadians, info.pitchRadians);
        } break;

        case AI_STATE_FINISH_ATTACK:
        {
            ai->state.state = AI_STATE_TRACKING;
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            motor->state.input.buttons = 0;
        } break;
    }
    #endif
}

static void Game_UpdateAIControllers(GameState *gs, GameTime *time)
{
    u32 max = gs->aiControllerList.max;
    for (u32 i = 0; i < max; ++i)
    {
        EC_AIController *ai = &gs->aiControllerList.items[i];
        if (ai->header.inUse == 0)
        {
            continue;
        }
        EC_AIState *s = &ai->state;
        AI_Tick(gs, ai, time);
    }
}
