/*
APP MAIN - base level of the game DLL,
holding game/menu state and calling game update when required
*/
#pragma once

#include "app_module.cpp"

void App_ErrorStop()
{
    DebugBreak();
}

void App_EndSession()
{
	printf("APP Ending session\n");
	Game_Shutdown(&g_gameState);
	Phys_ClearWorld();
	// Clear command buffer
	//App_ClearClients();
	// Break client links
	App_ClearClientGameLinks();
	COM_ZeroMemory((u8*)g_gameOutputByteBuffer.ptrStart, g_gameOutputByteBuffer.capacity);

}

void R_Scene_Init(RenderScene *scene, RenderListItem *objectArray, u32 maxObjects,
                  i32 fov, i32 projectionMode, f32 orthographicHalfHeight)
{
    *scene = {};
    Transform_SetToIdentity(&scene->cameraTransform);
    scene->numObjects = 0;
    scene->maxObjects = maxObjects;
    scene->sceneItems = objectArray;
    scene->settings.fov = fov;
    scene->settings.projectionMode = projectionMode;
    scene->settings.orthographicHalfHeight = orthographicHalfHeight;
}

void R_Scene_Init(RenderScene *scene, RenderListItem *objectArray, u32 maxObjects)
{
    R_Scene_Init(scene, objectArray, maxObjects, 90, RENDER_PROJECTION_MODE_3D, 8);
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

	// Spawn local client
	// Assign local client id.
    Cmd_ClientUpdate spawnClient = {};
    // assign local id directly...
    // TODO: Do local client Id assignment via network command
    g_localClientId = -1;
    spawnClient.clientId = g_localClientId;
    spawnClient.state = CLIENT_STATE_OBSERVER;
    App_EnqueueCmd((u8*)&spawnClient, CMD_TYPE_CLIENT_UPDATE, sizeof(Cmd_ClientUpdate));
    return 1;
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

    AllocateDebugStrings(&g_heap);
    //AllocateTestStrings(&g_heap);

    // Buffers, enlarge if necessary
    Heap_Allocate(&g_heap, &g_gameInputBufferRef, KiloBytes(64), "Game Input", 1);
    g_gameInputByteBuffer = Heap_RefToByteBuffer(&g_heap, &g_gameInputBufferRef);
    Heap_Allocate(&g_heap, &g_gameOutputBufferRef, KiloBytes(64), "Game Output", 1);
    g_gameOutputByteBuffer = Heap_RefToByteBuffer(&g_heap, &g_gameOutputBufferRef);

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

    R_Scene_Init(&g_worldScene, g_scene_renderList, GAME_MAX_ENTITIES);
    R_Scene_Init(&g_uiScene, g_ui_renderList, UI_MAX_ENTITIES,
                 90,
                 RENDER_PROJECTION_MODE_IDENTITY,
                 //RENDER_PROJECTION_MODE_ORTHOGRAPHIC,
                 8);
    //
    App_InitInput(&g_inputActions);
    
    //App_DumpHeap();
    return 1;
}

i32 App_Shutdown()
{
    printf("APP DLL Shutdown\n");

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

void App_Frame(GameTime *time, ByteBuffer commands)
{
    g_time = *time;

    /////////////////////////////////////////////////////
    // Read Command buffers
    /////////////////////////////////////////////////////

    // Read Platform commands (input + network?)
    App_ReadCommandBuffer(commands);
        
    // Update local client input
    App_UpdateLocalClients(time);
    
    // Read game output from last frame + internally issued commands
    App_ReadCommandBuffer(g_gameOutputByteBuffer);
	// Clear output buffer ready for write to this frame
    Buf_Clear(&g_gameOutputByteBuffer);


    // Local debugging. Not command related
    if (Input_CheckActionToggledOn(&g_inputActions, "Cycle Debug", time->frameNumber))
    {
        // Ye gads will he every figure out matrix maths...?
        g_worldScene.settings.viewModelMode++;
    }
    if (Input_CheckActionToggledOn(&g_inputActions, "Menu", time->frameNumber))
    {
        Input_ToggleMouseMode();
    }

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
    ByteBuffer inBuf = Heap_RefToByteBuffer(&g_heap, &g_gameInputBufferRef);

    // Game state update
    Game_Tick(gs,
              inBuf,
              &g_gameOutputByteBuffer,
              time,
              &g_inputActions);

    g_gameOutputByteBuffer.ptrEnd = g_gameOutputByteBuffer.ptrWrite;

    ///////////////////////////////////////
    // Render
    ///////////////////////////////////////

    // Make sure  render lists have been cleared or bad stuff will happen
    g_worldScene.numObjects = 0;
    g_uiScene.numObjects = 0;

    Game_BuildRenderList(gs, &g_worldScene);
    Game_DrawColliderAABBs(gs, time, &g_worldScene);

    Game_IntersectionTest(gs, &g_worldScene);

    platform.Platform_RenderScene(&g_worldScene);

#if 1
    Game_UpdateUI(ui, time);
    Game_BuildRenderList(ui, &g_uiScene);
    // Render debug string
    //ZSTR_WriteChars(&g_debugStr, "Test testy\ntest test", 21);
    //App_WriteCameraDebug(time);

    g_debugStr = Game_WriteDebugString(gs, time);

    Game_SetDebugStringRender();
    Transform t = {};
    t.pos.x = -1;
    t.pos.y = 1;
    // t.pos.x = -1;
    // t.pos.y = 1;
    RScene_AddRenderItem(&g_uiScene, &t, &g_debugStrRenderer);
    platform.Platform_RenderScene(&g_uiScene);
#endif
}

/***************************************
* Export DLL functions
***************************************/
#include "app_dll_export.cpp"
