/*
APP MAIN - base level of the game DLL,
holding game/menu state and calling game update when required
*/
#pragma once

#include "app_main.h"
#include "../Shared/shared_assets.h"
#include "../Shared/Memory/HeapUtils.h"

#include "app_testTextures.h"

#include <stdio.h>

// TODO: STILL USING WINDOWS INCLUDE FOR DEBUGGING. NOT PLATFORM AGNOSTIC!
#include "../Platform/win32_system_include.h"

void R_Scene_CreateTestScene()
{
    #if 0
    RenderListItem* item = g_scene_renderList;
    *item = {};
    Transform_SetToIdentity(&item->transform);
    RendObj_SetAsMesh(&item->obj, &g_meshInverseCube, 1, 1, 1, 5);
    item->transform.pos.x = 0;
    item->transform.pos.y = 0;
    item->transform.pos.z = 0;
    item->transform.scale.x = 6;
    item->transform.scale.y = 2;
    item->transform.scale.z = 6;
    g_worldScene.numObjects++;
    item++;
    
    RendObj* obj = g_game_rendObjects;

    // 0
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    obj->transform.pos.x = 0;
    obj->transform.pos.y = 0;
    obj->transform.pos.z = 0;
    //obj->transform.rot.x = 270;
    obj->transform.scale.x = 6;
    obj->transform.scale.y = 2;
    obj->transform.scale.z = 6;
    //g_meshPrimitive_quad
    //RendObj_SetAsColouredQuad(obj, 1, 0, 1);
    RendObj_SetAsMesh(obj, &g_meshInverseCube, 1, 1, 1, 5);
    g_scene.numObjects++;
    obj++;
    #if 1
    // 1
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    obj->transform.pos.x = 0;
    obj->transform.pos.y = 0;
    obj->transform.pos.z = 0;
    
    obj->transform.rot.x = 0;
    obj->transform.scale.x = 0.5;
    obj->transform.scale.y = 0.5;
    obj->transform.scale.z = 0.5;
    //g_meshPrimitive_quad
    //RendObj_SetAsColouredQuad(obj, 1, 0, 1);
    RendObj_SetAsMesh(obj, &g_meshOctahedron, 1, 1, 1, 5);
    //RendObj_SetAsMesh(obj, &g_meshCube, 1, 1, 1, 5);
    g_scene.numObjects++;
    obj++;

    // 2
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    obj->transform.scale.x = 4;
    obj->transform.rot.y = 90;
    obj->transform.pos.x = -4;
    RendObj_SetAsColouredQuad(obj, 0, 1, 0);
    g_scene.numObjects++;
    obj++;
    
    // 3
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    // obj->transform.rot.y = 270;
    obj->transform.pos.x = 1;
    // obj->transform.pos.z = -2;
    //RendObj_SetAsColouredQuad(obj, 0, 0, 1);
    RendObj_SetAsAABB(obj, 1, 2, 1, 1, 0, 0);
    g_scene.numObjects++;
    obj++;
    
    // 4
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    obj->transform.rot.y = 180;
    obj->transform.pos.z = 4;
    RendObj_SetAsColouredQuad(obj, 1, 1, 0);
    g_scene.numObjects++;
    obj++;

    // 5
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    obj->transform.rot.y = 0;
    obj->transform.pos.x = -2;
    obj->transform.pos.z = -3;
    RendObj_SetAsBillboard(obj, 1, 1, 1, 4);
    g_scene.numObjects++;
    obj++;
    
    // 6
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    obj->transform.rot.y = 0;
    obj->transform.pos.x = 3;
    obj->transform.pos.z = -4;
    //RendObj_SetAsColouredQuad(obj, 1, 0, 0);
    RendObj_SetAsMesh(obj, &g_meshCube, 1, 1, 1, 5);
    g_scene.numObjects++;
    obj++;
    
    // 7
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    obj->transform.rot.y = 0;
    obj->transform.pos.x = -3;
    obj->transform.pos.z = -2;
    RendObj_SetAsBillboard(obj, 1, 1, 1, 6);
    obj->transform.scale.x = 0.25;
    obj->transform.scale.y = 0.25;
    g_scene.numObjects++;
    obj++;
    #endif

    // 8
    *obj = {};
    Transform_SetToIdentity(&obj->transform);
    obj->transform.pos.x = 0;
    obj->transform.rot.y = 0;
    obj->transform.pos.z = 0;
    RendObj_SetAsLine(obj,
        -2, -2, -2, 2, 2, 2,
        1, 0, 0, 0, 1, 0);
    g_scene.numObjects++;
    obj++;
    

    /////////////////////////////////////////////////////////////////////////////
    // UI Tests
    obj = g_ui_rendObjects;

    // test sprite
    *obj = {};
    RendObj_SetAsSprite(obj, SPRITE_MODE_UI, 4, 0.1f, 0.1f);
    //RendObj_SetSpriteUVs(&obj->obj.sprite, 0.0625, 0.125, 0.5625, 0.5625 + 0.0625);
    RendObj_CalculateSpriteAsciUVs(&obj->data.sprite, g_testAsciChar);
    obj->transform.pos.z = -1;
    g_scene.numUIObjects++;
    obj++;

    // test string
    *obj = {};
	/*char* chars = g_testString;
	i32 numChars = COM_StrLenA(g_testString);*/

	char* chars = (char*)g_debugBuffer.ptrMemory;
	i32 numChars = g_debugBuffer.objectSize;

    RendObj_SetAsAsciCharArray(obj, chars, numChars, 0.05f);
    obj->transform.pos.x = -1;//-0.75f;
    obj->transform.pos.y = 1;//0.75f;
    g_scene.numUIObjects++;
    obj++;

    // test char
    // *obj = {};
    // RendObj_SetAsAsciChar(obj, '+');
    // g_scene.numUIObjects++;
    // obj++;
    #endif
}

