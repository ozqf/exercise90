/*
APP MAIN - base level of the game DLL,
holding game/menu state and calling game update when required
*/
#pragma once

#include "app_module.h"

void App_SwapGameCommandBuffers();

Var* App_GetVar(char* name)
{
    return COM_GetVar(g_vars, g_nextVar, name);
}

i32 App_GetVarAsI32(char* name)
{
	i32 result = 0;
    Var* v = App_GetVar(name);
	if (v != NULL)
	{
		result = COM_AsciToInt32(v->value);
	}
    else
    {
        printf("No var %s\n", name);
    }
    return result;
}

void App_InitVars()
{
    COM_SetVarByString("i.sensitivity", "20", g_vars, &g_nextVar, MAX_VARS);
    COM_SetVarByString("i.inverted", "1", g_vars, &g_nextVar, MAX_VARS);
}

void App_LoadDataVariables(char* fileName)
{
    BlockRef ref = {};
    if (!platform.Platform_LoadFileIntoHeap(&g_heap, &ref, fileName, 0))
    {
        printf(" Failed to open config ini\n");
        return;
    }
    printf("  Reading config.ini\n");
    COM_ReadVariablesBuffer((char*)ref.ptrMemory, ref.objectSize, g_vars, &g_nextVar, MAX_VARS);
    Heap_Free(&g_heap, ref.id);
}

void App_SaveDataVariables(char* fileName)
{
    const i32 lineBufferSize = 512;
    char lineBuffer[lineBufferSize];

    i32 fileId = platform.Platform_OpenFileForWriting(fileName);
    if (fileId == -1)
    {
        printf("  Failed to open %s for writing\n", fileName);
        return;
    }

    i32 numWritten = 0;

    numWritten = sprintf_s(lineBuffer, lineBufferSize, "# --- EX90 vars ---\n# Saved %s\n", __DATE__);
    platform.Platform_WriteToFile(fileId, (u8*)lineBuffer, numWritten);

    for (i32 i = 0; i < g_nextVar; ++i)
    {
        Var* v = &g_vars[i];
        numWritten = sprintf_s(lineBuffer, lineBufferSize, "%s=%s\n", v->name, v->value);
        platform.Platform_WriteToFile(fileId, (u8*)lineBuffer, numWritten);
    }

    platform.Platform_CloseFileForWriting(fileId);
}

void App_FatalError(char* message, char* heading)
{
    platform.Platform_Error(message, heading);
}

void App_PhysicsErrorHandler(char* message)
{
    platform.Platform_Error(message, "Physics Error");
}

////////////////////////////////////////////////////////////////////////////
// App Interface
////////////////////////////////////////////////////////////////////////////

void App_RunPostInitCommands()
{
	// TODO: This will EXEC straight away, which we don't want!
	App_StartSession(
        NETMODE_SINGLE_PLAYER, APP_FIRST_MAP,
        GetSession(), GetScene());
    //platform.Platform_WriteTextCommand("LOAD TEST");
	//platform.Platform_WriteTextCommand("IMPULSE 1");
	//platform.Platform_WriteTextCommand("MENU CLOSE");
}

