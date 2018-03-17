#pragma once

#include "app_main.h"
#include "../Shared/shared_assets.h"

#include <stdio.h>

// TODO: STILL USING WINDOWS INCLUDE FOR DEBUGGING. NOT PLATFORM AGNOSTIC!
#include "../Platform/win32_system_include.h"

void R_Scene_CreateTestScene()
{
    RendObj* obj = g_rendObjects;

    // 0
    *obj = {};
    obj->transform.pos.z = -2;
    //g_meshPrimitive_quad
    //RendObj_SetAsColouredQuad(obj, 1, 0, 1);
    RendObj_SetAsMesh(obj, &g_meshPrimitive_quad, 1, 1, 1, 3);
    g_scene.numObjects++;
    obj++;

    // 1
    *obj = {};
    obj->transform.rot.y = 90;
    obj->transform.pos.x = -2;
    RendObj_SetAsColouredQuad(obj, 0, 1, 0);
    g_scene.numObjects++;
    obj++;
    
    // 2
    *obj = {};
    obj->transform.rot.y = 270;
    obj->transform.pos.x = 2;
    RendObj_SetAsColouredQuad(obj, 0, 0, 1);
    g_scene.numObjects++;
    obj++;
    
    // 3
    *obj = {};
    obj->transform.rot.y = 180;
    obj->transform.pos.z = 2;
    RendObj_SetAsColouredQuad(obj, 1, 1, 0);
    g_scene.numObjects++;
    obj++;

    // 4
    *obj = {};
    obj->transform.rot.y = 0;
    obj->transform.pos.x = -3;
    obj->transform.pos.z = -4;
    obj->type = RENDOBJ_TYPE_BILLBOARD;
    g_scene.numObjects++;
    obj++;
    
    // 5
    *obj = {};
    obj->transform.rot.y = 0;
    obj->transform.pos.x = 3;
    obj->transform.pos.z = -4;
    RendObj_SetAsColouredQuad(obj, 1, 0, 0);
    g_scene.numObjects++;
    obj++;
}

void R_Scene_Init()
{
    g_scene = {};
    g_scene.numObjects = 0;
    g_scene.maxObjects = R_MAX_RENDER_OBJECTS;
    g_scene.rendObjects = g_rendObjects;
}

void R_Scene_Tick(GameTime* time, RenderScene* scene)
{
    RendObj* obj;
    i32 rotatingEntity = 5;

    obj = &scene->rendObjects[5];
    obj->transform.rot.y += 90 * time->deltaTime;
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

	// x = pitch, y = yaw, z = roll
	f32 sensitivity = 10.0f;
	i8 inverted = -1;

#if 1 // Disabled mouse input for debugging
	//t->rot.x += testInput.rotation.x * (testInput.rotSpeed * time->deltaTime);
	t->rot.x -= (((f32)input->mouseMovement[1] * sensitivity) * time->deltaTime) * inverted;
	t->rot.x = COM_CapAngleDegrees(t->rot.x);

	//t->rot.y += testInput.rotation.y * (testInput.rotSpeed * time->deltaTime);
	t->rot.y -= ((f32)input->mouseMovement[0] * sensitivity) * time->deltaTime;
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

	AngleToAxes(&t->rot, &t->left, &t->up, &t->forward);

	Vec3 forward = t->forward;
	Vec3 left = t->left;
	Vec3 up = t->up;

	// If input is blank mag will be speed * 0?
	Vec3_SetMagnitude(&left, ((testInput.speed * time->deltaTime) * testInput.movement.x));
	Vec3_SetMagnitude(&up, ((testInput.speed * time->deltaTime) * testInput.movement.y));
	Vec3_SetMagnitude(&forward, ((testInput.speed * time->deltaTime) * testInput.movement.z));
	
	frameMove.x = forward.x + up.x + left.x;
	frameMove.y = forward.y + up.y + left.y;
	frameMove.z = forward.z + up.z + left.z;

	t->pos.x += frameMove.x;
	t->pos.y += frameMove.y;
	t->pos.z += frameMove.z;

	// char output[256];
	// sprintf_s(output, "FRAME MOVE\n\nPos: %.2f, %.2f, %.2f\n",
	// 	frameMove.x, frameMove.y, frameMove.z
	// 	);
	// OutputDebugStringA(output);
}

#if 0
#define BMP_FILE_TYPE 19778