void R_Scene_Init(RenderScene* scene, RenderListItem* objectArray, u32 maxObjects,
    i32 fov, i32 projectionMode, f32 orthographicHalfHeight)
{
    *scene = {};
    Transform_SetToIdentity(&scene->cameraTransform);
    scene->numObjects = 0;
    scene->maxObjects = maxObjects;
    scene->sceneItems = objectArray;
    scene->fov = fov;
    scene->projectionMode = projectionMode;
    scene->orthographicHalfHeight = orthographicHalfHeight;
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

void AppBindTexture(Texture2DHeader* header)
{
    //AssertAlways(g_textures.numTextures < g_textures.maxTextures);
    //g_textures.textureRefs[g_textures.numTextures++] = ref;
    //Texture2DHeader* header = (Texture2DHeader*)ref.ptrMemory;
    header->id = g_nextTextureIndex;
    platform.Platform_BindTexture(header->ptrMemory, header->width, header->height, g_nextTextureIndex++);
}

void AppLoadTexture(char* filePath)
{
    BlockRef ref = {};
    platform.Platform_LoadTexture(&g_heap, &ref, filePath);

    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    AppBindTexture((Texture2DHeader*)ref.ptrMemory);
    
    //AppRegisterTexture(ref);
}

void AppLoadTestTextures()
{
    AppInitTestTextures();
    AppBindTexture(&testBuffer);
    AppBindTexture(&testBuffer2);
    AppBindTexture(&testBuffer3);

    AppLoadTexture("base/Bitmaptest.bmp");
    AppLoadTexture("base/charset.bmp");
    AppLoadTexture("base/brbrick2.bmp");
    AppLoadTexture("base/BKEYA0.bmp");
}

////////////////////////////////////////////////////////////////////////////
// App Interface
// App_ == interface function
// Return 1 if successful, 0 if failed
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

    SharedAssets_Init();

    BlockRef ref = {};


    //g_numDebugTextures = platform.Platform_LoadDebugTextures(&g_heap);
    AppLoadTestTextures();
    Game_InitDebugStr();
    R_Scene_Init(&g_worldScene, g_scene_renderList, GAME_MAX_ENTITIES);
    R_Scene_Init(&g_uiScene, g_ui_renderList, UI_MAX_ENTITIES,
        90,
        RENDER_PROJECTION_MODE_IDENTITY,
        //RENDER_PROJECTION_MODE_ORTHOGRAPHIC,
        8
    );
    //R_Scene_CreateTestScene();
    
    testInput = {};
	testInput.speed = 3.0f;
    testInput.rotSpeed = 90.0f;
    
    Game_Init();

    return 1;
}

