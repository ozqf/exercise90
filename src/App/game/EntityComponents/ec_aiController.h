#pragma once

#include "game.h"

// EC_AIController* Ent_AddAIController(Ent* ent)
// {
//     EC_AIController* obj = NULL;
//     for (u32 i = 0; i < R_MAX_RENDER_OBJECTS; ++i)
//     {
//         obj = &g_aiControllers[i];
//         if (obj->inUse == 0)
//         {
//             obj->inUse = 1;
//             obj->entId.index = ent->entId.index;
//             ent->componentFlags |= EEC_FLAG_AI_CONTROLLER;
//             return obj;
//         }
//     }
//     Assert(obj != NULL);
//     return NULL;
// }

// void Ent_RemoveAIController(Ent* ent, EC_AIController* obj)
// {
//     Assert(ent != NULL);
//     Assert(obj != NULL);
//     Assert(ent->entId.index == obj->entId.index);
//     ent->componentFlags &= ~EEC_FLAG_AI_CONTROLLER;
//     obj->inUse = 0;
// }

void Ent_InitAIController(EC_AIController *controller, f32 dirX, f32 dirY, f32 dirZ, f32 speed)
{
    controller->state.dir.x = dirX;
    controller->state.dir.y = dirY;
    controller->state.dir.z = dirZ;
    controller->state.speed = speed;
}

f32 moveDistance = 12;

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

void AI_Stun(GameState* gs, EC_AIController* ai)
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
            //f32 dx = targetTrans->t.pos.x - selfTrans->t.pos.x;
            //f32 dz = targetTrans->t.pos.z - selfTrans->t.pos.z;
            f32 dx = selfTrans->t.pos.x - targetTrans->t.pos.x;
            f32 dz = selfTrans->t.pos.z - targetTrans->t.pos.z;

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

#if 1
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
#if 0
        Ent* ent = Ent_GetEntityByIndex(&gs->entList, controller->entId.index);
        if (controller->dir.x < 0)
        {
            ent->transform.pos.x -= controller->speed * time->deltaTime;
            if (ent->transform.pos.x < -moveDistance)
            {
                ent->transform.pos.x = -moveDistance;
                controller->dir.x = 1;
            }
        }
        else if (controller->dir.x > 0)
        {
            ent->transform.pos.x += controller->speed * time->deltaTime;
            if (ent->transform.pos.x > moveDistance)
            {
                ent->transform.pos.x = moveDistance;
                controller->dir.x = -1;
            }
        }
#endif
    }
#endif
}
