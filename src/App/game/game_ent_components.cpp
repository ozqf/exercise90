#pragma once

#include "game.h"
#include "../common/com_module.h"

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

Ent* Game_FindEntityByLabel(GameState* gs, char* queryLabel)
{
    for (u32 i = 0; i < gs->labelList.max; ++i)
    {
        EC_Label* entLabel = &gs->labelList.items[i];
        if (!COM_CompareStrings(entLabel->label, queryLabel))
        {
            return Ent_GetEntity(&gs->entList, &entLabel->entId);
        }
    }
    return NULL;
}

void Game_SpawnTestBullet(GameState* gs, Transform* originT)
{
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    EntId id = Ent_ReserveFreeEntity(&gs->entList);
    Ent* ent = Ent_GetEntityAndAssign(&gs->entList, &id);
    
    //M4x4_SetToIdentity(ent->transform.matrix.cells);
    //ent->transform.matrix.wAxis = originT->matrix.wAxis;

    // Copy position and rotation
    ent->transform = *originT;
    Transform_SetScale(&ent->transform, 0.1f, 0.1f, 0.5f);
    
    EC_Renderer* rend = EC_AddRenderer(ent, gs);
    RendObj_SetAsMesh(&rend->rendObj, &g_meshSpike, 1, 1, 1, 0);
    rend->rendObj.flags = 0 | RENDOBJ_FLAG_DEBUG;

    //Vec4 scale = M4x4_GetScale(ent->transform.matrix.cells);

    EC_Projectile* prj = EC_AddProjectile(ent, gs);
    prj->tock = 4.0f;
    prj->tick = prj->tock;
    
    prj->move.x = -ent->transform.rotation.zAxis.x * TEST_PROJECTILE_SPEED;
    prj->move.y = -ent->transform.rotation.zAxis.y * TEST_PROJECTILE_SPEED;
    prj->move.z = -ent->transform.rotation.zAxis.z * TEST_PROJECTILE_SPEED;
    // move projectile forward a little
    ent->transform.pos.x += -ent->transform.rotation.zAxis.x * 1;
    ent->transform.pos.y += -ent->transform.rotation.zAxis.y * 1;
    ent->transform.pos.z += -ent->transform.rotation.zAxis.z * 1;
    
    prj->tick = 1.0f;
    prj->tock = 1.0f;
}

///////////////////////////////////////////////////////////////////
// Player
///////////////////////////////////////////////////////////////////
void Game_UpdateActorMotors(GameState* gs, GameTime* time, InputActionSet* actions)
{
    static float fireTick = 0;
    Ent* ent = Ent_GetEntityByIndex(&gs->entList, gs->playerEntityIndex);

    if (fireTick <= 0)
    {
        if (Input_GetActionValue(actions, "Attack 1"))
        {
            fireTick = 0.1f;
            Transform* t = &g_worldScene.cameraTransform;
            
            Game_SpawnTestBullet(gs, t);
        }
    }
    else
    {
        fireTick -= time->deltaTime;
    }
}

///////////////////////////////////////////////////////////////////
// Projectiles
///////////////////////////////////////////////////////////////////
void Game_UpdateProjectiles(GameState* gs, GameTime* time)
{
    for (u32 i = 0; i < gs->projectileList.max; ++i)
    {
        EC_Projectile* prj = &gs->projectileList.items[i];
        if (prj->inUse == 0) { continue; }

        Ent* e = Ent_GetEntityByIndex(&gs->entList, prj->entId.index);

        if (prj->tick <= 0.0f)
        {
            // Delete
            
            Ent_Free(gs, e);
        }
        else
        {
            prj->tick -= time->deltaTime;
        }

        Transform* t = &e->transform;

        Vec3 newPos = { prj->move.x, prj->move.y, prj->move.z};

        Phys_RayTest(t->pos.x, t->pos.y, t->pos.z, newPos.x, newPos.y, newPos.z);

        t->pos.x += prj->move.x * time->deltaTime;
        t->pos.y += prj->move.y * time->deltaTime;
        t->pos.z += prj->move.z * time->deltaTime;
    }
}

///////////////////////////////////////////////////////////////////
// Colliders
///////////////////////////////////////////////////////////////////

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
        if (collider->inUse == 1)
        {
            Ent* ent = Ent_GetEntityByIndex(&gs->entList, collider->entId.index);
            RenderListItem* item = &scene->sceneItems[scene->numObjects];
            scene->numObjects++;
            item->transform = ent->transform;
            Transform_SetScale(&item->transform, 1, 1, 1);
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

void Game_UpdateUI(GameState* gs, GameTime* time)
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