i32 App_Init()
{
    App_Win32_AttachErrorHandlers();
    //DebugBreak();
    u32 heapMB = 64;
    u32 mainMemorySize = MegaBytes(heapMB);
    MemoryBlock mem = {};

    printf("APP Requested %d MB for Heap\n", heapMB);

    if (!platform.Platform_Malloc(&mem, mainMemorySize))
    {
        printf("APP Platform malloc failed\n");
        Assert(false);
        return 0;
    }
    else
    {
        Heap_Init(&g_heap, mem.ptrMemory, mem.size);
    }
    
    App_InitVars();
    printf(" Num data vars: %d\n", g_nextVar);
    App_LoadDataVariables("config.ini");

    //AllocateDebugStrings(&g_heap);
    //AllocateTestStrings(&g_heap);

    // Buffers, enlarge if necessary
    Heap_Allocate(&g_heap, &g_appBufferA_Ref, MegaBytes(4), "App Buffer A", 1);
    g_appBufferA = Heap_RefToByteBuffer(&g_heap, &g_appBufferA_Ref);
    Heap_Allocate(&g_heap, &g_appBufferB_Ref, MegaBytes(4), "App Buffer B", 1);
    g_appBufferB = Heap_RefToByteBuffer(&g_heap, &g_appBufferB_Ref);

    Heap_Allocate(&g_heap, &g_serverStreamInputRef, KiloBytes(64), "Server Input", 1);
    Heap_Allocate(&g_heap, &g_serverStreamOutputRef, KiloBytes(64), "Server Output", 1);
    g_serverStreamInput = Heap_RefToByteBuffer(&g_heap, &g_serverStreamInputRef);
    g_serverStreamOutput = Heap_RefToByteBuffer(&g_heap, &g_serverStreamOutputRef);
    g_serverStream.inputBuffer = g_serverStreamInput;
    g_serverStream.outputBuffer = g_serverStreamOutput;

    g_appWriteBuffer = &g_appBufferA;
    g_appReadBuffer = &g_appBufferB;
    printf("APP init write Buffer: %s\n",
        App_GetBufferName(g_appWriteBuffer->ptrStart));
    printf("APP init read Buffer: %s\n",
        App_GetBufferName(g_appReadBuffer->ptrStart));

    // 64KB = roughly 760 or so shape updates max per frame. (update being 84B due to being an entire matrix)
    Heap_Allocate(&g_heap, &g_collisionEventBuffer, MegaBytes(4), "Collision EV", 1);
    Heap_Allocate(&g_heap, &g_collisionCommandBuffer, MegaBytes(4), "Collision CMD", 1);

    SharedAssets_Init();
    Assets_InitMeshDB();

    BlockRef ref = {};

    //g_numDebugTextures = platform.Platform_LoadDebugTextures(&g_heap);
    g_textureHandles.numTextures = 0;
    AppLoadTestTextures();

    PhysExt_Init(
        g_collisionCommandBuffer.ptrMemory,
        g_collisionCommandBuffer.objectSize,
        g_collisionEventBuffer.ptrMemory,
        g_collisionEventBuffer.objectSize,
        App_PhysicsErrorHandler
        );
	
    // Init network functions
    ZNetPlatformFunctions netFuncs = {};
	netFuncs.Init = platform.Net_Init;
    netFuncs.Shutdown = platform.Net_Shutdown;
    netFuncs.OpenSocket = platform.Net_OpenSocket;
    netFuncs.CloseSocket = platform.Net_CloseSocket;
    netFuncs.Read = platform.Net_Read;
    netFuncs.SendTo = platform.Net_SendTo;
    netFuncs.FatalError = App_FatalError;
	ZNet_Init(
        netFuncs,
        Net_CreateOutputInterface(),
        //ZNET_SIM_MODE_NONE);
        ZNET_SIM_MODE_REALISTIC);
        //ZNET_SIM_MODE_BAD);
    
    
    Game_Init();
    Game_InitDebugStr();

    // Render Scenes
    RScene_Init(&g_worldScene, g_scene_renderList, GAME_MAX_ENTITIES);
    RScene_Init(&g_weaponModelScene, g_weaponModel_renderList, GAME_MAX_ENTITIES);
    RScene_Init(&g_uiScene, g_ui_renderList, UI_MAX_ENTITIES,
                 90,
                 RENDER_PROJECTION_MODE_IDENTITY,
                 //RENDER_PROJECTION_MODE_ORTHOGRAPHIC,
                 8);
    //
    App_InitInput(&g_inputActions);
    App_InitMenus();
    App_RunPostInitCommands();
    
    return 1;
}

i32 App_Shutdown()
{
    printf("APP DLL Shutdown\n");

    App_StopRecording();
	
    GS_Clear(GetScene());
    PhysExt_Shutdown();
    ZNet_Shutdown();

    // Free memory, assuming a new APP might be loaded in it's place
    MemoryBlock mem = {};
    mem.ptrMemory = g_heap.ptrMemory;
    mem.size = g_heap.size;
    platform.Win32_Free(&mem);

    return 1;
}

