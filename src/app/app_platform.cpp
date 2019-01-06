#pragma once

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "../sim/sim.h"
#include "app_textures.h"

internal PlatformInterface g_platform = {};
internal i32 g_simFrameRate = 10;
internal f32 g_simFrameAcculator = 0;
internal Heap g_heap;

// World scene
#define MAX_WORLD_SCENE_ITEMS 2048
internal RenderScene g_worldScene;
internal RenderListItem g_worldSceneItems[MAX_WORLD_SCENE_ITEMS];

/***************************************
* Private
***************************************/
internal f32 App_GetSimFrameInterval()
{
    return (1.0f / g_simFrameRate);
}

internal void App_RunSimFrame(f32 deltaTime)
{

}

/***************************************
* Define functions accessible to platform
***************************************/
internal i32  g_isValid = 0;

internal i32  App_Init()
{
    printf("App Init\n");

    //App_Win32_AttachErrorHandlers();

    // Memory
    u32 heapMB = 64;
    u32 mainMemorySize = MegaBytes(heapMB);
    MemoryBlock mem = {};

    printf("APP Requested %d MB for Heap\n", heapMB);

    if (!g_platform.Platform_Malloc(&mem, mainMemorySize))
    {
        printf("APP Platform malloc failed\n");
        Assert(false);
        return 0;
    }
    else
    {
        Heap_Init(&g_heap, mem.ptrMemory, mem.size);
    }
    
    // Assets
    Tex_Init(&g_heap, g_platform);

    // Render Scenes
    RScene_Init(&g_worldScene, g_worldSceneItems, MAX_WORLD_SCENE_ITEMS);
    //RScene_Init(&g_weaponModelScene, g_weaponModel_renderList, GAME_MAX_ENTITIES);
    //RScene_Init(&g_uiScene, g_ui_renderList, UI_MAX_ENTITIES,
    //             90,
    //             RENDER_PROJECTION_MODE_IDENTITY,
    //             //RENDER_PROJECTION_MODE_ORTHOGRAPHIC,
    //             8);
    //
    g_worldScene.cameraTransform.pos.z += 4;
    //Transform t;
    //Transform_SetToIdentity(&t);
    //t.pos.z -= 2;
    //RendObj obj = {};
    //RendObj_SetAsMesh(&obj, g_meshCube, 1, 1, 1, Tex_GetTextureIndexByName("textures\\W33_5.bmp"));
    //RScene_AddRenderItem(&g_worldScene, &t, &obj);

    // create test scene
    for (i32 i = 0; i < 10; ++i)
    {
        Sim_AddEntity();
    }
    
    return COM_ERROR_NONE;
}

internal i32  App_Shutdown()
{
    printf("App Shutdown\n");
    
    // Free memory, assuming a new APP might be loaded in it's place
    MemoryBlock mem = {};
    mem.ptrMemory = g_heap.ptrMemory;
    mem.size = g_heap.size;
    g_platform.Platform_Free(&mem);

    return COM_ERROR_NONE;
}

internal i32  App_RendererReloaded()
{
    Tex_RenderModuleReloaded();
    return COM_ERROR_NONE;
}

internal void App_Input(PlatformTime* time, ByteBuffer commands)
{
    
}

internal void App_Update(PlatformTime* time)
{
    g_simFrameAcculator += time->deltaTime;
    //printf("App Accumulator %.4f\n", g_simFrameAcculator);
    f32 interval = App_GetSimFrameInterval();
    if (g_simFrameAcculator > interval)
    {
        g_simFrameAcculator -= interval;
        App_RunSimFrame(interval);
    }
}

internal void App_Render(PlatformTime* time, ScreenInfo info)
{
    g_worldScene.numObjects = 0;
    Sim_Entity* ents;
    i32 maxEnts;
    Sim_GetEntityList(&ents, &maxEnts);
    for (i32 i = 0; i < maxEnts; ++i)
    {
        Sim_Entity* ent = &ents[i];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }

        Transform t;
        Transform_SetToIdentity(&t);
        RendObj obj = {};
        MeshData* cube = COM_GetCubeMesh();
        RendObj_SetAsMesh(
            &obj, *cube, 1, 1, 1, Tex_GetTextureIndexByName("textures\\W33_5.bmp"));
        
        t.pos.x = ent->t.pos.x;
        t.pos.y = ent->t.pos.y;
        RScene_AddRenderItem(&g_worldScene, &t, &obj);
    }

    g_platform.Platform_RenderScene(&g_worldScene);
}

internal u8 App_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
    return 0;
}


/***************************************
* Export Windows DLL functions
***************************************/
#include <Windows.h>

extern "C"
AppInterface __declspec(dllexport) LinkToApp(PlatformInterface platInterface)
{
    printf("APP: Library Built on %s at %s\n", __DATE__, __TIME__);
    g_platform = platInterface;
    g_isValid = true;

    AppInterface app;
    app.isValid = true;
    app.AppInit = App_Init;
    app.AppShutdown = App_Shutdown;
    app.AppRendererReloaded = App_RendererReloaded;
    //app.AppFixedUpdate = App_FixedFrame;
    app.AppInput = App_Input;
    app.AppUpdate = App_Update;
    app.AppParseCommandString = App_ParseCommandString;
	app.AppRender = App_Render;
    return app;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    // TODO: Find out why this called seamingly at random whilst running
    printf("APP DLL Main\n");
	return 1;
}
