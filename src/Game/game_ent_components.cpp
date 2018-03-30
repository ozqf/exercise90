#pragma once

#include "game.h"

// void Game_UpdateAI(GameTime* time)
// {
//     for (i32 i = 0; i < g_num_brains; ++i)
//     {
//         EntComp_TestBrain* brain = &g_game_brains[i];
//         Ent* ent = Ent_GetEntityByIndex(brain->entId.index);
//         Assert(ent != NULL);
//         ent->transform.rot.y += 90 * time->deltaTime;
//     }
// }


void Game_DrawColliderAABBs(World* world, RenderScene* scene)
{
    for (u32 i = 0; i < world->colliderList.max; ++i)
    {
        EC_Collider* collider = &world->colliderList.items[i];
        if (collider->inUse == 1)
        {
            Ent* ent = Ent_GetEntityByIndex(collider->entId.index);
            RenderListItem* item = &scene->sceneItems[scene->numObjects];
            scene->numObjects++;
            item->transform = ent->transform;
            item->transform.scale = { 1, 1, 1 };
            RendObj_SetAsAABB(&item->obj, 1, 1, 1, 1, 0, 0);
        }
    }
}
