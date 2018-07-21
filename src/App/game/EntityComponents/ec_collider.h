#pragma once

#include "game.h"
#include "../common/com_module.h"

///////////////////////////////////////////////////////////////////
// Colliders
///////////////////////////////////////////////////////////////////
EC_Collider* EC_ColliderGetByShapeId(EC_ColliderList* list, i32 shapeId)
{
    for (u32 i = 0; i < list->count; ++i)
    {
        EC_Collider* col = &list->items[i];
        if (col->shapeId == shapeId)
        {
            return col;
        }
    }
    printf("GAME No collider with shape Id %d found\n", shapeId);
    return NULL;
}

// Handled in physics engine now
void Game_UpdateColliders(GameState* gs, GameTime* time)
{
    #if 0
    u32 currentFrame = time->frameNumber;
    for (u32 i = 0; i < gs->colliderList.max; ++i)
    {
        EC_Collider* a = &gs->colliderList.items[i];
        if (a->inUse == 0) { continue; }
        Ent* entA = Ent_GetEntityByIndex(&gs->entList, a->entId.index);
        Transform* transA = &entA->transform;
        // Move collider
        // transA->pos.x += a->velocity.x * time->deltaTime;
        // transA->pos.y += a->velocity.y * time->deltaTime;
        // transA->pos.z += a->velocity.z * time->deltaTime;
        
        // for (u32 j = 0; j < gs->colliderList.max; ++j)
        // {
        //     if (i == j) { continue; }
        //     EC_Collider* b = &gs->colliderList.items[j];
        //     if (b->inUse == 0) { continue; }
        //     Ent* entB = Ent_GetEntityByIndex(&gs->entList, b->entId.index);
        //     Transform* transB = &entB->transform;

        //     if (AABBVsAABB(
        //         transA->pos.x,
        //         transA->pos.y,
        //         transA->pos.z,
        //         transB->pos.x,
        //         transB->pos.y,
        //         transB->pos.z,
        //         a->size.x / 2, a->size.y / 2, a->size.z / 2,
        //         b->size.x / 2, b->size.y / 2, b->size.z / 2

        //     ))
        //     {
        //         a->lastFrameOverlapping = currentFrame;
        //         b->lastFrameOverlapping = currentFrame;
        //     }
        // }
    }
    #endif
}

void Game_DrawColliderAABBs(GameState* gs, GameTime* time, RenderScene* scene)
{
    for (u32 i = 0; i < gs->colliderList.max; ++i)
    {
        EC_Collider* collider = &gs->colliderList.items[i];
        if (collider->header.inUse == 1)
        {
            Ent* ent = Ent_GetEntityByIndex(&gs->entList, collider->header.entId.index);
            RenderListItem* item = &scene->sceneItems[scene->numObjects];
            scene->numObjects++;
            item->transform = ent->transform;
            Transform_SetScale(&item->transform, 1, 1, 1);
            //if (collider->lastFrameOverlapping == time->frameNumber)
            if (collider->isGrounded)
            {
                RendObj_SetAsAABB(&item->obj, collider->state.size.x, collider->state.size.y, collider->state.size.z, 1, 0, 0);
            }
            else
            {
                RendObj_SetAsAABB(&item->obj, collider->state.size.x, collider->state.size.y, collider->state.size.z, 0, 1, 0);
            }
            
        }
    }
}
