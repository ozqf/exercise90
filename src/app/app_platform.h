#pragma once

#include "stdlib.h"

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
* Callbacks from net connection module
***************************************/
internal void Net_ConnectionAccepted(ZNetConnectionInfo* info)
{

}

internal void Net_ConnectionDropped(ZNetConnectionInfo* info)
{

}

internal void Net_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes)
{

}

internal void Net_DeliveryConfirmed(ZNetConnectionInfo* info, u32 packetNumber)
{

}
#if 0
internal ZNetPlatformFunctions Net_GetPlatformFunctions()
{
    ZNetPlatformFunctions funcs = {};
    funcs.Init = g_platform.Net_Init;
    funcs.Shutdown = g_platform.Net_Shutdown;
    funcs.OpenSocket = g_platform.Net_OpenSocket;
    funcs.CloseSocket = g_platform.Net_CloseSocket;
    funcs.Read = g_platform.Net_Read;
    funcs.SendTo = g_platform.Net_SendTo;
    funcs.FatalError = App_FatalError;
    return funcs;
}

internal ZNetOutputInterface Net_GetNetworkCallbacks()
{
    ZNetOutputInterface funcs = {};
    funcs.ConnectionAccepted = Net_ConnectionAccepted;
    funcs.ConnectionDropped = Net_ConnectionDropped;
    funcs.DataPacketReceived = Net_DataPacketReceived;
    funcs.DeliveryConfirmed = Net_DeliveryConfirmed;
    return funcs;
}
#endif
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

    g_localClientPacket = Buf_FromMalloc(COM_Malloc(&g_mallocs, KiloBytes(64), "CL Packet"), 64);
    g_localServerPacket = Buf_FromMalloc(COM_Malloc(&g_mallocs, KiloBytes(64), "SV Packet"), 64);

    // init client and server connection managers
    i32 znetInstanceSize = ZNet_RequiredInstanceSize();
    g_clientNet = (ZNetHandle*)COM_Malloc(&g_mallocs, znetInstanceSize, "CL Conn");
    g_clientNet->memSize = znetInstanceSize;
    ZNet_Init(
        g_clientNet,
        App_CLNet_CreatePlatformFunctions(),
        App_CLNet_CreateOutputFunctions(),
        ZNET_SIM_MODE_NONE);
    
    g_serverNet = (ZNetHandle*)COM_Malloc(&g_mallocs, znetInstanceSize, "SV Conn");
    g_serverNet->memSize = znetInstanceSize;
    ZNet_Init(
        g_serverNet,
        App_SVNet_CreatePlatformFunctions(),
        App_SVNet_CreateOutputFunctions(),
        ZNET_SIM_MODE_NONE);

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

    return COM_ERROR_NONE;
}

internal i32 App_EndSession()
{
    if (g_isRunningServer) { SV_Shutdown(); }
    if (g_isRunningClient) { CL_Shutdown(); }
    ZNet_EndSession(g_serverNet);
    ZNet_EndSession(g_clientNet);
    return COM_ERROR_NONE;
}

internal i32 App_StartSession(i32 sessionType)
{
    switch (sessionType)
    {
        case APP_SESSION_TYPE_SINGLE_PLAYER:
        {
            App_EndSession();
            ZNet_StartSession(g_serverNet, NETMODE_SINGLE_PLAYER, NULL, ZNET_LOOPBACK_PORT);
            ZNet_StartSession(g_clientNet, NETMODE_SINGLE_PLAYER, NULL, ZNET_LOOPBACK_PORT);
            SV_Init();
            UserIds ids = SV_CreateLocalUser();
            CL_Init();
            CL_SetLocalUser(ids);
            g_isRunningServer = 1;
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
    
}

internal void App_Update(PlatformTime* time)
{
    g_simFrameAcculator += time->deltaTime;
    //printf("App Accumulator %.4f\n", g_simFrameAcculator);
    f32 interval = App_GetSimFrameInterval();
    if (g_simFrameAcculator > interval)
    {
        g_simFrameAcculator -= interval;
        
        if (g_isRunningServer)
        {
            SV_Tick(interval);
        }

        if (g_isRunningClient)
        {
            CL_Tick(interval);
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
