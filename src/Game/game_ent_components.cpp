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
void Game_UpdateActorMotors(GameState* gs, GameTime* time, InputTick* input)
{
    #if 0
    Ent* ent = Ent_GetEntityByIndex(&gs->entList, gs->playerEntityIndex);
    EC_ActorMotor* aMotor = EC_FindActorMotor(ent, gs);
    EC_Collider* collider = EC_FindCollider(ent, gs);
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
    #endif
}

///////////////////////////////////////////////////////////////////
// Colliders
///////////////////////////////////////////////////////////////////

void Game_UpdateColliders(GameState* gs, GameTime* time)
{
    u32 currentFrame = time->frameNumber;
    for (u32 i = 0; i < gs->colliderList.max; ++i)
    {
        EC_Collider* a = &gs->colliderList.items[i];
        if (a->inUse == 0) { continue; }
        Ent* entA = Ent_GetEntityByIndex(&gs->entList, a->entId.index);

        // Move collider
        entA->transform.pos.x += a->velocity.x * time->deltaTime;
        entA->transform.pos.y += a->velocity.y * time->deltaTime;
        entA->transform.pos.z += a->velocity.z * time->deltaTime;
        
        for (u32 j = 0; j < gs->colliderList.max; ++j)
        {
            if (i == j) { continue; }
            EC_Collider* b = &gs->colliderList.items[j];
            if (b->inUse == 0) { continue; }
            Ent* entB = Ent_GetEntityByIndex(&gs->entList, b->entId.index);

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
                a->lastFrameOverlapping = currentFrame;
                b->lastFrameOverlapping = currentFrame;
            }
        }
    }
}

void Game_DrawColliderAABBs(GameState* gs, GameTime* time, RenderScene* scene)
{
    for (u32 i = 0; i < gs->colliderList.max; ++i)
    {
        EC_Collider* collider = &gs->colliderList.items[i];
        if (collider->inUse == 1)
        {
            Ent* ent = Ent_GetEntityByIndex(&gs->entList, collider->entId.index);
            RenderListItem* item = &scene->sceneItems[scene->numObjects];
            scene->numObjects++;
            item->transform = ent->transform;
            item->transform.scale = { 1, 1, 1 };
            if (collider->lastFrameOverlapping == time->frameNumber)
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

void Game_UpdateUI(GameState* gs, GameTime* time, InputTick* input)
{
    #if 0
    f32 speed = 8.0f;
    Ent* ent = &gs->entList.items[0];
    if (input->yawLeft)
    {
        ent->transform.pos.x -= speed * time->deltaTime;
    }
    if (input->yawRight)
    {
        ent->transform.pos.x += speed * time->deltaTime;
    }

    if (input->pitchUp)
    {
        ent->transform.pos.y += speed * time->deltaTime;
    }
    if (input->pitchDown)
    {
        ent->transform.pos.y -= speed * time->deltaTime;
    }

    if (input->rollLeft)
    {
        ent->transform.pos.z -= speed * time->deltaTime;
    }
    if (input->rollRight)
    {
        ent->transform.pos.z += speed * time->deltaTime;
    }
    #endif
}
