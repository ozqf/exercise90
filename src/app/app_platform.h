#pragma once

#include "stdlib.h"
#include "../interface/sys_events.h"

/***************************************
* Public (app.h)
***************************************/
void App_Log(char* msg)
{
    g_platform.Log(msg);
}

void App_Print(char* msg)
{
    g_platform.Print(msg);
}

void App_SetMouseMode(ZMouseMode mode)
{
	g_platform.SetMouseMode(mode);
}

void App_Error(char* msg, char* heading)
{
    g_platform.Error(msg, heading);
}

ScreenInfo App_GetScreenInfo()
{
	return g_screenInfo;
}

f32 App_GetSimFrameInterval()
{
    return (1.0f / g_simFrameRate);
}

i32 App_CalcTickInterval(f32 seconds)
{
    f32 result = seconds / App_GetSimFrameInterval();
    // round
    return (i32)(result + 0.5f);
}

/***************************************
* Private
***************************************/

internal f32 App_CalcInterpolationTime(f32 accumulator, f32 interval)
{
    return (accumulator / interval);
}

internal void App_Fatal(char* msg, char* heading)
{
	g_platform.Error(msg, heading);
}

/***************************************
* Define functions accessible to platform
***************************************/
internal i32  g_isValid = 0;

internal i32  App_Init()
{
    APP_LOG(128, "App initialising. Build data %s - %s\n", __DATE__, __TIME__);
    //App_Log("Test Log\n");
	
    //App_Win32_AttachErrorHandlers();
	COM_SetFatalError(App_Fatal);

    // Memory

    // Acquiring an old 'heap object here. Various platform functions
    // still use it for loading assets so can't remove. Future allocations
    // should just use the basic malloc tracker until further notice
    u32 heapMB = 64;
    u32 mainMemorySize = MegaBytes(heapMB);
    MemoryBlock mem = {};

    APP_LOG(128, "APP Requested %d MB for Heap\n", heapMB);

    if (!g_platform.Malloc(&mem, mainMemorySize))
    {
        APP_LOG(128, "APP Platform malloc failed\n");
		COM_ASSERT(0, "APP Platform malloc failed");
        return 0;
    }
    else
    {
        Heap_Init(&g_heap, mem.ptrMemory, mem.size);
    }

    ///////////////////////////////////////////////////////////////
    // Assets
    ///////////////////////////////////////////////////////////////
    Tex_Init(&g_heap, g_platform);
    char * textures[] = 
    {
        "textures\\BitmapTest.bmp",
        DEFAULT_CONSOLE_CHARSET_PATH,
        "textures\\brbrick2.bmp",
        "textures\\W33_5.bmp",
        "textures\\COMP03_1.bmp",
        "\0"
    };
    //g_platform.Error("Bar", "Foo");
    APP_PRINT(64, "App load texture list\n");
    Tex_LoadTextureList(textures);

    COM_InitEmbeddedAssets();

    // proc gen textures here
    //i32 texIndex = Tex_GetTextureIndexByName(sourceTextureName);
    //Texture2DHeader* h = &g_textureHandles.textureHeaders[texIndex];
    Texture2DHeader* h = Tex_GetTextureByName(DEFAULT_CONSOLE_CHARSET_PATH);
    //Tex_GenerateBW(h);

    ///////////////////////////////////////////////////////////////

    // must be init AFTER textures as it needs teh console char sheet
    App_DebugInit();

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
    
	g_loopbackSocket.Init(g_fakeLagMinMS, g_fakeLagMaxMS, g_fakeLoss);
    
    g_localServerAddress = {};
    g_localServerAddress.port = APP_SERVER_LOOPBACK_PORT;

    // Render Scenes - orient camera
    RScene_Init(&g_worldScene, g_worldSceneItems, MAX_WORLD_SCENE_ITEMS,
		90, RENDER_PROJECTION_MODE_3D, 8);
    g_worldScene.cameraTransform.pos.z = 10;
    g_worldScene.cameraTransform.pos.y += 34;
    Transform_SetRotation(&g_worldScene.cameraTransform, -(80.0f    * DEG2RAD), 0, 0);

    // server and client areas currently acquiring their own memory
    App_StartSession(APP_SESSION_TYPE_SINGLE_PLAYER);

    return COM_ERROR_NONE;
}

