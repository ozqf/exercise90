#pragma once

#include "game.h"

void Game_BuildTestScene(GameState* gs)
{
    Ent* ent;
    EC_Renderer* renderer;
    EC_AIController* controller;
    EC_Collider* collider;
    EC_ActorMotor* motor;

    // Init gs and component lists
    *gs = {};

    gs->entList.items = g_gameEntities;
    gs->entList.count = GAME_MAX_ENTITIES;
    gs->entList.max = GAME_MAX_ENTITIES;

    gs->aiControllerList.items = g_aiControllers;
    gs->aiControllerList.count = GAME_MAX_ENTITIES;
    gs->aiControllerList.max = GAME_MAX_ENTITIES;

    gs->rendererList.items = g_renderers;
    gs->rendererList.count = GAME_MAX_ENTITIES;
    gs->rendererList.max = GAME_MAX_ENTITIES;

    gs->colliderList.items = g_colliders;
    gs->colliderList.count = GAME_MAX_ENTITIES;
    gs->colliderList.max = GAME_MAX_ENTITIES;

    gs->actorMotorList.items = g_actorMotors;
    gs->actorMotorList.count = GAME_MAX_ENTITIES;
    gs->actorMotorList.max = GAME_MAX_ENTITIES;

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
    collider->size = { testArenaWidth, 1, testArenaWidth };

    // walls
    
    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.x = ((testArenaWidth / 2) + 0.5f);
    collider = EC_AddCollider(ent, gs);
    collider->size = { 1, testArenaHeight, testArenaWidth };
    
    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.x = -((testArenaWidth / 2) + 0.5f);
    collider = EC_AddCollider(ent, gs);
    collider->size = { 1, testArenaHeight, testArenaWidth };
    
    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.z = -((testArenaWidth / 2) + 0.5f);
    collider = EC_AddCollider(ent, gs);
    collider->size = { testArenaWidth, testArenaHeight, 1 };
    
    ent = Ent_GetFreeEntity(&gs->entList);
    ent->transform.pos.z = ((testArenaWidth / 2) + 0.5f);
    collider = EC_AddCollider(ent, gs);
    collider->size = { testArenaWidth, testArenaHeight, 1 };
    
    // end room

    /////////////////////////////////////////////////////////////
    // Player
    /////////////////////////////////////////////////////////////
    ent = Ent_GetFreeEntity(&gs->entList);
    
    collider = EC_AddCollider(ent, gs);
    collider->size = { 1, 2, 1 };
    
    motor = EC_AddActorMotor(ent, gs);
    motor->speed = 5;

    gs->playerEntityIndex = ent->entId.index;

    /////////////////////////////////////////////////////////////
    // Octahedron object
    /////////////////////////////////////////////////////////////
    ent = Ent_GetFreeEntity(&gs->entList);
    
    controller = EC_AddAIController(ent, gs);
    Ent_InitAIController(controller, 1, 0, 0, 5);

    renderer = EC_AddRenderer(ent, gs);
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);

    collider = EC_AddCollider(ent, gs);
    collider->size = { 1, 1, 1 };
}

void Game_BuildTestHud(GameState* state)
{
    
}

void Game_BuildTestMenu()
{

}

void Game_Init()
{
    Game_BuildTestScene(&g_gameState);
    Game_BuildTestHud(&g_uiState);
    Game_BuildTestMenu();
}
