#pragma once

#include "game.h"

#include "game_intersection_test.cpp"

void Game_BuildTestScene(GameState *gs)
{
    Ent *ent;
    EC_Renderer *renderer;
    EC_AIController *controller;
    EC_Collider *collider;
    EC_ActorMotor *motor;

    // Init gs and component lists
    *gs = {};

    gs->entList.items = g_gameEntities;
    gs->entList.count = GAME_MAX_ENTITIES;
    gs->entList.max = GAME_MAX_ENTITIES;

    gs->rendererList.items = g_renderers;
    gs->rendererList.count = GAME_MAX_ENTITIES;
    gs->rendererList.max = GAME_MAX_ENTITIES;

    gs->aiControllerList.items = g_aiControllers;
    gs->aiControllerList.count = GAME_MAX_ENTITIES;
    gs->aiControllerList.max = GAME_MAX_ENTITIES;

    gs->colliderList.items = g_colliders;
    gs->colliderList.count = GAME_MAX_ENTITIES;
    gs->colliderList.max = GAME_MAX_ENTITIES;

    gs->actorMotorList.items = g_actorMotors;
    gs->actorMotorList.count = GAME_MAX_ENTITIES;
    gs->actorMotorList.max = GAME_MAX_ENTITIES;

    gs->projectileList.items = g_prjControllers;
    gs->projectileList.count = GAME_MAX_ENTITIES;
    gs->projectileList.max = GAME_MAX_ENTITIES;

    const f32 testArenaWidth = 12;
    const f32 testArenaHeight = 4;
    const f32 wallHalfWidth = 0.5f;

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // Build Entities
    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // Walls mesh
    /////////////////////////////////////////////////////////////
    ent = Ent_GetFreeEntity(&gs->entList);

    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0;
    ent->transform.pos.z = 0;
    ent->transform.scale.x = testArenaWidth;
    ent->transform.scale.y = testArenaHeight;
    ent->transform.scale.z = testArenaWidth;

    renderer = EC_AddRenderer(ent, gs);

    RendObj_SetAsMesh(&renderer->rendObj, &g_meshInverseCube, 1, 1, 1, 5);

    /////////////////////////////////////////////////////////////
    // Room
    /////////////////////////////////////////////////////////////
    // Floor
    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.y = -((testArenaHeight / 2) + 0.5);
    collider = EC_AddCollider(ent, gs);
    collider->size = {testArenaWidth, 1, testArenaWidth};

    // walls

    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.x = ((testArenaWidth / 2) + 0.5f);
    collider = EC_AddCollider(ent, gs);
    collider->size = {1, testArenaHeight, testArenaWidth};

    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.x = -((testArenaWidth / 2) + 0.5f);
    collider = EC_AddCollider(ent, gs);
    collider->size = {1, testArenaHeight, testArenaWidth};

    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.z = -((testArenaWidth / 2) + 0.5f);
    collider = EC_AddCollider(ent, gs);
    collider->size = {testArenaWidth, testArenaHeight, 1};

    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.z = ((testArenaWidth / 2) + 0.5f);
    collider = EC_AddCollider(ent, gs);
    collider->size = {testArenaWidth, testArenaHeight, 1};

// end room

/////////////////////////////////////////////////////////////
// Player
/////////////////////////////////////////////////////////////
#if 1
    ent = Ent_GetFreeEntity(&gs->entList);

    collider = EC_AddCollider(ent, gs);
    collider->size = {1, 2, 1};

    motor = EC_AddActorMotor(ent, gs);
    motor->speed = 5;

    gs->playerEntityIndex = ent->entId.index;
#endif
    /////////////////////////////////////////////////////////////
    // Octahedron object
    /////////////////////////////////////////////////////////////'
    ent = Ent_GetFreeEntity(&gs->entList);
    ent->tag = 1;
    ent->transform.pos.y = 5;
    ent->transform.pos.z = -3;
    ent->transform.scale.x = 0.1f;
    ent->transform.scale.y = 0.1f;
    ent->transform.scale.z = 0.5f;
    ent->transform.rot.x = 22.5; // Pitch
    ent->transform.rot.y = 45;   // Yaw
    //ent->transform.rot.z = 0;   // Roll

    controller = EC_AddAIController(ent, gs);
    Ent_InitAIController(controller, 1, 0, 0, 5);

    renderer = EC_AddRenderer(ent, gs);
    //RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshSpike, 1, 1, 1, 2);

    collider = EC_AddCollider(ent, gs);
    collider->size = {1, 1, 1};
    collider->shapeId = Phys_CreateSphere(
        ent->transform.pos.x,
        ent->transform.pos.y,
        ent->transform.pos.z,
        1,
        ent->entId.index,
        ent->entId.iteration);

    // and again
    ent = Ent_GetFreeEntity(&gs->entList);
    ent->tag = 2;
    ent->transform.pos.x = 5;
    ent->transform.pos.y = 6;
    ent->transform.pos.z = 3;
    ent->transform.scale.x = 0.1f;
    ent->transform.scale.y = 0.1f;
    ent->transform.scale.z = 0.5f;
    ent->transform.rot.x = 22.5; // Pitch
    ent->transform.rot.y = 45;   // Yaw
                                 //ent->transform.rot.z = 0;   // Roll

    controller = EC_AddAIController(ent, gs);
    Ent_InitAIController(controller, 1, 0, 0, 5);

    renderer = EC_AddRenderer(ent, gs);
    //RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshSpike, 1, 1, 1, 2);

    collider = EC_AddCollider(ent, gs);
    collider->size = {1, 1, 1};
    collider->shapeId = Phys_CreateSphere(
        ent->transform.pos.x,
        ent->transform.pos.y,
        ent->transform.pos.z,
        1,
        ent->entId.index,
        ent->entId.iteration);

    // and again
    ent = Ent_GetFreeEntity(&gs->entList);
    ent->tag = 3;
    ent->transform.pos.y = 16;
    ent->transform.pos.x = -3;
    ent->transform.pos.z = -3;
    ent->transform.scale.x = 0.1f;
    ent->transform.scale.y = 0.1f;
    ent->transform.scale.z = 0.5f;
    ent->transform.rot.x = 22.5; // Pitch
    ent->transform.rot.y = 45;   // Yaw
                                 //ent->transform.rot.z = 0;   // Roll

    controller = EC_AddAIController(ent, gs);
    Ent_InitAIController(controller, 1, 0, 0, 5);

    renderer = EC_AddRenderer(ent, gs);
    //RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshSpike, 1, 1, 1, 2);

    collider = EC_AddCollider(ent, gs);
    collider->size = {1, 1, 1};
    collider->shapeId = Phys_CreateSphere(
        ent->transform.pos.x,
        ent->transform.pos.y,
        ent->transform.pos.z,
        1,
        ent->entId.index,
        ent->entId.iteration);
}

