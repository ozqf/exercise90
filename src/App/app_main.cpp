/*
APP MAIN - base level of the game DLL,
holding game/menu state and calling game update when required
*/
#pragma once

#include "app_module.cpp"

void App_StopRecording()
{
    if (g_replayFileId != -1)
    {
        platform.Platform_CloseFileForWriting(g_replayFileId);
        g_replayMode = NoReplayMode;
        g_replayFileId = -1;
    }
}

void App_StartRecording(GameState* gs)
{
    #if 1
	if (g_replayMode != None)
	{
		printf("APP already recording...\n");
		return;
	}

    // Allocate a temporary chunk to write to, then dump it all out into a file
    i32 capacity = MegaBytes(10);
    
    MemoryBlock mem = {};
    platform.Platform_Malloc(&mem, MegaBytes(10));
    ByteBuffer buf = Buf_FromMemoryBlock(mem);
    
    char* demoName = "demo.dem";

	g_replayMode = RecordingReplay;
	g_replayFileId = platform.Platform_OpenFileForWriting(demoName);
	StateSaveHeader h = {};
	h.magic[0] = 'S';
	h.magic[1] = 'A';
	h.magic[2] = 'V';
	h.magic[3] = 'E';
	COM_CopyStringLimited(g_currentSceneName, h.baseFile, 32);

    // step forward, write header later
    buf.ptrWrite += sizeof(StateSaveHeader);
	//platform.Platform_WriteToFile(g_replayFileId, (u8*)&h, sizeof(StateSaveHeader));
    
    printf("APP sizeof(EntityState): %d\n", sizeof(Cmd_EntityState));

    ///////////////////////////////
    // Write commands

    BufferItemHeader cmdHeader = {};
    
    // Record current position to calculate bytes written after loop
    u8* startOfDynamicCmds = buf.ptrWrite;
    h.dynamicEntities.offset = (u32)(buf.ptrWrite - buf.ptrStart);

    // ents
    i32 numEntities = gs->entList.count;
    for (i32 i = 0; i < numEntities; ++i)
    {
        Ent* e = &gs->entList.items[i];
        if (e->inUse != ENTITY_STATUS_IN_USE) { continue; }
        Cmd_EntityState s = {};
        Game_WriteEntityState(gs, e, &s);
        cmdHeader.type = CMD_TYPE_ENTITY_STATE;
        cmdHeader.size = sizeof(Cmd_EntityState);
        buf.ptrWrite += COM_COPY_STRUCT(&cmdHeader, buf.ptrWrite, BufferItemHeader);
        buf.ptrWrite += COM_COPY_STRUCT(&s, buf.ptrWrite, Cmd_EntityState);
        h.dynamicEntities.count++;
    }
    
    // write header
    h.dynamicEntities.size = (u32)(buf.ptrWrite - startOfDynamicCmds);
    
    // Demo frames marked as negative means demo frames should be expected
    // but the count is unknown. If the count is not set it could mean there
    // was a crash and the demo was never completed

    
    COM_COPY_STRUCT(&h, buf.ptrStart, sizeof(StateSaveHeader));

    u32 written = (u32)(buf.ptrWrite - buf.ptrStart);
    printf("APP Wrote %dKB to %s:\n", (written / 1024), demoName);
    printf("  BASE FILE: %s\n", h.baseFile);
    printf("  Dynamic Entities: offset %d count %d bytes %d\n", h.dynamicEntities.offset, h.dynamicEntities.count, h.dynamicEntities.size);


    // Open file and begin writing demo frames.


    platform.Platform_Free(&mem);
    #endif
}

void App_EndSession()
{
	printf("APP Ending session\n");
	
    App_StopRecording();

	Game_Shutdown(&g_gameState);
	Phys_ClearWorld();
	// Clear command buffer
	//App_ClearClients();
	// Break client links
	App_ClearClientGameLinks();
	COM_ZeroMemory((u8*)g_appWriteBuffer->ptrStart, g_appWriteBuffer->capacity);
}

char* g_bufferA_Name = "a";
char* g_bufferB_Name = "b";
char* g_unknown = "Unknown";

char* App_GetBufferName(u8* bufferStartAddr)
{
    if (bufferStartAddr == g_appBufferA.ptrStart)
    {
        return g_bufferA_Name;
    }
    if (bufferStartAddr == g_appBufferB.ptrStart)
    {
        return g_bufferB_Name;
    }
    return g_unknown;
}

