/*
APP MAIN - base level of the game DLL,
holding game/menu state and calling game update when required
*/
#pragma once

#include "app_main.h"
#include "../common/com_module.h"

#include "app_testTextures.h"

#include <stdio.h>

// TODO: STILL USING WINDOWS INCLUDE FOR DEBUGGING. NOT PLATFORM AGNOSTIC!
#include "../Platform/win32_system_include.h"

void App_ErrorStop()
{
    DebugBreak();
}

void R_Scene_Init(RenderScene* scene, RenderListItem* objectArray, u32 maxObjects,
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

void R_Scene_Init(RenderScene* scene, RenderListItem* objectArray, u32 maxObjects)
{
    R_Scene_Init(scene, objectArray, maxObjects, 90, RENDER_PROJECTION_MODE_3D, 8);
}

void Input_SetMouseMode(ZMouseMode mode)
{
    g_mouseMode = mode;
    platform.Platform_SetMouseMode(mode);
}

void Input_ToggleMouseMode()
{
    if (g_mouseMode == Free)
    {
        Input_SetMouseMode(Captured);
    }
    else
    {
        Input_SetMouseMode(Free);
    }
}

void AllocateDebugStrings(Heap* heap)
{
	// Buffer used for live stat output
	Heap_AllocString(&g_heap, &g_debugBuffer, 1024);
	char* writeStart = (char*)g_debugBuffer.ptrMemory;
	i32 charsWritten = sprintf_s(writeStart, g_debugBuffer.objectSize, "Debug Test string alloc from\nline %d\nIn file %s\nOn %s\n", __LINE__, __FILE__, __DATE__);
	sprintf_s(writeStart + charsWritten, g_debugBuffer.objectSize - charsWritten, "This text is appended to the previous line and\ncontains a 10 here: %d", 10);
	OutputDebugStringA("Allocated debug string\n");
}

i32 AllocateTestStrings(Heap* heap)
{
	BlockRef ref = {};
    Heap_AllocString(&g_heap, &ref, 128);
    char* testStr1 = "This is a test string. It should get copied onto the heap";
	COM_CopyStringLimited(testStr1, (char*)ref.ptrMemory, ref.objectSize);

	ref = {};
	Heap_AllocString(&g_heap, &ref, 256);
	char* testStr2 = "Welcome to another test string. This one goes on and on and on and on and on and blooooody on. At most 256 though, no more, but possible less. Enough to require 256 bytes of capacity certainly. I mean, with more than that it would just break right? It'll go right off the end and just...";
	COM_CopyStringLimited(testStr2, (char*)ref.ptrMemory, ref.objectSize);
    return 1;
}

/**
 * Record that a texture has been loaded
 * > header information will be copied and updated
 * > BlockRef should be a Heap block for a Texture2DHeader!
 */
void AppRegisterTexture(Texture2DHeader* header, BlockRef* ref)
{
    i32 index = g_textureHandles.numTextures;
    header->index = index;
    g_textureHandles.textureHeaders[index] = *header;
    if (ref != NULL)
    {
        g_textureHandles.blockRefs[index] = *ref;
    }
    else
    {
        g_textureHandles.blockRefs[index] = {};
    }
    g_textureHandles.numTextures++;
}

/**
 * Upload a texture to the GPU
 */
void AppBindTexture(Texture2DHeader* header)
{
    platform.Platform_BindTexture(header->ptrMemory, header->width, header->height, header->index);
}

/**
 * Read a texture onto the Global Heap
 */
BlockRef AppLoadTexture(char* filePath)
{
    BlockRef ref = {};
    platform.Platform_LoadTexture(&g_heap, &ref, filePath);
    return ref;
}

/**
 * Read a texture onto the global heap and then immediately bind it
 */
void AppLoadAndRegisterTexture(char* filePath)
{
    BlockRef ref = AppLoadTexture(filePath);
    
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    Texture2DHeader* header = (Texture2DHeader*)ref.ptrMemory;
    AppRegisterTexture(header, &ref);
    //AppBindTexture((Texture2DHeader*)ref.ptrMemory, &ref);
}

/**
 * Upload all registered textures
 */
void AppBindAllTextures()
{
    i32 numTextures = g_textureHandles.numTextures;
    for (i32 i = 0; i < numTextures; ++i)
    {
        AppBindTexture(&g_textureHandles.textureHeaders[i]);
    }
}

void AppLoadTestTextures()
{
    BlockRef ref;
    Texture2DHeader* header;

    AppInitTestTextures();

    AppRegisterTexture(&testBuffer, NULL);
    
    AppRegisterTexture(&testBuffer2, NULL);
    
    AppRegisterTexture(&testBuffer3, NULL);
    
    ref = AppLoadTexture("BitmapTest.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader*)ref.ptrMemory;
    AppRegisterTexture(header, &ref);
    
    ref = AppLoadTexture("charset.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader*)ref.ptrMemory;
    AppRegisterTexture(header, &ref);
    
    ref = AppLoadTexture("brbrick2.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader*)ref.ptrMemory;
    AppRegisterTexture(header, &ref);
    
    ref = AppLoadTexture("BKEYA0.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader*)ref.ptrMemory;
    AppRegisterTexture(header, &ref);
    
    AppBindAllTextures();
}

i32 AppRendererReloaded()
{
    AppBindAllTextures();
    return 1;
}

////////////////////////////////////////////////////////////////////////////
// App Interface
////////////////////////////////////////////////////////////////////////////
i32 App_Init()
{
    printf("DLL Init\n");
    //DebugBreak();

    u32 mainMemorySize = MegaBytes(64);
    MemoryBlock mem = {};

    if (!platform.Platform_Malloc(&mem, mainMemorySize))
    {
        return 0;
    }
    else
    {
        Heap_Init(&g_heap, mem.ptrMemory, mem.size);
    }

	AllocateDebugStrings(&g_heap);
	//AllocateTestStrings(&g_heap);

    Heap_Allocate(&g_heap, &g_gameInputBuffer, 2048, "Game Input");
    Heap_Allocate(&g_heap, &g_gameOutputBuffer, 2048, "Game Output");

    Heap_Allocate(&g_heap, &g_collisionEventBuffer, 2048, "Collision EV");
    Heap_Allocate(&g_heap, &g_collisionCommandBuffer, 2048, "Collision CMD");

    SharedAssets_Init();

    BlockRef ref = {};


    //g_numDebugTextures = platform.Platform_LoadDebugTextures(&g_heap);
    g_textureHandles.numTextures = 0;
    AppLoadTestTextures();

    Phys_Init(
        g_collisionCommandBuffer.ptrMemory,
        g_collisionCommandBuffer.objectSize,
        g_collisionEventBuffer.ptrMemory,
        g_collisionEventBuffer.objectSize
        );
    
    Game_Init(&g_heap);
    Game_InitDebugStr();

    R_Scene_Init(&g_worldScene, g_scene_renderList, GAME_MAX_ENTITIES);
    R_Scene_Init(&g_uiScene, g_ui_renderList, UI_MAX_ENTITIES,
        90,
        RENDER_PROJECTION_MODE_IDENTITY,
        //RENDER_PROJECTION_MODE_ORTHOGRAPHIC,
        8
    );
    
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_V, "Cycle Debug");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_R, "Reset");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_ESCAPE, "Menu");

    Input_InitAction(&g_inputActions, Z_INPUT_CODE_A, "Move Left");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_D, "Move Right");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_W, "Move Forward");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_S, "Move Backward");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_SPACE, "Move Up");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_CONTROL, "Move Down");

    Input_InitAction(&g_inputActions, Z_INPUT_CODE_G, "Spawn Test");

    Input_InitAction(&g_inputActions, Z_INPUT_CODE_MOUSE_POS_X, "Mouse Pos X");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_MOUSE_POS_Y, "Mouse Pos Y");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_MOUSE_MOVE_X, "Mouse Move X");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_MOUSE_MOVE_Y, "Mouse Move Y");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_MOUSE_1, "Attack 1");
    Input_InitAction(&g_inputActions, Z_INPUT_CODE_MOUSE_2, "Attack 2");
    return 1;
}