void Game_BuildTestHud(GameState *state)
{
#if 0
    Ent* ent;
    EC_Renderer* renderer;
    
    // Init gs and component lists
    *state = {};

    state->entList.items = g_uiEntities;
    state->entList.count = UI_MAX_ENTITIES;
    state->entList.max = UI_MAX_ENTITIES;

    state->rendererList.items = g_ui_renderers;
    state->rendererList.count = UI_MAX_ENTITIES;
    state->rendererList.max = UI_MAX_ENTITIES;
#endif

/////////////////////////////////////////////////////////////
// A test HUD
/////////////////////////////////////////////////////////////
#if 0
    // UI
    ent = Ent_GetFreeEntity(&state->entList);
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 0.1f;
    ent->transform.scale.y = 0.1f;
    ent->transform.scale.z = 0.1f;

    renderer = EC_AddRenderer(ent, state);
    // RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);

    RendObj_SetAsSprite(&renderer->rendObj, SPRITE_MODE_UI, 4, 1, 1);
    //RendObj_SetSpriteUVs(&obj->obj.sprite, 0.0625, 0.125, 0.5625, 0.5625 + 0.0625);
    RendObj_CalculateSpriteAsciUVs(&renderer->rendObj.data.sprite, '.');
#endif

#if 0
    // Sentence
    ent = Ent_GetFreeEntity(&state->entList);
    ent->transform.pos.x = -1;
    ent->transform.pos.y = 1;
    renderer = EC_AddRenderer(ent, state);

    char* chars = g_debugStr->chars;
    i32 numChars = COM_StrLen(chars);
	//i32 numChars = g_testString;

    RendObj_SetAsAsciCharArray(&renderer->rendObj, chars, numChars, 0.05f);
    //obj->transform.pos.x = -1;//-0.75f;
    //obj->transform.pos.y = 1;//0.75f;
#endif

#if 0
    /////////////////////////////////////////////////////////////
    // Walls mesh
    /////////////////////////////////////////////////////////////
    ent = Ent_GetFreeEntity(&state->entList);

    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0;
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 12;
    ent->transform.scale.y = 4;
    ent->transform.scale.z = 12;

    renderer = EC_AddRenderer(ent, state);
    
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshInverseCube, 1, 1, 1, 5);
#endif
}

void Game_BuildTestMenu()
{
}

void Game_Init()
{
    Phys_Init();

    Game_BuildTestScene(&g_gameState);
    Game_BuildTestHud(&g_uiState);
    Game_BuildTestMenu();
}

void Game_Shutdown()
{
    Phys_Shutdown();
}

inline void Game_UpdateEntityPosition(GameState *gs, EntId *entId, f32 x, f32 y, f32 z)
{
    Ent *ent = Ent_GetEntity(&gs->entList, entId);
    if (ent != NULL)
    {
        ent->transform.pos.x = x;
        ent->transform.pos.y = y;
        ent->transform.pos.z = z;
    }
}

