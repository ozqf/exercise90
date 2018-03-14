#pragma once

#include "shared.h"


#define R_MAX_RENDER_OBJECTS 2048

global_variable RendObj g_rendObjects[R_MAX_RENDER_OBJECTS];
global_variable i32 g_numRenderObjects = 0;

global_variable RenderScene g_scene;

void R_Scene_CreateTestScene()
{
    RendObj* obj = g_rendObjects;

    // 0
    *obj = {};
    obj->transform.pos.z = -2;
    //g_meshPrimitive_quad
    //RendObj_SetAsColouredQuad(&ent->rendObj, 1, 0, 1);
    RendObj_SetAsMesh(obj, &g_meshPrimitive_quad, 0, 1, 0, 0);
    g_scene.numObjects++;
    obj++;

    // 1
    *obj = {};
    obj->transform.rot.y = 90;
    obj->transform.pos.x = -2;
    RendObj_SetAsColouredQuad(obj, 0, 1, 0);
    g_scene.numObjects++;
    obj++;
    
    // 2
    *obj = {};
    obj->transform.rot.y = 270;
    obj->transform.pos.x = 2;
    RendObj_SetAsColouredQuad(obj, 0, 0, 1);
    g_scene.numObjects++;
    obj++;
    
    // 3
    *obj = {};
    obj->transform.rot.y = 180;
    obj->transform.pos.z = 2;
    RendObj_SetAsColouredQuad(obj, 1, 1, 0);
    g_scene.numObjects++;
    obj++;

    // 4
    *obj = {};
    obj->transform.rot.y = 0;
    obj->transform.pos.x = -3;
    obj->transform.pos.z = -4;
    obj->type = RENDOBJ_TYPE_BILLBOARD;
    g_scene.numObjects++;
    obj++;
    
    // 5
    *obj = {};
    obj->transform.rot.y = 0;
    obj->transform.pos.x = 3;
    obj->transform.pos.z = -4;
    RendObj_SetAsColouredQuad(obj, 1, 0, 0);
    g_scene.numObjects++;
    obj++;
}

void R_Scene_Init()
{
    g_scene = {};
    g_scene.numObjects = 0;
    g_scene.maxObjects = R_MAX_RENDER_OBJECTS;
    g_scene.rendObjects = g_rendObjects;

    R_Scene_CreateTestScene();
}

void R_Scene_Tick(GameTime time, RenderScene* scene)
{
    RendObj* obj;
    i32 rotatingEntity = 5;

    obj = &scene->rendObjects[5];
    obj->transform.rot.y += 90 * time.deltaTime;
}