i32 App_Shutdown()
{
    printf("DLL Shutdown\n");

    // Free memory, assuming a new APP might be loaded in it's place
    MemoryBlock mem = {};
    mem.ptrMemory = g_heap.ptrMemory;
    mem.size = g_heap.size;
    platform.Platform_Free(&mem);

    return 1;
}

////////////////////////////////////////////////////////////////////////////
// Process Input
////////////////////////////////////////////////////////////////////////////
void Input_ApplyInputToTransform(InputTick* input, Transform* t, GameTime* time)
{
	testInput.movement = { 0, 0, 0 };
	testInput.rotation = { 0, 0, 0 };

	/////////////////////////////////////////////////
	// READ ROTATION
	/////////////////////////////////////////////////
	if (input->yawLeft) { testInput.rotation.y += 1; }
	if (input->yawRight) { testInput.rotation.y += -1; }

	if (input->pitchUp) { testInput.rotation.x += -1; }
	if (input->pitchDown) { testInput.rotation.x += 1; }

	if (input->rollLeft) { testInput.rotation.z += 1; }
	if (input->rollRight) { testInput.rotation.z += -1; }

    // test mouse input
    // if (input->attack1) { testInput.rotation.z += 1; }
	// if (input->attack2) { testInput.rotation.z += -1; }

	// x = pitch, y = yaw, z = roll
	f32 sensitivity = 0.1f;
	i8 inverted = -1;

#if 1
    // Disabled mouse input for debugging
	//t->rot.x += testInput.rotation.x * (testInput.rotSpeed * time->deltaTime);
	//t->rot.x -= (((f32)input->mouseMovement[1] * sensitivity) * time->deltaTime) * inverted;
    t->rot.x -= (((f32)input->mouseMovement[1] * sensitivity)) * inverted;
	t->rot.x = COM_CapAngleDegrees(t->rot.x);

	//t->rot.y += testInput.rotation.y * (testInput.rotSpeed * time->deltaTime);
	//t->rot.y -= ((f32)input->mouseMovement[0] * sensitivity) * time->deltaTime;
    t->rot.y -= ((f32)input->mouseMovement[0] * sensitivity);
	t->rot.y = COM_CapAngleDegrees(t->rot.y);

	t->rot.z += testInput.rotation.z * (testInput.rotSpeed * time->deltaTime);
	t->rot.z = COM_CapAngleDegrees(t->rot.z);
#endif

	if (input->moveLeft)
	{
		testInput.movement.x += -1;
	}
	if (input->moveRight)
	{
		testInput.movement.x += 1;
	}
	if (input->moveForward)
	{
		testInput.movement.z += -1;
	}
	if (input->moveBackward)
	{
		testInput.movement.z += 1;
	}

	if (input->moveDown)
	{
		testInput.movement.y += -1;
	}
	if (input->moveUp)
	{
		testInput.movement.y += 1;
	}

	/*
	Movement forward requires creating a vector in the direction of
	the object's "forward". Object in this case has rotation, so must convert
	rotation to a forward vector, and then scale this vector by desired speed
	to create a velocity change in the desired direction

	> Read rotation input and rotate angles
	> Calculate forward vector
	> Scale forward to desired speed and add to position

	Rotation Matrix to rotate on Z axis
	cos(theta),	-sin(theta),		0,			0,
	sin(theta),	cos(theta),			0,			0,
	0,			0,					1,			0,
	0,			0,					0,			1
	*/
	Vec3 frameMove = {};

	// Sideways: X Input
	// Vertical: Y Input
	// Forward: Z input

	// if (Vec3_Magnitude(&testInput.movement) == 0)
	// {
	// 	return;
	// }

	// Quick hack to force movement to occur on a flat x/z plane only
	Vec3 groundRot = t->rot;
	groundRot.x = 0;
	groundRot.z = 0;
#if 0
    AngleVectors* angleVectors = &g_worldScene.cameraAngleVectors;

	AngleToAxes(&groundRot, &angleVectors->left, &angleVectors->up, &angleVectors->forward);

	// Vec3 forward = t->forward;
	// Vec3 left = t->left;
	// Vec3 up = t->up;

	// If input is blank mag will be speed * 0?
	Vec3_SetMagnitude(&angleVectors->left, ((testInput.speed * time->deltaTime) * testInput.movement.x));
	Vec3_SetMagnitude(&angleVectors->up, ((testInput.speed * time->deltaTime) * testInput.movement.y));
	Vec3_SetMagnitude(&angleVectors->forward, ((testInput.speed * time->deltaTime) * testInput.movement.z));

	frameMove.x = angleVectors->forward.x + angleVectors->up.x + angleVectors->left.x;
	frameMove.y = angleVectors->forward.y + angleVectors->up.y + angleVectors->left.y;
	frameMove.z = angleVectors->forward.z + angleVectors->up.z + angleVectors->left.z;
#endif
#if 1

    AngleVectors angleVectors = {};

	Calc_AnglesToAxesZYX(&groundRot, &angleVectors.left, &angleVectors.up, &angleVectors.forward);


    // Set the cameras own
    // TODO: Clean this whole messy function up!
    AngleVectors* camVecs = &g_worldScene.cameraAngleVectors;
    Calc_AnglesToAxesZYX(&t->rot, &camVecs->left, &camVecs->up, &camVecs->forward);

	// Vec3 forward = t->forward;
	// Vec3 left = t->left;
	// Vec3 up = t->up;

	// If input is blank mag will be speed * 0?
	Vec3_SetMagnitude(&angleVectors.left, ((testInput.speed * time->deltaTime) * testInput.movement.x));
	Vec3_SetMagnitude(&angleVectors.up, ((testInput.speed * time->deltaTime) * testInput.movement.y));
	Vec3_SetMagnitude(&angleVectors.forward, ((testInput.speed * time->deltaTime) * testInput.movement.z));

	frameMove.x = angleVectors.forward.x + angleVectors.up.x + angleVectors.left.x;
	frameMove.y = angleVectors.forward.y + angleVectors.up.y + angleVectors.left.y;
	frameMove.z = angleVectors.forward.z + angleVectors.up.z + angleVectors.left.z;

#endif

	t->pos.x += frameMove.x;
	t->pos.y += frameMove.y;
	t->pos.z += frameMove.z;

	// char output[256];
	// sprintf_s(output, "FRAME MOVE\n\nPos: %.2f, %.2f, %.2f\n",
	// 	frameMove.x, frameMove.y, frameMove.z
	// 	);
	// OutputDebugStringA(output);
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

void App_WriteCameraDebug(GameTime* time)
{
    Vec3 pos = g_worldScene.cameraTransform.pos;
    Vec3 rot = g_worldScene.cameraTransform.rot;

    AngleVectors vectors = {};
    
    Calc_AnglesToAxesZYX(&rot, &vectors.left, &vectors.up, &vectors.forward);
    
    char buf[512];
    i32 numWritten = sprintf_s(buf, 512,
    "TimeDelta: %3.7f\n-- Camera --\nPos: %3.3f, %3.3f, %3.3f\nRot: %3.3f, %3.3f, %3.3f\nForward: %3.3f, %3.3f, %3.3f\nUp: %3.3f, %3.3f, %3.3f\nLeft: %3.3f, %3.3f, %3.3f\n",
        time->deltaTime,
        pos.x, pos.y, pos.z,
        rot.x, rot.y, rot.z,
        vectors.forward.x, vectors.forward.y, vectors.forward.z,
        vectors.up.x, vectors.up.y, vectors.up.z,
        vectors.left.x, vectors.left.y, vectors.left.z
    );
    
    ZSTR_WriteChars(&g_debugStr, buf, numWritten);
}

void App_Frame(GameTime* time, InputTick* input)
{
    #if 0
    // Trap to do single presses of this key
    if (input->debug_cycleTexture)
    {
        if (cycleTexturePressed == 0)
        {
            cycleTexturePressed = 1;
            CycleTestAsciChar();
            //CycleTestTexture();
        }
        
    }
    else if (cycleTexturePressed == 1)
    {
        cycleTexturePressed = 0;
    }
    #endif
    
    // TODO: Move this to a better location
    // ...requires better button handling though!
    if (input->escape)
    {
        if (g_input_escapePressed == 0)
        {
            g_input_escapePressed = 1;
            Input_ToggleMouseMode();
        }
    }
    else if (g_input_escapePressed == 1) { g_input_escapePressed = 0; }
    
    if (input->reset)
    {
        g_worldScene.cameraTransform.pos = { 0, 0, 2 };
        g_worldScene.cameraTransform.rot = { 0, 0, 0 };
        g_worldScene.cameraTransform.scale = { 0, 0, 0 };

        // g_uiScene.cameraTransform.pos = { 0, 0, 2 };
        // g_uiScene.cameraTransform.rot = { 0, 0, 0 };
        // g_uiScene.cameraTransform.scale = { 0, 0, 0 };
    }
    else
    {
        Input_ApplyInputToTransform(input, &g_worldScene.cameraTransform, time);

        // Input_ApplyInputToTransform(input, &g_uiScene.cameraTransform, time);
    }

    ///////////////////////////////////////
    // Process gamestate
    ///////////////////////////////////////

    // clear debug buffer
    g_debugStr.length = 0;

    GameState* gs = &g_gameState;
    GameState* ui = &g_uiState;

    // Game state update
    // Update all inputs, entity components and colliders/physics
    Game_UpdateActorMotors(gs, time, input);
    Ent_UpdateAIControllers(gs, time);
    Game_UpdateColliders(gs, time);
    Game_UpdateProjectiles(gs, time);
    //Game_UpdateAI(time);

    // Render
    // Make sure  render lists have been cleared or bad stuff will happen
    g_worldScene.numObjects = 0;
    g_uiScene.numObjects = 0;
    
    #if 1
    Game_BuildRenderList(gs, &g_worldScene);
    Game_DrawColliderAABBs(gs, time, &g_worldScene);

    #if 0
    // Vec3 lineOrigin = g_worldScene->cameraTransform.pos;
    // Vec3 lineDest = lineOrigin;
    // lineDest.z -= 10;
    Vec3 lineOrigin = { 1, 1, 1 };
    Vec3 lineDest = { -1, -1, -1 };
    Transform t;
    Transform_SetToIdentity(&t);

    RendObj rendObj;
    RendObj_SetAsLine(&rendObj,
        lineOrigin.x, lineOrigin.y, lineOrigin.z,
        lineDest.x, lineDest.y, lineDest.z,
        0, 1, 0, 1, 0, 0);
    Game_AddRenderItem(&g_worldScene, &t, &rendObj);
    #endif
    //RScene_AddRenderItem(&g_worldScene, &t, &rendObj);

    Game_IntersectionTest(gs, &g_worldScene);

    platform.Platform_RenderScene(&g_worldScene);
    #endif

    #if 1
    Game_UpdateUI(ui, time, input);
    Game_BuildRenderList(ui, &g_uiScene);
    // Render debug string
    //ZSTR_WriteChars(&g_debugStr, "Test testy\ntest test", 21);
    App_WriteCameraDebug(time);
    Game_SetDebugStringRender();
    Transform t = {};
    t.pos.x = -1;
    t.pos.y = 1;
    //Game_AddRenderItem(&g_uiScene, &t, &g_debugStrRenderer);
    RScene_AddRenderItem(&g_uiScene, &t, &g_debugStrRenderer);
    platform.Platform_RenderScene(&g_uiScene);
    #endif
    
}

// void App_FixedFrame(GameTime* time, InputTick* inputTick)
// {

// }

/***************************************
* Export DLL functions
***************************************/
#include "app_dll_export.cpp"
