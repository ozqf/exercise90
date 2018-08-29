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
    return 1;
}

inline void AI_CalcTargetInfo(Transform* self, Transform* target, AITargetInfo* info)
{
    
    ////////////////////////////////////
    // flat plane angle
    info->dx = self->pos.x - target->pos.x;
    info->dy = self->pos.y - target->pos.y;
    info->dz = self->pos.z - target->pos.z;
    info->flatMagnitude = Vec3_Magnitudef(info->dx, 0, info->dz);
    info->yawRadians = atan2f(info->dx, info->dz);            
    ////////////////////////////////////
    // Pitch, look up/down angle
    info->pitchRadians = atan2f(info->dy, info->flatMagnitude);
}

inline void AI_ClearInput(GameState* gs, EC_AIController* ai)
{
    ai->state.state = 0;
    EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
    motor->state.input.buttons = 0;
}

inline void AI_Stun(GameState* gs, EC_AIController* ai)
{
    AI_ClearInput(gs, ai);
    ai->state.state = 2;
    ai->state.ticker.tick = 0.5f;
}

inline void AI_ApplyLookAngles(EC_ActorMotor* m, f32 yawRadians, f32 pitchRadians)
{
    m->state.input.degrees.y = yawRadians * RAD2DEG;
    m->state.input.degrees.x = -(pitchRadians * RAD2DEG);
}

void AI_Tock(GameState* gs, EC_AIController* ai)
{
    //printf("AI tock. State: %d\n", ai->state.state);
    #if 0
    switch (ai->state.state)
    {
        case AI_STATE_TRACKING:
        {
            if (!AI_AcquireAndValidateTarget(gs, &ai->state.target))
            {
                AI_ClearInput(gs, ai);
                return;
            }
            EC_Transform* selfTrans = EC_FindTransform(gs, &ai->header.entId);
            EC_Transform* targetTrans = EC_FindTransform(gs, &ai->state.target);
            if (targetTrans == NULL)
            {
                AI_ClearInput(gs, ai);
                return;
            }

            EC_Collider* col = EC_FindCollider(gs, &EC_GET_ID(ai));
            if (col != NULL && col->isGrounded == 0) { return; }

            AITargetInfo info = {};
            AI_CalcTargetInfo(&selfTrans->t, &targetTrans->t, &info);
            #if 0
            ////////////////////////////////////
            // flat plane angle
            f32 dx = selfTrans->t.pos.x - targetTrans->t.pos.x;
            f32 dz = selfTrans->t.pos.z - targetTrans->t.pos.z;

            f32 flatMagnitude = Vec3_Magnitudef(dx, 0, dz);

            f32 yawRadians = atan2f(dx, dz);
            
            ////////////////////////////////////
            // Pitch, look up/down angle
            f32 dy = selfTrans->t.pos.y - targetTrans->t.pos.y;
            f32 pitchRadians = atan2f(dy, flatMagnitude);
            #endif
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
            motor->state.input.buttons |= ACTOR_INPUT_ATTACK;
            
            AI_ApplyLookAngles(motor, info.yawRadians, info.pitchRadians);
        } break;

        case AI_STATE_STUNNED:
        {
            ai->state.state = AI_STATE_NULL;
        } break;

        case AI_STATE_BEGIN_ATTACK:
        {
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            motor->state.input.buttons = 0;
        } break;

        case AI_STATE_ATTACKING:
        {
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            motor->state.input.buttons = 1;
        } break;
        
        case AI_STATE_FINISH_ATTACK:
        {
            ai->state.state = AI_STATE_TRACKING;
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            motor->state.input.buttons = 0;
        } break;

        default:
        {
            if (AI_AcquireAndValidateTarget(gs, &ai->state.target))
            {
                ai->state.state = AI_STATE_TRACKING;
            }
            else if (ai->state.target.value != 0)
            {
                ai->state.target.value = 0;
                AI_ClearInput(gs, ai);
            }
        } break;
    }
    #endif
}

void Game_UpdateAIControllers(GameState *gs, GameTime *time)
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
        if (s->ticker.tick < 0)
        {
            s->ticker.tick = s->ticker.tickMax;
            AI_Tock(gs, ai);
        }
        else
        {
            s->ticker.tick -= time->deltaTime;
        }
    }
}