void AllocateDebugStrings(Heap *heap)
{
    // Buffer used for live stat output
    Heap_AllocString(&g_heap, &g_debugBuffer, 1024);
    char *writeStart = (char *)g_debugBuffer.ptrMemory;
    i32 charsWritten = sprintf_s(writeStart, g_debugBuffer.objectSize, "Debug Test string alloc from\nline %d\nIn file %s\nOn %s\n", __LINE__, __FILE__, __DATE__);
    sprintf_s(writeStart + charsWritten, g_debugBuffer.objectSize - charsWritten, "This text is appended to the previous line and\ncontains a 10 here: %d", 10);
    printf("APP Allocated debug string\n");
}

i32 AllocateTestStrings(Heap *heap)
{
    BlockRef ref = {};
    Heap_AllocString(&g_heap, &ref, 128);
    char *testStr1 = "This is a test string. It should get copied onto the heap";
    COM_CopyStringLimited(testStr1, (char *)ref.ptrMemory, ref.objectSize);

    ref = {};
    Heap_AllocString(&g_heap, &ref, 256);
    char *testStr2 = "Welcome to another test string. This one goes on and on and on and on and on and blooooody on. At most 256 though, no more, but possible less. Enough to require 256 bytes of capacity certainly. I mean, with more than that it would just break right? It'll go right off the end and just...";
    COM_CopyStringLimited(testStr2, (char *)ref.ptrMemory, ref.objectSize);
    return 1;
}

u8 App_StartSinglePlayer(char* path)
{
	printf("APP Start single player session: %s\n", path);
    
	App_EndSession();

    if (!App_LoadStateFromFile(&g_gameState, path))
    {
        return 0;
    }
	COM_CopyStringLimited(path, g_currentSceneName, MAX_SCENE_NAME_CHARS);
	
	App_StartRecording(&g_gameState);

	// Spawn local client
	// Assign local client id.
    Cmd_ClientUpdate spawnClient = {};
    // assign local id directly...
    // TODO: Do local client Id assignment via network command
    g_localClientId = -1;
    spawnClient.clientId = g_localClientId;
    spawnClient.state = CLIENT_STATE_OBSERVER;
    App_WriteGameCmd((u8*)&spawnClient, CMD_TYPE_CLIENT_UPDATE, sizeof(Cmd_ClientUpdate));
    return 1;
}

void App_InitMenus()
{
    Transform t = {};
    Transform_SetToIdentity(&t);
    t.pos.y = 0.5;
    t.pos.z = -1;
    t.scale.x = 0.5;
    t.scale.y = 0.5;
    t.scale.x = 0.5;
    RendObj obj;
    RendObj_SetAsBillboard(&obj, 1, 1, 1, AppGetTextureIndexByName("textures\\ui_text_menu_title.bmp"));

    RScene_Init(&g_menuScene, g_menuRenderList, APP_MAX_MENU_ITEMS);
    RScene_AddRenderItem(&g_menuScene, &t, &obj);
    
}

////////////////////////////////////////////////////////////////////////////
// App Interface
////////////////////////////////////////////////////////////////////////////
i32 App_StartSession(u8 netMode, char* path)
{
    switch (netMode)
    {
        case NETMODE_SINGLE_PLAYER:
        {
            if (App_StartSinglePlayer(path))
            {
                g_gameState.netMode = NETMODE_SINGLE_PLAYER;
                return 1;
            }

        } break;

        default:
        {
            printf("APP Cannot start unknown netMode %d\n", netMode);
        } break;
    }

    return 0;
}

i32 App_Init()
{
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

    //AllocateDebugStrings(&g_heap);
    //AllocateTestStrings(&g_heap);

    // Buffers, enlarge if necessary
    Heap_Allocate(&g_heap, &g_appBufferA_Ref, KiloBytes(1024), "App Buffer A", 1);
    g_appBufferA = Heap_RefToByteBuffer(&g_heap, &g_appBufferA_Ref);
    Heap_Allocate(&g_heap, &g_appBufferB_Ref, KiloBytes(1024), "App Buffer B", 1);
    g_appBufferB = Heap_RefToByteBuffer(&g_heap, &g_appBufferB_Ref);

    g_appWriteBuffer = &g_appBufferA;
    g_appReadBuffer = &g_appBufferB;

    // 64KB = roughly 760 or so shape updates max per frame. (update being 84B due to being an entire matrix)
    Heap_Allocate(&g_heap, &g_collisionEventBuffer, KiloBytes(64), "Collision EV", 1);
    Heap_Allocate(&g_heap, &g_collisionCommandBuffer, KiloBytes(64), "Collision CMD", 1);

    SharedAssets_Init();

    BlockRef ref = {};

    //g_numDebugTextures = platform.Platform_LoadDebugTextures(&g_heap);
    g_textureHandles.numTextures = 0;
    AppLoadTestTextures();

    Phys_Init(
        g_collisionCommandBuffer.ptrMemory,
        g_collisionCommandBuffer.objectSize,
        g_collisionEventBuffer.ptrMemory,
        g_collisionEventBuffer.objectSize);
    
    
    Game_Init(&g_heap);
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
    
    App_StartSession(NETMODE_SINGLE_PLAYER, APP_FIRST_MAP);

    //App_DumpHeap();
    return 1;
}

