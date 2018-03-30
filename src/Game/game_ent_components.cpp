#pragma once

#include "game.h"
#include "../Shared/Collision/shared_collision.h"

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

///////////////////////////////////////////////////////////////////
// Player
///////////////////////////////////////////////////////////////////
void Game_UpdateActorMotors(World* world, GameTime* time, InputTick* input)
{
    Ent* ent = Ent_GetEntityByIndex(world->playerEntityIndex);
    EC_ActorMotor* aMotor = EC_FindActorMotor(ent, world);
    EC_Collider* collider = EC_FindCollider(ent, world);
    if (aMotor == 0) { return; }

    aMotor->move = { 0, 0, 0 };
    
    collider->velocity = {};

    if (input->rollLeft)
    {
        collider->velocity.x = -aMotor->speed;
        //aMotor->move.x = -speed;
        //ent->transform.pos.x -= 1 * time->deltaTime;
    }
    if (input->rollRight)
    {
        collider->velocity.x = aMotor->speed;
        //aMotor->move.x = speed;
        //ent->transform.pos.x += 1 * time->deltaTime;
    }

}

///////////////////////////////////////////////////////////////////
// Colliders
///////////////////////////////////////////////////////////////////

void Game_UpdateColliders(World* world, GameTime* time)
{
    for (u32 i = 0; i < world->colliderList.max; ++i)
    {
        EC_Collider* a = &world->colliderList.items[i];
        if (a->inUse == 0) { continue; }
        Ent* entA = Ent_GetEntityByIndex(a->entId.index);

        // Move collider
        entA->transform.pos.x += a->velocity.x * time->deltaTime;
        entA->transform.pos.y += a->velocity.y * time->deltaTime;
        entA->transform.pos.z += a->velocity.z * time->deltaTime;

        for (u32 j = 0; j < world->colliderList.max; ++j)
        {
            if (i == j) { continue; }
            EC_Collider* b = &world->colliderList.items[j];
            if (b->inUse == 0) { continue; }
            Ent* entB = Ent_GetEntityByIndex(b->entId.index);

            if (AABBVsAABB(
                entA->transform.pos.x,
                entA->transform.pos.y,
                entA->transform.pos.z,
                entB->transform.pos.x,
                entB->transform.pos.y,
                entB->transform.pos.z,
                a->size.x / 2, a->size.y / 2, a->size.z / 2,
                b->size.x / 2, b->size.y / 2, b->size.z / 2

            ))
            {
                a->isOverlapping = 1;
                b->isOverlapping = 1;
            }
            else
            {
                a->isOverlapping = 0;
                b->isOverlapping = 0;
            }
        }
    }
}

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
            if (collider->isOverlapping)
            {
                RendObj_SetAsAABB(&item->obj, collider->size.x, collider->size.y, collider->size.z, 1, 0, 0);
            }
            else
            {
                RendObj_SetAsAABB(&item->obj, collider->size.x, collider->size.y, collider->size.z, 0, 1, 0);
            }
            
        }
    }
}