inline void Game_HandleEntityUpdate(GameState *gs, ZTransformUpdateEvent *ev)
{
    EntId entId = {};
    entId.index = ev->ownerId;
    entId.iteration = ev->ownerIteration;
    Ent *ent = Ent_GetEntity(&gs->entList, &entId);
    if (ent == NULL)
    {
        return;
    }
    ent->transform.pos.x = ev->pos.x;
    ent->transform.pos.y = ev->pos.y;
    ent->transform.pos.z = ev->pos.z;

    ent->transform.rot.x = ev->rot.x;
    ent->transform.rot.y = ev->rot.y;
    ent->transform.rot.z = ev->rot.z;
}

#define GAME_DEBUG_MODE_NONE 0
#define GAME_DEBUG_MODE_CAMERA 1
#define GAME_DEBUG_MODE_ENT 2

ZStringHeader Game_WriteDebugString(GameState *gs, GameTime *time)
{
    Vec3 pos, rot;

    ZStringHeader h;
    h.chars = gs->debugString;
    h.length = 0;

    switch (gs->debugMode)
    {
        case GAME_DEBUG_MODE_CAMERA:
        {
            pos = gs->cameraTransform.pos;
            rot = gs->cameraTransform.rot;
            AngleVectors vectors = {};
            Calc_AnglesToAxesZYX(&rot, &vectors.left, &vectors.up, &vectors.forward);
            h.length = sprintf_s(gs->debugString, gs->debugStringCapacity,
                                 "Game.DLL:\nTimeDelta: %3.7f\n-- Camera --\nPos: %3.3f, %3.3f, %3.3f\nRot: %3.3f, %3.3f, %3.3f\nForward: %3.3f, %3.3f, %3.3f\nUp: %3.3f, %3.3f, %3.3f\nLeft: %3.3f, %3.3f, %3.3f\n",
                                 time->deltaTime,
                                 pos.x, pos.y, pos.z,
                                 rot.x, rot.y, rot.z,
                                 vectors.forward.x, vectors.forward.y, vectors.forward.z,
                                 vectors.up.x, vectors.up.y, vectors.up.z,
                                 vectors.left.x, vectors.left.y, vectors.left.z);
        }
        break;

        case GAME_DEBUG_MODE_ENT:
        {
            Ent *ent = Ent_GetEntityByTag(&gs->entList, 1);
            if (ent == NULL)
            {
                h.length = sprintf_s(gs->debugString, gs->debugStringCapacity, "No Debug Entity found\n");
            }
            else
            {
                pos = ent->transform.pos;
                rot = ent->transform.rot;
                AngleVectors vectors = {};
                h.length = sprintf_s(gs->debugString, gs->debugStringCapacity,
                                     "Game.DLL:\nTimeDelta: %3.7f\n-- Debug Entity --\nPos: %3.3f, %3.3f, %3.3f\nRot: %3.3f, %3.3f, %3.3f\n",
                                     time->deltaTime,
                                     pos.x, pos.y, pos.z,
                                     rot.x, rot.y, rot.z);
            }
            
        }
        break;

        default:
        {
            h.length = 0;
            return h;
        }
        break;
    }

    return h;
}

void Game_Tick(GameState *gs, MemoryBlock *eventBuffer, GameTime *time, InputTick *input)
{
    Game_ApplyInputToTransform(input, &gs->cameraTransform, time);

    Phys_Step(eventBuffer, time->deltaTime);
    //u8* ptr = (u8*)eventBuffer->ptrMemory;
    i32 ptrOffset = 0;
    u8 reading = 1;
    while (reading && ptrOffset < eventBuffer->size)
    {
        u8 *mem = (u8 *)((u8 *)eventBuffer->ptrMemory + ptrOffset);
        i32 type = *(i32 *)mem;
        switch (type)
        {
        case 1:
        {
            ZTransformUpdateEvent tUpdate = {};
            COM_CopyMemory(mem, (u8 *)&tUpdate, sizeof(ZTransformUpdateEvent));
            ptrOffset += sizeof(ZTransformUpdateEvent);
            Game_HandleEntityUpdate(gs, &tUpdate);
            //Game_UpdateEntityPosition(gs, &entId, tUpdate.pos.x, tUpdate.pos.y, tUpdate.pos.z);
            //ptr += sizeof(ZTransformUpdateEvent);
        }
        break;

        default:
        {
            reading = 0;
        }
        break;
        }
    }

    /*Ent* ent = Ent_GetEntityByTag(&gs->entList, 1);
    if (ent != NULL)
    {
        Vec3 p = Phys_DebugGetPosition();
        ent->transform.pos.x = p.x;
        ent->transform.pos.y = p.y;
        ent->transform.pos.z = p.z;
    }*/

    // Game state update
    // Update all inputs, entity components and colliders/physics
    Game_UpdateActorMotors(gs, time, input);
    Ent_UpdateAIControllers(gs, time);
    Game_UpdateColliders(gs, time);
    Game_UpdateProjectiles(gs, time);
    //Game_UpdateAI(time);
}
