#pragma once

#include "game.h"

void Game_Init()
{
    g_num_entities = 0;
    //g_num_brains = 0;
    g_num_colliders = 0;
    g_numSceneRendComponents = 0;
    g_numUIRendComponents = 0;

    Ent* ent;
    EC_Renderer* renderer;

    // Init world and component lists
    g_world = {};
    g_world.aiControllerList.items = g_aiControllers;
    g_world.aiControllerList.count = GAME_MAX_ENTITIES;
    g_world.aiControllerList.max = GAME_MAX_ENTITIES;
    g_world.rendererList.items = g_renderers;
    g_world.rendererList.count = GAME_MAX_ENTITIES;
    g_world.rendererList.max = GAME_MAX_ENTITIES;

    // Ent 0, walls
    ent = Ent_GetFreeEntity();

    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0;
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 6;
    ent->transform.scale.y = 2;
    ent->transform.scale.z = 6;

    renderer = EC_AddRenderer(ent, &g_world);
    
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshInverseCube, 1, 1, 1, 5);

    
    // Ent 1, Octahedron
    ent = Ent_GetFreeEntity();
    ent->transform.scale.x = 0.5;
    ent->transform.scale.y = 0.5;
    ent->transform.scale.z = 0.5;
    EC_AIController* controller = EC_AddAIController(ent, &g_world);
    Ent_InitAIController(controller, 1, 0, 0, 5);

    renderer = EC_AddRenderer(ent, &g_world);
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 4);

    #if 0
    #endif
}