void App_ClearIOBuffers()
{
	COM_ZeroMemory(g_appReadBuffer->ptrStart, g_appReadBuffer->capacity);
	COM_ZeroMemory(g_appWriteBuffer->ptrStart, g_appWriteBuffer->capacity);
}

////////////////////////////////////////////////////////////////////////////
// INPUT
////////////////////////////////////////////////////////////////////////////

void App_ReadInputItem(InputItem *item, i32 value, u32 frameNumber)
{
    if (item->on != value)
    {
        item->on = (u8)value;
        item->lastChangeFrame = frameNumber;
    }
}




///////////////////////////////////////////////////////////////////////////////
// GAME TICK
 //////////////////////////////////////////////////////////////////////////////
void App_UpdateGameScene(GameTime* time)
{
    Game game;
    game.scene = GetScene();
    game.session = GetSession();

    // Write local client input commands
    App_UpdateLocalClients(time, GetClientList());
    
    // clear debug buffer
    g_debugStr.length = 0;

    // Prepare input buffer
    // aaaargh how confusing is this?!?
    // buffers have not swapped yet
	ByteBuffer* writeBuf = g_appWriteBuffer;
	
	
	//////////////////////////////////////////////
	// demo buffer
	//////////////////////////////////////////////
	
	ByteBuffer replayBuffer = {};
    if (g_replayMode == RecordingReplay)
    {
        // > Write frame header
        // > Write contents of read buffer into demo file
        u32 bytesInBuffer = (u32)writeBuf->ptrEnd - (u32)writeBuf->ptrStart;
        ReplayFrameHeader h = {};
		COM_CopyStringLimited("FRAME", h.label, 16);
        h.frameNumber = time->gameFrameNumber;
        h.size = bytesInBuffer;
        if (time->singleFrame)
        {
            printf("APP writing demo frame %d to file (%d bytes)\n",
                h.frameNumber, h.size);
        }
        g_replayHeader.frames.count++;
        g_replayHeader.frames.size += sizeof(ReplayFrameHeader) + bytesInBuffer;
        platform.Platform_WriteToFile(g_replayFileId, (u8*)&h, sizeof(ReplayFrameHeader));
        platform.Platform_WriteToFile(g_replayFileId, writeBuf->ptrStart, bytesInBuffer);
    }
    else if (g_replayMode == PlayingReplay && g_replayPtr != NULL)
    {
        // ignore g_appReadBuffer and instead read a frame from
        // the demo file
        ReplayFrameHeader h = {};
		u8* readStart = g_replayPtr;
		g_replayPtr += COM_COPY_STRUCT(g_replayPtr, &h, ReplayFrameHeader);
		
		if (COM_CompareStrings(h.label, "FRAME") == 1)
		{
			printf("APP Invalid frame header at %d\n", (u32)readStart);
		}
		else if(time->singleFrame)
		{
			printf("APP Reading demo frame %d (%d bytes)\n",
				h.frameNumber,
				h.size
			);
		}
		
        // Set input buffer
		replayBuffer.ptrStart = g_replayPtr;
		replayBuffer.ptrEnd = g_replayPtr + h.size;
		writeBuf = &replayBuffer;

        // Step read forward
        g_replayPtr += h.size;

        if (g_replayPtr >= g_replayReadBuffer.ptrEnd)
        {
            printf(">> End of replay <<\n");
            g_replayPtr = NULL;
            platform.Platform_WriteTextCommand("STOP");
            // debug loop this demo
            //platform.Platform_WriteTextCommand("PLAY DEMO2.DEM");
        }
    }
	//////////////////////////////////////////////

	if (time->singleFrame)
	{
		printf("-- GAME INPUT --\n");
		App_PrintCommandBufferManifest(writeBuf->ptrStart, (u16)writeBuf->Written());
	}
	
    //////////////////////////////////////////////
    // Game state update
    //////////////////////////////////////////////
    App_SwapGameCommandBuffers();
    Game_Tick(&game,
              g_appReadBuffer,
              g_appWriteBuffer,
              time,
              &g_inputActions);

    g_appReadBuffer->ptrEnd = g_appReadBuffer->ptrWrite;

    time->sessionEllapsed += time->deltaTime;
}





