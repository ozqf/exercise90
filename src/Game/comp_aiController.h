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
//             ent->componentFlags |= ECOMP_FLAG_AI_CONTROLLER;
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
//     ent->componentFlags &= ~ECOMP_FLAG_AI_CONTROLLER;
//     obj->inUse = 0;
// }

void Ent_InitAIController(EC_AIController* controller, f32 dirX, f32 dirY, f32 dirZ, f32 speed)
{
    controller->dir.x = dirX;
    controller->dir.y = dirY;
    controller->dir.z = dirZ;
    controller->speed = speed;
}

f32 moveDistance = 12;

void Ent_UpdateAIControllers(GameState* gs, GameTime* time)
{
    #if 0
    u32 max = gs->aiControllerList.max;
    for (u32 i = 0; i < max; ++i)
    {
        EC_AIController* controller = &gs->aiControllerList.items[i];
        if (controller->inUse == 0) { continue; }
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
    }
    #endif
}
