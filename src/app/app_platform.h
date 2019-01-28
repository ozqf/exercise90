#pragma once

#include "stdlib.h"
#include "../interface/sys_events.h"

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

    // Acquiring an old 'heap object here. Various platform functions
    // still use it for loading assets so can't remove. Future allocations
    // should just use the basic malloc tracker until further notice
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

    // Basic malloc tracker
    g_mallocs = COM_InitMallocList(g_mallocItems, APP_MAX_MALLOCS);

    i32 bufferSize = KiloBytes(64);

    // Internal loopbacks for client and server
    g_serverLoopback.a = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, bufferSize, "Server Loopback A"),
        bufferSize);
	g_serverLoopback.b = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, bufferSize, "Server Loopback B"),
        bufferSize);

    g_clientLoopback.a = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, bufferSize, "Client Loopback A"),
        bufferSize);
	g_clientLoopback.b = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, bufferSize, "Client Loopback B"),
        bufferSize);
    //g_localServerPacket = Buf_FromMalloc(COM_Malloc(&g_mallocs, bufferSize, "SV Packet"), bufferSize);
	
	g_loopbackSocket.Init(75, 150, 0.1f);

    g_localServerAddress = {};
    g_localServerAddress.port = APP_SERVER_LOOPBACK_PORT;

    // Render Scenes
    RScene_Init(&g_worldScene, g_worldSceneItems, MAX_WORLD_SCENE_ITEMS);
    g_worldScene.cameraTransform.pos.y += 16;
    Transform_SetRotation(&g_worldScene.cameraTransform, -(90 * DEG2RAD), 0, 0);

    // server and client areas currently acquiring their own memory
    App_StartSession(APP_SESSION_TYPE_SINGLE_PLAYER);
    //ZNet_Init(Net_GetPlatformFunctions(), Net_GetNetworkCallbacks(), ZNET_SIM_MODE_NONE);

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
	
    //g_localClientSocket.Destroy();
    //g_localServerSocket.Destroy();

    return COM_ERROR_NONE;
}

internal i32 App_EndSession()
{
    if (g_isRunningServer) { SV_Shutdown(); }
    if (g_isRunningClient) { CL_Shutdown(); }
    //ZNet_EndSession(g_serverNet);
    //ZNet_EndSession(g_clientNet);
    return COM_ERROR_NONE;
}

internal i32 App_StartSession(i32 sessionType)
{
    printf("\n=== START SESSION ===\n");
    switch (sessionType)
    {
        case APP_SESSION_TYPE_SINGLE_PLAYER:
        {
            App_EndSession();
            printf("\tStarting single player\n");
            /*ZNet_StartSession(
                g_serverNet,
                NETMODE_DEDICATED_SERVER,
                NULL,
                APP_SERVER_LOOPBACK_PORT);*/
            //g_localServerSocket.isActive = 1;
            SV_Init();
            UserIds ids = SV_CreateLocalUser();
            g_isRunningServer = 1;

            /*ZNet_StartSession(
                g_clientNet,
                NETMODE_CLIENT,
                &g_localServerAddress,
                APP_CLIENT_LOOPBACK_PORT);*/
            //g_localClientSocket.isActive = 1;
            ZNetAddress addr = {};
            addr.port = APP_SERVER_LOOPBACK_PORT;
            CL_Init(addr);
            CL_SetLocalUser(ids);
            g_isRunningClient = 1;

            return COM_ERROR_NONE;
        } break;
        default:
        {
            printf("Unknown Session type %d\n", sessionType);
            return COM_ERROR_BAD_ARGUMENT;
        };
    }
    
}

internal i32  App_RendererReloaded()
{
    Tex_RenderModuleReloaded();
    return COM_ERROR_NONE;
}

internal void App_Input(PlatformTime* time, ByteBuffer commands)
{
	i32 inputBytes = commands.Written();
	//printf("APP input %d bytes\n", inputBytes);
	
    u8* read = commands.ptrStart;
    u8* end = commands.ptrWrite;
    while (read < end)
    {
        SysEvent* header = (SysEvent*)read;
        Assert(Sys_ValidateEvent(header) == COM_ERROR_NONE)
        read += header->size;
		
        switch (header->type)
        {
            case SYS_EVENT_INPUT:
            {
                SysInputEvent* ev = (SysInputEvent*)header;
                //printf("APP input Event: %d value %d\n", ev->inputID, ev->value);
            } break;
            case SYS_EVENT_PACKET:
            {

            } break;

            case SYS_EVENT_SKIP: break;

            default:
            {
                printf("APP Unknown sys event type %d size %d\n", header->type, header->size);
            } break;
        }

    }
	
}

internal void App_Update(PlatformTime* time)
{
    g_simFrameAcculator += time->deltaTime;
    //printf("App Accumulator %.4f\n", g_simFrameAcculator);
    f32 interval = App_GetSimFrameInterval();
    if (g_simFrameAcculator > interval)
    {
        printf("\n === APP TICK (%.4fs) ===\n", interval);
        g_simFrameAcculator -= interval;
        
        /*
        TODO: Remove/reduce use of callbacks here, particularly for data packets.
        code is spagettifying and flow is not obvious.
        Approach should be:
        > Pump platform events and load into input buffer.
        > Pump remote socket for packets/events and load into input buffer.
        > Pump local socket for packets/events and load into input buffer
        > Tick server (read commands)
        > Clear buffer
        */
		
		App_UpdateLoopbackSocket(&g_loopbackSocket, interval);
		
		g_serverLoopback.Swap();
		Buf_Clear(g_serverLoopback.GetWrite());
		
        if (g_isRunningServer)
        {
            //g_localServerSocket.Tick(interval);
            //ZNet_Tick(g_serverNet, interval);
            printf("*** SV TICK ***\n");
            SV_Tick(g_serverLoopback.GetRead(), interval);
        }

        g_clientLoopback.Swap();
        Buf_Clear(g_clientLoopback.GetWrite());

        if (g_isRunningClient)
        {
            //g_localClientSocket.Tick(interval);
            printf("*** CL TICK ***\n");
            //ZNet_Tick(g_clientNet, interval);
            CL_Tick(g_clientLoopback.GetRead(), interval);
        }
        
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
    f32 interpolationTime = App_CalcInterpolationTime(
        g_simFrameAcculator, App_GetSimFrameInterval());
    
    // offset blocks of render objects left or right to show SV and CL side by side

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
    // ANSWER: For each thread that is started dllMain is called
    printf("APP DLL Main\n");
	return 1;
}
