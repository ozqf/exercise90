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
    RendObj* obj;

    // Ent 0, walls
    ent = Ent_GetFreeEntity();

    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0;
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 6;
    ent->transform.scale.y = 2;
    ent->transform.scale.z = 6;

    obj = Ent_AddRenderObj(ent);
    
    RendObj_SetAsMesh(obj, &g_meshInverseCube, 1, 1, 1, 5);

    // Ent 1, Octahedron
    ent = Ent_GetFreeEntity();
    ent->transform.scale.x = 0.5;
    ent->transform.scale.y = 0.5;
    ent->transform.scale.z = 0.5;
    EntComp_AIController* controller = Ent_AddAIController(ent);
    Ent_InitAIController(controller, 1, 0, 0, 5);

    obj = Ent_AddRenderObj(ent);
    RendObj_SetAsMesh(obj, &g_meshOctahedron, 1, 1, 1, 4);

}
