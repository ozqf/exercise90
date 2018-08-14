#pragma once

#include "game.h"

u8 AI_AcquireAndValidateTarget(GameState *gs, EntId* id)
{
    if (id->value == 0) { *id = AI_FindNearestPlayer(gs, {0, 0, 0}); }
    if (id->value == 0) { return 0; }
    Ent* ent = Ent_GetEntityById(gs, id);
    if (ent->inUse != ENTITY_STATUS_IN_USE) { return 0; }
    return 1;
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

void AI_Tock(GameState* gs, EC_AIController* ai)
{
    //printf("AI tock\n");
    switch (ai->state.state)
    {
        case 1:
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

            //f32 dx = targetTrans->t.pos.x - selfTrans->t.pos.x;
            //f32 dz = targetTrans->t.pos.z - selfTrans->t.pos.z;
            f32 dx = selfTrans->t.pos.x - targetTrans->t.pos.x;
            f32 dz = selfTrans->t.pos.z - targetTrans->t.pos.z;

            f32 mag = Vec3_Magnitudef(dx, 0, dz);
            
            if (mag < 2.0f)
            {
                //printf("mag: %.2f STOP\n", mag);
                AI_ClearInput(gs, ai);
                return;
            }
            else
            {
                //printf("mag: %.2f PUPPY!\n", mag);
            }
            

            f32 radians = atan2f(dx, dz);
            //printf("  To player: %.2f\n", radians * RAD2DEG);

            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            motor->state.input.degrees.y = radians * RAD2DEG;
            motor->state.input.buttons |= ACTOR_INPUT_MOVE_FORWARD;

        } break;
        case 2:
        {
            ai->state.state = 1;
        } break;
        default:
        {
            if (AI_AcquireAndValidateTarget(gs, &ai->state.target))
            {
                ai->state.state = 1;
            }
        } break;
    }
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