///////////////////////////////////////////////////////////////////////////////
// RENDER
///////////////////////////////////////////////////////////////////////////////
void App_Render(GameTime* time, ScreenInfo screenInfo)
{
    ClientList* clients = GetClientList();
    GameSession* session = GetSession();
    GameScene *gs = GetScene();
    g_screenInfo = screenInfo;
    
    // Make sure  render lists have been cleared or bad stuff will happen
    g_worldScene.numObjects = 0;
    g_weaponModelScene.numObjects = 0;
    g_uiScene.numObjects = 0;
    g_menuScene.numObjects = 0;

    // Camera selection
    if (g_debugCameraOn)
    {
        Game_ApplyInputFlyMode(
            &g_inputActions,
            &g_debugCameraDegrees,
            &g_debugCameraTransform,
            time->deltaTime,
            time->platformFrameNumber
            );
        g_worldScene.cameraTransform = g_debugCameraTransform;
    }
    else
    {
        g_worldScene.cameraTransform = gs->cameraTransform;
    }
    
    Game_BuildRenderList(gs, &g_worldScene, time);
    if (g_debugColliders == 2 || (g_debugColliders == 1 && g_debugCameraOn))
    {
        Game_DrawColliderAABBs(gs, time, &g_worldScene);
    }

    Game_IntersectionTest(gs, &g_worldScene);

    // Set lights and render
    //g_worldScene.settings.lightBits |= (1 << 0);
    platform.Platform_RenderScene(&g_worldScene);

    Client* cl = App_FindLocalClient(clients, 1);
    if (cl && !g_debugCameraOn)
    {
        Game_BuildWeaponModelScene(cl, gs, &g_weaponModelScene);
        platform.Platform_RenderScene(&g_weaponModelScene);
    }

    #if 1
    Game_UpdateUI(cl, gs, GetUIEntities(), UI_MAX_ENTITIES, time);
    //App_BuildMenuRenderScene()
    UI_BuildUIRenderScene(&g_uiScene, GetUIEntities(), UI_MAX_ENTITIES);


    // Render debug string
    //App_WriteCameraDebug(time);

    g_debugStr = App_WriteDebugString(session, gs, time);

    Game_SetDebugStringRender();
    Transform t = {};
    t.pos.x = -1;// (-1 - 0.05f);
    t.pos.y = 1;
    // t.pos.x = -1;
    // t.pos.y = 1;
    RScene_AddRenderItem(&g_uiScene, &t, &g_debugStrRenderer);
    platform.Platform_RenderScene(&g_uiScene);
    #endif

    // Paused message and actual menus are different items
    // displayed at different times, but always over the game and HUD
    if (g_menuOn)
    {
        App_MenuInput(&g_inputActions, time, &g_screenInfo);
        App_BuildMenuRenderScene();
        platform.Platform_RenderScene(&g_menuScene);
    }
    
}

void App_SwapGameCommandBuffers()
{
    //printf("APP Swap buffers R <-> W\n");
    ByteBuffer* tempReadBuffer = g_appReadBuffer;
    g_appReadBuffer = g_appWriteBuffer;
    g_appWriteBuffer = tempReadBuffer;
    //printf("APP write Buffer: %s\n", App_GetBufferName(g_appWriteBuffer->ptrStart));
    //printf("APP read Buffer: %s\n", App_GetBufferName(g_appReadBuffer->ptrStart));
	
    g_appWriteBuffer->ptrWrite = g_appWriteBuffer->ptrStart;
    g_appWriteBuffer->ptrEnd = g_appWriteBuffer->ptrStart;
    // Zeroing unncessary, just mark first byte null incase nothing is written for some reason
    COM_ZeroMemory(g_appWriteBuffer->ptrWrite, g_appWriteBuffer->capacity);
    //*g_appWriteBuffer->ptrWrite = NULL;
	
}