i32 App_Shutdown()
{
    printf("APP DLL Shutdown\n");

    App_StopRecording();
	
    Game_Shutdown(&g_gameState);
    Phys_Shutdown();

    // Free memory, assuming a new APP might be loaded in it's place
    MemoryBlock mem = {};
    mem.ptrMemory = g_heap.ptrMemory;
    mem.size = g_heap.size;
    platform.Platform_Free(&mem);

    return 1;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE
////////////////////////////////////////////////////////////////////////////

void R_Scene_Tick(GameTime *time, RenderScene *scene)
{
#if 0
	RendObj* obj;
	i32 rotatingEntity = 5;

	obj = &scene->rendObjects[6];
	obj->transform.rot.y += 90 * time->deltaTime;

    
	obj = &scene->rendObjects[3];
	obj->transform.rot.y += 90 * time->deltaTime;
#endif
}

void CycleTestTexture()
{
#if 0
    RendObj* obj = g_game_rendObjects + 0;
    RendObj_ColouredMesh* rMesh = &obj->data.mesh;
	//DebugBreak();
    rMesh->textureIndex++;
    if (rMesh->textureIndex >= g_numDebugTextures)
    {
        rMesh->textureIndex = 0;
    }
    char buf[128];
        sprintf_s(buf, 128, "Cycled test texture to: %d\n", rMesh->textureIndex);
        OutputDebugString(buf);
#endif
}

void CycleTestAsciChar()
{
#if 0
    RendObj* obj = g_ui_rendObjects + 0;
    RendObj_Sprite* sprite = &obj->data.sprite;
    g_testAsciChar++;
    RendObj_CalculateSpriteAsciUVs(sprite, g_testAsciChar);
#endif
}

void App_ReadInputItem(InputItem *item, i32 value, u32 frameNumber)
{
    if (item->on != value)
    {
        item->on = (u8)value;
        item->lastChangeFrame = frameNumber;
    }
}

void App_UpdateGameState(GameTime* time)
{
    ///////////////////////////////////////
    // Process messages before game update
    ///////////////////////////////////////

    // Update local client input
    App_UpdateLocalClients(time);
    
    // Read game output from last frame + internally issued commands
    //App_ReadCommandBuffer(g_appReadBuffer);

    ///////////////////////////////////////
    // Process gamestate
    ///////////////////////////////////////

    // clear debug buffer
    g_debugStr.length = 0;

    GameState *gs = &g_gameState;
    GameState *ui = &g_uiState;

    MemoryBlock collisionBuffer = {};
    Heap_GetBlockMemoryAddress(&g_heap, &g_collisionEventBuffer);
    collisionBuffer.ptrMemory = g_collisionEventBuffer.ptrMemory;
    collisionBuffer.size = g_collisionEventBuffer.objectSize;

    MemoryBlock commandBuffer = {};
    Heap_GetBlockMemoryAddress(&g_heap, &g_collisionCommandBuffer);
    commandBuffer.ptrMemory = g_collisionCommandBuffer.ptrMemory;
    commandBuffer.size = g_collisionCommandBuffer.objectSize;

    // Prepare input buffer
#if 1
    ByteBuffer* input = g_appReadBuffer;
    if (g_replayMode == RecordingReplay)
    {
        // > Write frame header
        // > Write contents of read buffer into demo file
        u32 bytesInBuffer = (u32)input->ptrEnd - (u32)input->ptrStart;
        ReplayFrameHeader h = {};
        h.frameNumber = time->gameFrameNumber;
        h.size = bytesInBuffer;
        if (time->singleFrame)
        {
            printf("APP writing demo frame %d to file (%d bytes)\n", h.frameNumber, h.size);
        }
        platform.Platform_WriteToFile(g_replayFileId, (u8*)&h, sizeof(ReplayFrameHeader));
        platform.Platform_WriteToFile(g_replayFileId, input->ptrStart, bytesInBuffer);
    }
    else if (g_replayMode == PlayingReplay)
    {
        // ignore g_appReadBuffer and instead read a frame from
        // the demo file
        ReplayFrameHeader h = {};
    }
#endif

    // Game state update
    Game_Tick(gs,
              g_appReadBuffer,
              g_appWriteBuffer,
              time,
              &g_inputActions);

    g_appReadBuffer->ptrEnd = g_appReadBuffer->ptrWrite;
}

void App_Render(GameTime* time)
{
    GameState *gs = &g_gameState;
    GameState *ui = &g_uiState;

    // Make sure  render lists have been cleared or bad stuff will happen
    g_worldScene.numObjects = 0;
    g_weaponModelScene.numObjects = 0;
    g_uiScene.numObjects = 0;

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
    
    Game_BuildRenderList(gs, &g_worldScene);
    if (g_debugColliders == 2 || (g_debugColliders == 1 && g_debugCameraOn))
    {
        Game_DrawColliderAABBs(gs, time, &g_worldScene);
    }

    Game_IntersectionTest(gs, &g_worldScene);

    platform.Platform_RenderScene(&g_worldScene);

    if (gs->localPlayerHasEnt && !g_debugCameraOn)
    {
        Game_BuildWeaponModelScene(&g_weaponModelScene);
        platform.Platform_RenderScene(&g_weaponModelScene);
    }
    
    // TODO: Not actually building the menu scene yet.
    // Paused message and actual menus are different items
    // displayed at different times, but always over the game and HUD
    if (g_menuOn)
    {
        platform.Platform_RenderScene(&g_menuScene);
    }
    
    #if 1
    Game_UpdateUI(ui, time);
    Game_BuildRenderList(ui, &g_uiScene);
    // Render debug string
    //ZSTR_WriteChars(&g_debugStr, "Test testy\ntest test", 21);
    //App_WriteCameraDebug(time);

    g_debugStr = App_WriteDebugString(gs, time);

    Game_SetDebugStringRender();
    Transform t = {};
    t.pos.x = -1;// (-1 - 0.05f);
    t.pos.y = 1;
    // t.pos.x = -1;
    // t.pos.y = 1;
    RScene_AddRenderItem(&g_uiScene, &t, &g_debugStrRenderer);
    platform.Platform_RenderScene(&g_uiScene);
#endif

}

void App_ReadInputEvents(GameTime* time, ByteBuffer platformCommands)
{
	// Increment internal platform frame number here!
	g_time.platformFrameNumber = time->platformFrameNumber;
    
    App_ReadCommandBuffer(&platformCommands);
    
    // Local debugging. Not command related
    if (Input_CheckActionToggledOn(&g_inputActions, "Cycle Debug", time->platformFrameNumber))
    {
        // Ye gads will he every figure out matrix maths...?
        g_worldScene.settings.viewModelMode++;
    }
    if (Input_CheckActionToggledOn(&g_inputActions, "Menu", time->platformFrameNumber))
    {
        Input_ToggleMouseMode();
        g_menuOn = !g_menuOn;
    }
    if (Input_CheckActionToggledOn(&g_inputActions, "Pause", time->platformFrameNumber))
    {
        g_paused = !g_paused;
        printf("PAUSED: %d\n", g_paused);
    }
}

void App_Frame(GameTime *time)
{
    g_time = *time;

    //if (!g_paused)
    //{
        App_UpdateGameState(time);
        //time->frameNumber++;
    //}
    
    if (time->singleFrame)
	{
		u32 bytesRead = g_appReadBuffer->ptrWrite - g_appReadBuffer->ptrStart;
		u32 bytesWritten = g_appWriteBuffer->ptrWrite - g_appWriteBuffer->ptrStart;
		printf("APP frame end. Read %d bytes, wrote %d bytes\n", bytesRead, bytesWritten);
	}
    // End of Frame. Swap Command Buffers.
    ByteBuffer* temp = g_appReadBuffer;
    g_appReadBuffer = g_appWriteBuffer;
    g_appWriteBuffer = temp;
    g_appWriteBuffer->ptrWrite = g_appWriteBuffer->ptrStart;
    g_appWriteBuffer->ptrEnd = g_appWriteBuffer->ptrStart;
    // Zeroing unncessary, just mark first byte null incase nothing is written for some reason
    COM_ZeroMemory(g_appWriteBuffer->ptrWrite, g_appWriteBuffer->capacity);
    //*g_appWriteBuffer->ptrWrite = NULL;

    time->gameFrameNumber++;
}
