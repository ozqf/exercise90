#pragma once

#include "game.h"

void Game_BuildTestScene(World* world)
{
    g_num_entities = 0;
    //g_num_brains = 0;
    //g_num_colliders = 0;
    //g_numSceneRendComponents = 0;
    //g_numUIRendComponents = 0;

    Ent* ent;
    EC_Renderer* renderer;
    EC_AIController* controller;
    EC_Collider* collider;
    EC_ActorMotor* motor;

    // Init world and component lists
    *world = {};
    world->aiControllerList.items = g_aiControllers;
    world->aiControllerList.count = GAME_MAX_ENTITIES;
    world->aiControllerList.max = GAME_MAX_ENTITIES;

    world->rendererList.items = g_renderers;
    world->rendererList.count = GAME_MAX_ENTITIES;
    world->rendererList.max = GAME_MAX_ENTITIES;
    world->colliderList.items = g_colliders;
    world->colliderList.count = GAME_MAX_ENTITIES;
    world->colliderList.max = GAME_MAX_ENTITIES;
    world->actorMotorList.items = g_actorMotors;
    world->actorMotorList.count = GAME_MAX_ENTITIES;
    world->actorMotorList.max = GAME_MAX_ENTITIES;

    const f32 testArenaSize = 6;

    // Ent 0, walls mesh
    ent = Ent_GetFreeEntity();

    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0;
    ent->transform.pos.z = 0;
    ent->transform.scale.x = testArenaSize;
    ent->transform.scale.y = 2;
    ent->transform.scale.z = testArenaSize;

    renderer = EC_AddRenderer(ent, world);
    
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshInverseCube, 1, 1, 1, 5);
    
    /////////////////////////////////////////////////////////////
    // Room
    /////////////////////////////////////////////////////////////
    // Floor
    ent = Ent_GetFreeEntity();
    ent->transform.pos.y = -1.5f;
    collider = EC_AddCollider(ent, world);
    collider->size = { testArenaSize, 1, testArenaSize };

    // walls
    
    ent = Ent_GetFreeEntity();
    ent->transform.pos.x = 3.5f;
    collider = EC_AddCollider(ent, world);
    collider->size = { 1, 2, testArenaSize };
    
    ent = Ent_GetFreeEntity();
    ent->transform.pos.x = -3.5f;
    collider = EC_AddCollider(ent, world);
    collider->size = { 1, 2, testArenaSize };
    
    ent = Ent_GetFreeEntity();
    ent->transform.pos.z = 3.5f;
    collider = EC_AddCollider(ent, world);
    collider->size = { testArenaSize, 2, 1 };
    
    ent = Ent_GetFreeEntity();
    ent->transform.pos.z = -3.5f;
    collider = EC_AddCollider(ent, world);
    collider->size = { testArenaSize, 2, 1 };
    
    // Player
    ent = Ent_GetFreeEntity();
    
    collider = EC_AddCollider(ent, world);
    collider->size = { 1, 2, 1 };
    
    motor = EC_AddActorMotor(ent, world);
    motor->speed = 5;

    world->playerEntityIndex = ent->entId.index;

    // end room

    // Octahedron object
    ent = Ent_GetFreeEntity();
    
    controller = EC_AddAIController(ent, world);
    Ent_InitAIController(controller, 1, 0, 0, 5);

    renderer = EC_AddRenderer(ent, world);
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);

    collider = EC_AddCollider(ent, world);
    collider->size = { 1, 1, 1 };
}

void Game_Init()
{
    Game_BuildTestScene(&g_world);

}