///////////////////////////////////////////////////////////////////////////////
// PLATFORM INPUT
///////////////////////////////////////////////////////////////////////////////
void App_ReadInputEvents(GameTime* time, ByteBuffer platformCommands)
{
	// Increment internal platform frame number here!
	g_time.platformFrameNumber = time->platformFrameNumber;
    
    App_ReadPlatformCommandBuffer(&platformCommands);
    
    // Local debugging. Not command related
    if (Input_CheckActionToggledOn(&g_inputActions, "Cycle Debug", time->platformFrameNumber))
    {
        // Ye gads will he every figure out matrix maths...?
        g_worldScene.settings.viewModelMode++;
    }
    if (Input_CheckActionToggledOn(&g_inputActions, "Menu", time->platformFrameNumber))
    {
        platform.Platform_WriteTextCommand("MENU ESCAPE");
        // Input_ToggleMouseMode();
        // g_menuOn = !g_menuOn;
    }
    // if (Input_CheckActionToggledOn(&g_inputActions, "Pause", time->platformFrameNumber))
    // {
        // g_paused = !g_paused;
        // printf("PAUSED: %d\n", g_paused);
    // }
}

void App_Frame(GameTime *time)
{
    // TODO: Fix pause functionality - currently blocks Net_Tick which means no net I/O!
	//if (g_paused || g_minimised) { return; }
	
    g_time = *time;
    GameSession* session = GetSession();
    GameScene* gs = GetScene();

    Net_ReadPackets(session, gs, time);
    Net_ReadInputStreams(session, gs, time);
	
	App_UpdateGameScene(time);
	Net_Transmit(session, gs, time);
    
    if (time->singleFrame)
	{
		u32 bytesRead = g_appReadBuffer->ptrWrite - g_appReadBuffer->ptrStart;
		u32 bytesWritten = g_appWriteBuffer->ptrWrite - g_appWriteBuffer->ptrStart;
		printf("APP frame end. Read %d bytes, wrote %d bytes\n", bytesRead, bytesWritten);
	}
	
	// Mark end of write buffer ready for reading next frame
	g_appWriteBuffer->ptrEnd = g_appWriteBuffer->ptrWrite;
	if (time->singleFrame)
	{
		printf("Marked end of write buffer, %d bytes\n",
            (g_appWriteBuffer->ptrEnd - g_appWriteBuffer->ptrStart));
		App_PrintCommandBufferManifest(g_appWriteBuffer->ptrStart, (u16)g_appWriteBuffer->Written());
	}

	// Used to swap Command Buffers here for... some reason?
    
	///////////////////////////
    // End of Frame.
	///////////////////////////
    i32 error;
	switch (g_appStateOperation.op)
	{
        case APP_STATE_OP_NONE: { } break;
		case APP_STATE_OP_SAVE:
		{
			StateSaveHeader h = {};
			App_WriteStateToFile(
                session, gs,
                g_appStateOperation.fileName, true, &h);
		} break;
		case APP_STATE_OP_LOAD:
		{
			error = App_StartSession(
                NETMODE_SINGLE_PLAYER, g_appStateOperation.fileName,
                session, gs);
            if (error)
			{
				printf("Failed to load game: Code %d\n", error);
			}
		} break;
        case APP_STATE_OP_HOST:
		{
			error = App_StartSession(
                NETMODE_LISTEN_SERVER, g_appStateOperation.fileName,
                session, gs);
			if (error)
			{
				printf("Failed to load game: Code %d\n", error);
			}
		} break;
        case APP_STATE_OP_JOIN:
		{
			error = App_StartSession(
                NETMODE_CLIENT, g_appStateOperation.fileName,
                session, gs);
			if (error)
			{
				printf("Failed to load game: Code %d\n", error);
			}
		} break;
        case APP_STATE_OP_EMPTY:
        {
            error = App_StartSession(
                NETMODE_NONE, g_appStateOperation.fileName,
                session, gs);
            if (error)
			{
				printf("Failed to switch to empty state: Code %d\n", error);
			}
        } break;
		case APP_STATE_OP_RECORD:
		{
			ILLEGAL_CODE_PATH
		} break;
		case APP_STATE_OP_PLAY:
		{
			ILLEGAL_CODE_PATH
		} break;
	}
	g_appStateOperation.op = APP_STATE_OP_NONE;
    if (gs->verboseFramesTick > 0)
    {
        gs->verboseFramesTick--;
    }

    time->gameFrameNumber++;
}