i32 App_Shutdown()
{
    printf("DLL Shutdown\n");

    Game_Shutdown();

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

void R_Scene_Tick(GameTime* time, RenderScene* scene)
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

void App_ReadInputItem(InputItem* item, i32 value, u32 frameNumber)
{
   if (item->on != value)
   {
       item->on = (u8)value;
       item->lastChangeFrame = frameNumber;
   }
}

void App_ReadInput(GameTime* time, InputEvent ev)
{
    InputAction* action = Input_TestForAction(&g_inputActions, ev.value, ev.inputID, time->frameNumber);
    
}

void App_Frame(GameTime* time, ByteBuffer commands)
{
	/////////////////////////////////////////////////////
	// Read Command buffer
	/////////////////////////////////////////////////////
    i32 numRead = 0;
    i32 numSkipped = 0;
    u8* ptrRead = commands.ptrStart;
	u32 headerSize = sizeof(BufferItemHeader);
	u32 evSize = sizeof(InputEvent);

    //u8* ptrEventStart = ptrRead;
    while (ptrRead < commands.ptrEnd && (numRead + numSkipped) < commands.count)
    {
        BufferItemHeader header = {};
        ptrRead += COM_COPY(ptrRead, &header, BufferItemHeader);
        
        //u32 type = *(u32*)ptrRead;
        //ptrEventStart = ptrRead;
        switch (header.type)
        {
            case PLATFORM_EVENT_CODE_INPUT:
            {
                numRead++;
                InputEvent ev = {};
                ptrRead += COM_COPY(ptrRead, &ev, InputEvent);

                App_ReadInput(time, ev);

                // InputAction* action = Input_TestForAction(
                //     &g_inputActions,
                //     ev.value,
                //     ev.inputID,
                //     time->frameNumber);
            } break;

			case NULL:
			{
				// 0 == end here now
				ptrRead = commands.ptrEnd;
			} break;

			default:
			{
                // Item type is unknown. Just size the size the header specifies
                numSkipped++;
			} break;
        }
    }

    if (Input_CheckActionToggledOn(&g_inputActions, "Cycle Debug", time->frameNumber))
    {
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

    GameState* gs = &g_gameState;
    GameState* ui = &g_uiState;

    MemoryBlock collisionBuffer = {};
    Heap_GetBlockMemoryAddress(&g_heap, &g_collisionEventBuffer);
    collisionBuffer.ptrMemory = g_collisionEventBuffer.ptrMemory;
    collisionBuffer.size = g_collisionEventBuffer.objectSize;

    MemoryBlock commandBuffer = {};
    Heap_GetBlockMemoryAddress(&g_heap, &g_collisionCommandBuffer);
    commandBuffer.ptrMemory = g_collisionCommandBuffer.ptrMemory;
    commandBuffer.size = g_collisionCommandBuffer.objectSize;

    // Clear output buffer ready for game to write to
    ByteBuffer outBuf = Heap_RefToByteBuffer(&g_heap, &g_gameOutputBuffer);
    COM_ZeroMemory(outBuf.ptrStart, outBuf.capacity);
    outBuf.count = 0;

    // Game state update
    Game_Tick(gs,
        Heap_RefToByteBuffer(&g_heap, &g_gameInputBuffer),
        &outBuf,
        time,
        &g_inputActions);
    
    // How much output was written?
    u32 outputSize = outBuf.ptrWrite - outBuf.ptrEnd;
    u32 numItemsWritten = outBuf.count;
    
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