// TODO: This needs to be in the platform layer!
u8 ReadBMPTest(BlockRef ref)
{
    void* mem = Heap_GetBlockMemoryAddress(&g_heap, &ref);
	
	WINBMPFILEHEADER* fileHeader = (WINBMPFILEHEADER*)mem;
	AssertAlways(fileHeader->FileType == BMP_FILE_TYPE);
	
	const i32 fileHeaderSize = sizeof(WINBMPFILEHEADER);

	WINNTBITMAPHEADER* bmpHeader = (WINNTBITMAPHEADER*)((u8*)mem + fileHeaderSize);

	// only supporting 32 bit bitmaps right now!
	AssertAlways(bmpHeader->BitsPerPixel == 32);
	AssertAlways(bmpHeader->Compression == 3);
	
	u32* pixels = (u32*)((u8*)mem + fileHeaderSize + bmpHeader->Size);
	u32 firstPixel = *pixels;

	// Byte order is AA BB GG RR
	// Little endian 0xRRGGBBAA

	u8 alpha = (u8)(firstPixel);
	u8 blue = (u8)(firstPixel >> 8);
	u8 green = (u8)(firstPixel >> 16);
	u8 red = (u8)(firstPixel >> 24);

	//  GL_RGBA

	// Rearrange to rgba
	
    DebugBreak();
	return 1;
}
#endif

////////////////////////////////////////////////////////////////////////////
// App Interface
// App_ == interface function
// Return 1 if successful, 0 if failed
////////////////////////////////////////////////////////////////////////////
i32 App_Init()
{
    printf("DLL Init\n");
    // Init player
    // player.halfWidth = 1;
    // player.halfHeight = 1;
    // player.pos[0] = 0;
    // player.pos[1] = 0;
    // player.vel[0] = 0;
    // player.vel[1] = 0;
    // player.speed = 10;

    u32 mainMemorySize = MegaBytes(64);
    MemoryBlock mem = {};

	//u8 readBMP1;
	//u8 readBMP2;

    if (!platform.Platform_Malloc(&mem, mainMemorySize))
    {
        return 0;
    }
    else
    {
        Heap_Init(&g_heap, mem.ptrMemory, mem.size);
        //char* filePath = "ReadTest.txt";
        // BlockRef fileRef2 = platform.Platform_LoadFileIntoHeap(&g_heap, "ReadTest2.txt");
        // BlockRef fileRef1 = platform.Platform_LoadFileIntoHeap(&g_heap, "ReadTest.txt");

		// BlockRef bitmapReadTestRef = platform.Platform_LoadFileIntoHeap(&g_heap, "base/BitmapTest.bmp");
		// readBMP1 = ReadBMPTest(bitmapReadTestRef);

		/*BlockRef consoleCharsRef = platform.Platform_LoadFileIntoHeap(&g_heap, "base/charset.bmp");
		readBMP2 = ReadBMPTest(consoleCharsRef);*/
		
        
		//readBMP2 = ReadBMPTest(fileRef1);
    }

    SharedAssets_Init();

    i32 numTextures = platform.Platform_LoadDebugTextures(&g_heap);
    //DebugBreak();
    R_Scene_Init();
    R_Scene_CreateTestScene();

    testInput = {};
	testInput.speed = 3.0f;
    testInput.rotSpeed = 90.0f;
    
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

void CycleTestTexture()
{
    
    RendObj* obj = g_rendObjects + 0;
    RendObj_ColouredMesh* rMesh = &obj->obj.mesh;
	//DebugBreak();
    rMesh->textureIndex++;
    if (rMesh->textureIndex > 3)
    {
        rMesh->textureIndex = 0;
    }
    char buf[128];
        sprintf_s(buf, 128, "Cycled test texture to: %d\n", rMesh->textureIndex);
        OutputDebugString(buf);
}

u8 cycleTexturePressed = 0;

void App_Frame(GameTime* time, InputTick* input)
{
    // Trap to do single presses of this key
    if (input->debug_cycleTexture)
    {
        if (cycleTexturePressed == 0)
        {
            cycleTexturePressed = 1;
            CycleTestTexture();
        }
        
    }
    else if (cycleTexturePressed == 1)
    {
        cycleTexturePressed = 0;
    }

    if (input->reset)
    {
        g_scene.cameraTransform.pos = { 0, 0, 2 };
        g_scene.cameraTransform.rot = { 0, 0, 0 };
        g_scene.cameraTransform.scale = { 0, 0, 0 };
    }
    else
    {
        Input_ApplyInputToTransform(input, &g_scene.cameraTransform, time);
        //Input_ApplyInputToTransform()
    }

    R_Scene_Tick(time, &g_scene);
    platform.Platform_RenderScene(&g_scene);
}

// void App_FixedFrame(GameTime* time, InputTick* inputTick)
// {

// }

/***************************************
* Export DLL functions
***************************************/
#include "app_dll_export.cpp"
