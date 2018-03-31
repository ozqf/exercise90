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


    /////////////////////////////////////////////////////////////
    // A test HUD
    /////////////////////////////////////////////////////////////
    
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
    Game_BuildTestScene(&g_gameState);
    Game_BuildTestHud(&g_uiState);
    Game_BuildTestMenu();
}