internal i32  App_Shutdown()
{
    APP_LOG(128, "App Shutdown\n");
    
    // Free memory, assuming a new APP might be loaded in it's place
    MemoryBlock mem = {};
    mem.ptrMemory = g_heap.ptrMemory;
    mem.size = g_heap.size;
    g_platform.Free(&mem);
	
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
    APP_LOG(128, "\n=== START SESSION ===\n");
    switch (sessionType)
    {
        case APP_SESSION_TYPE_SINGLE_PLAYER:
        {
            App_EndSession();
            APP_LOG(128, "\tStarting single player\n");
            /*ZNet_StartSession(
                g_serverNet,
                NETMODE_DEDICATED_SERVER,
                NULL,
                APP_SERVER_LOOPBACK_PORT);*/
            //g_localServerSocket.isActive = 1;
            SV_Init();
            UserIds ids = SVU_CreateLocalUser();
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
            APP_LOG(128, "Unknown Session type %d\n", sessionType);
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
    g_lastPlatformFrame = time->frameNumber;
	i32 inputBytes = commands.Written();
	//printf("APP input %d bytes\n", inputBytes);
	
    u8* read = commands.ptrStart;
    u8* end = commands.ptrWrite;
    while (read < end)
    {
        SysEvent* header = (SysEvent*)read;
        COM_ASSERT(
			Sys_ValidateEvent(header) == COM_ERROR_NONE,
			"Error reading system event header")
        read += header->size;
		
        switch (header->type)
        {
            case SYS_EVENT_INPUT:
            {
                SysInputEvent* ev = (SysInputEvent*)header;
                //printf("APP input Event: %d value %d\n", ev->inputID, ev->value);
                ByteBuffer* b = g_clientLoopback.GetWrite();
                b->ptrWrite += COM_COPY(header, b->ptrWrite, header->size);
            } break;
            case SYS_EVENT_PACKET:
            {
                // TODO: Load into loopbacks
            } break;

            case SYS_EVENT_SKIP: break;

            default:
            {
                APP_LOG(128, "APP Unknown sys event type %d size %d\n", header->type, header->size);
            } break;
        }
    }
}

internal void App_SimFrame(f32 interval)
{
    #if APP_DEBUG_LOG_FRAME_TIMING
        APP_LOG(64, "App Sim tick %.8f\n", interval);
    #endif
    App_UpdateLoopbackSocket(&g_loopbackSocket, interval);
    if (g_isRunningServer)
    {
        g_serverLoopback.Swap();
	    Buf_Clear(g_serverLoopback.GetWrite());
        SV_Tick(g_serverLoopback.GetRead(), interval);
    }
    if (g_isRunningClient)
    {
        g_clientLoopback.Swap();
        Buf_Clear(g_clientLoopback.GetWrite());
        CL_Tick(g_clientLoopback.GetRead(), interval, g_lastPlatformFrame);
    }
}

internal void App_Update(PlatformTime* time)
{
    g_simFrameAcculator += time->deltaTime;
    f32 interval = App_GetSimFrameInterval();
    #if APP_DEBUG_LOG_FRAME_TIMING
        APP_LOG(128, "App Update DT: %.8f Accumulator: %.8f Interval: %.8f\n",
            time->deltaTime,
            g_simFrameAcculator,
            interval
            );
    #endif
    #if 1
    while (g_simFrameAcculator > interval)
    {
        APP_LOG(64, "FRAME\n");
        g_simFrameAcculator -= interval;
		App_SimFrame(interval);
    }
    #endif
    #if 0
    if (g_simFrameAcculator > interval)
    {
        g_simFrameAcculator -= interval;
		App_SimFrame(interval);
    }
    #endif
}

#if 0
// offset blocks of render objects left or right to show SV and CL side by side
internal void App_OffsetRenderObjects(RenderScene* scene, i32 firstItem, f32 x)
{
    for (u32 i = (u32)firstItem; i < scene->numObjects; ++i)
    {
        RenderListItem* item = &scene->sceneItems[i];
        item->transform.pos.x += x;
    }
}
#endif

internal void App_Render(PlatformTime* time, ScreenInfo info)
{
	g_screenInfo = info;
    char* texName = "textures\\white_bordered.bmp";
    //char* texName = "textures\\W33_5.bmp";
    i32 texIndex = Tex_GetTextureIndexByName(texName);
    f32 interval = App_GetSimFrameInterval();
    f32 interpolationTime = App_CalcInterpolationTime(
        g_simFrameAcculator, interval);
    #if APP_DEBUG_LOG_FRAME_TIMING
        APP_LOG(128, "APP Interpolate: %.8f Accumulator %.8f interval: %.8f\n",
                interpolationTime,
                g_simFrameAcculator,
                interval
            );
    #endif
    #if 0 // New route
    RenderCommand* cmds;
    i32 numCommands;
    CL_GetRenderCommands(&cmds, &numCommands, texIndex, interpolationTime);

    g_platform.SubmitRenderCommands(cmds, numCommands);

    #endif

    #if 1 // Old route
    
    // Have to remember to do this or things explode:
    g_worldScene.numObjects = 0;

    SV_PopulateRenderScene(
        &g_worldScene, g_worldScene.maxObjects, texIndex, 1,
        g_debugDrawServerScene, g_debugDrawServerTests);
    
    CL_PopulateRenderScene(
        &g_worldScene.cameraTransform,
        &g_worldScene, g_worldScene.maxObjects, texIndex, interpolationTime);
    
    g_platform.RenderScene(&g_worldScene);

    App_WriteDebugStrings();
    
    g_platform.RenderScene(&g_debugScene);
    #endif
}

internal u8 App_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
    if (numTokens == 2 && !COM_CompareStrings(tokens[0], "DRAW"))
    {
        if (!COM_CompareStrings(tokens[1], "SV"))
        {
            g_debugDrawServerScene = !g_debugDrawServerScene;
        }
        return 1;
    }
    if (numTokens == 4 && !COM_CompareStrings(tokens[0], "LAG"))
    {
        i32 minMS = COM_AsciToInt32(tokens[1]);
        i32 maxMS = COM_AsciToInt32(tokens[2]);
        f32 loss = (f32)COM_AsciToInt32(tokens[3]) / 100.0f;
        g_loopbackSocket.SetLagStats(minMS, maxMS, loss);
    }
    if (g_isRunningServer && SV_ParseCommandString(str, tokens, numTokens)) { return 1; }
    if (g_isRunningClient && CL_ParseCommandString(str, tokens, numTokens)) { return 1; }
    return 0;
}

/***************************************
* Export Windows DLL functions
***************************************/
#include <Windows.h>

extern "C"
AppInterface __declspec(dllexport) LinkToApp(AppPlatform platInterface)
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
