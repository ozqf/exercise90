#pragma once

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "app.h"
#include "server/server.h"
#include "client/client.h"
#include "app_textures.h"

#include "stdlib.h"

internal PlatformInterface g_platform = {};
internal i32 g_simFrameRate = 5;
internal f32 g_simFrameAcculator = 0;
internal Heap g_heap;

// World scene
#define MAX_WORLD_SCENE_ITEMS 2048
internal RenderScene g_worldScene;
internal RenderListItem g_worldSceneItems[MAX_WORLD_SCENE_ITEMS];

internal ByteBuffer g_localClientPacket;
internal ByteBuffer g_localServerPacket;

/***************************************
* Access Server <-> Client communication buffers
* TODO: storage of multiple packets. Pump 'ForRead'
* to empty them out
***************************************/
ByteBuffer* App_GetLocalClientPacketForRead()
{
    return &g_localClientPacket;
}

ByteBuffer* App_GetLocalClientPacketForWrite()
{
    return &g_localClientPacket;
}

ByteBuffer* App_GetLocalServerPacketForRead()
{
    return &g_localServerPacket;
}

ByteBuffer* App_GetLocalServerPacketForWrite()
{
    return &g_localServerPacket;
}

/***************************************
* Private
***************************************/
internal f32 App_GetSimFrameInterval()
{
    return (1.0f / g_simFrameRate);
}

internal f32 App_CalcInterpolationTime(f32 accumulator, f32 interval)
{
    return (accumulator / interval);
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
    g_worldScene.cameraTransform.pos.y += 16;
    Transform_SetRotation(&g_worldScene.cameraTransform, -(90 * DEG2RAD), 0, 0);

    SV_Init();
    CL_Init();

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
        SV_Tick(interval);

        CL_Tick(interval);
        
        //App_RunSimFrame(interval);
    }
}

internal void App_OffsetRenderObjects(RenderScene* scene, i32 firstItem, f32 x)
{
    for (u32 i = (u32)firstItem; i < scene->numObjects; ++i)
    {
        RenderListItem* item = &scene->sceneItems[i];
        item->transform.pos.x += x;
    }
}

internal void App_Render(PlatformTime* time, ScreenInfo info)
{
    char* texName = "textures\\white_bordered.bmp";
    //char* texName = "textures\\W33_5.bmp";
    i32 texIndex = Tex_GetTextureIndexByName(texName);
    //f32 interpolationTime = 1;
    f32 interpolationTime = App_CalcInterpolationTime(g_simFrameAcculator, App_GetSimFrameInterval());
    
    g_worldScene.numObjects = 0;
    SV_PopulateRenderScene(&g_worldScene, g_worldScene.maxObjects, texIndex, 1);
    App_OffsetRenderObjects(&g_worldScene, 0, -10);
    i32 firstCLObject = g_worldScene.numObjects;

    CL_PopulateRenderScene(&g_worldScene, g_worldScene.maxObjects, texIndex, interpolationTime);
    App_OffsetRenderObjects(&g_worldScene, firstCLObject, 10);
    

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
