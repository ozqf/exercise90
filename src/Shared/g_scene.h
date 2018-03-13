#pragma once

#include "shared.h"


#define R_MAX_RENDER_OBJECTS 2048

global_variable Entity g_entities[R_MAX_RENDER_OBJECTS];
global_variable i32 g_numRenderObjects = 0;

global_variable RenderScene g_scene;

void Scene_CreateTestScene()
{
    Entity* ent = g_entities;

    // 0
    *ent = {};
    ent->transform.pos.z = -2;
    //g_meshPrimitive_quad
    //RendObj_SetAsColouredQuad(&ent->rendObj, 1, 0, 1);
    RendObj_SetAsMesh(&ent->rendObj, &g_meshPrimitive_quad, 0, 1, 0, 0);
    g_scene.numEntities++;
    ent++;

    // 1
    *ent = {};
    ent->transform.rot.y = 90;
    ent->transform.pos.x = -2;
    RendObj_SetAsColouredQuad(&ent->rendObj, 0, 1, 0);
    g_scene.numEntities++;
    ent++;
    
    // 2
    *ent = {};
    ent->transform.rot.y = 270;
    ent->transform.pos.x = 2;
    RendObj_SetAsColouredQuad(&ent->rendObj, 0, 0, 1);
    g_scene.numEntities++;
    ent++;
    
    // 3
    *ent = {};
    ent->transform.rot.y = 180;
    ent->transform.pos.z = 2;
    RendObj_SetAsColouredQuad(&ent->rendObj, 1, 1, 0);
    g_scene.numEntities++;
    ent++;

    // 4
    *ent = {};
    ent->transform.rot.y = 0;
    ent->transform.pos.x = -3;
    ent->transform.pos.z = -4;
    ent->rendObj.type = RENDOBJ_TYPE_BILLBOARD;
    g_scene.numEntities++;
    ent++;
    
    // 5
    *ent = {};
    ent->transform.rot.y = 0;
    ent->transform.pos.x = 3;
    ent->transform.pos.z = -4;
    RendObj_SetAsColouredQuad(&ent->rendObj, 1, 0, 0);
    g_scene.numEntities++;
    ent++;
}

void Scene_Init()
{
    g_scene = {};
    g_scene.numEntities = 0;
    g_scene.entities = g_entities;

    Scene_CreateTestScene();
}

void Scene_Tick(GameTime time, RenderScene* scene)
{
    Entity* ent;
    i32 rotatingEntity = 5;

    ent = &scene->entities[5];
    ent->transform.rot.y += 90 * time.deltaTime;
}
