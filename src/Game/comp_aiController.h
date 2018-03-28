#pragma once

#include "game.h"

EntComp_AIController* Ent_AddAIController(Ent* ent)
{
    EntComp_AIController* obj = NULL;
    for (u32 i = 0; i < R_MAX_RENDER_OBJECTS; ++i)
    {
        obj = &g_aiControllers[i];
        if (obj->inUse == 0)
        {
            obj->inUse = 1;
            obj->entId.index = ent->entId.index;
            ent->componentFlags |= ECOMP_FLAG_AI_CONTROLLER;
            return obj;
        }
    }
    Assert(obj != NULL);
    return NULL;
}

void Ent_RemoveAIController(Ent* ent, EntComp_AIController* obj)
{
    Assert(ent != NULL);
    Assert(obj != NULL);
    Assert(ent->entId.index == obj->entId.index);
    ent->componentFlags &= ~ECOMP_FLAG_AI_CONTROLLER;
    obj->inUse = 0;
}

void Ent_InitAIController(EntComp_AIController* controller, f32 dirX, f32 dirY, f32 dirZ, f32 speed)
{
    controller->dir.x = dirX;
    controller->dir.y = dirY;
    controller->dir.z = dirZ;
    controller->speed = speed;
}

void Ent_UpdateAIControllers(EntComp_AIController* controllers, GameTime* time)
{
    for (i32 i = 0; i < WORLD_MAX_ENTITIES; ++i)
    {
        EntComp_AIController* controller = &controllers[i];
        Ent* ent = Ent_GetEntityByIndex(controller->entId.index);
        if (controller->dir.x < 0)
        {
            ent->transform.pos.x -= controller->speed * time->deltaTime;
            if (ent->transform.pos.x < -3)
            {
                ent->transform.pos.x = -3;
                controller->dir.x = 1;
            }
        }
        else if (controller->dir.x > 0)
        {
            ent->transform.pos.x += controller->speed * time->deltaTime;
            if (ent->transform.pos.x > 3)
            {
                ent->transform.pos.x = 3;
                controller->dir.x = -1;
            }
        }
    }
}
