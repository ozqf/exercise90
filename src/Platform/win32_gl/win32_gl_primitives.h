#pragma once

#include "win32_gl_main.h"

// #include <gl/gl.h>
// #include <math.h>

// #include "win32_main.h"
// #include "win32_gl.h"

// #define TEST_BUFFER_WIDTH 32
// #define TEST_BUFFER_HEIGHT 32
// #define TEST_BUFFER_PIXEL_COUNT (32 * 32)
// #define TEST_BUFFER_MEM_SIZE (TEST_BUFFER_PIXEL_COUNT * 4)

// #define NUM_GL_PRIMITIVE_MODES 3
// global_variable i32 g_gl_primitive_mode = 0;
// global_variable f32 g_gl_primitive_degrees_X = 0;
// global_variable f32 g_gl_primitive_degrees_Y = 0;

// global_variable f32 win32_aspectRatio = 0;

// //global_variable Transform cameraTransform;

// global_variable Transform entityTransform;
// global_variable Transform entityTransform2;
// global_variable Transform entityTransform3;

// global_variable int xOffset = 0;
// global_variable int yOffset = 0;

// win32_offscreen_buffer testBuffer;
// i32 testBufferPixels[TEST_BUFFER_PIXEL_COUNT];

// win32_offscreen_buffer testBuffer2;
// i32 testBufferPixels2[TEST_BUFFER_PIXEL_COUNT];

// win32_offscreen_buffer testBuffer3;
// i32 testBufferPixels3[TEST_BUFFER_PIXEL_COUNT];

// f32 mat[TRANSFORM_MATRIX_SIZE];

/*
6 sides, 2 triangles per side = 12 tris
12 * 3 points, each with pos (vec3), normal (vec3), colour (vec3) and uv (vec2)

	v6	v7

	v4	v5
  ________
 /		/ |
/______/  |
|v2	v3 |  |
|	   | /
|______|/
v0		v1
*/
#if 0
Vec3 primitive_cubeVerts[8];
Vec3 primitive_triangleVerts[12 * 3];
Vec3 primitive_cubeNormals[12 * 3];
Vec2 primitive_cubeUvs[12];

void ZSetTriangle(Vec3* vectors, Vec3 a, Vec3 b, Vec3 c)
{
	vectors[VEC_X] = a;
	vectors[VEC_Y] = b;
	vectors[VEC_Z] = c;
}


void win32_glBuildPrimitives()
{
	primitive_cubeVerts[0] = { -1, -1, -1 };
	primitive_cubeVerts[1] = {  1, -1, -1 };
	primitive_cubeVerts[2] = { -1,  1, -1 };
	primitive_cubeVerts[3] = {  1,  1, -1 };

	primitive_cubeVerts[4] = { -1, -1,  1 };
	primitive_cubeVerts[5] = {  1, -1,  1 };
	primitive_cubeVerts[6] = { -1,  1,  1 };
	primitive_cubeVerts[3] = {  1,  1,  1 };
	
	/*		Tri0	Tri1	Face Normal		UVs
	front	0 1 3 - 0 3 2	 0  0 -1		(0, 0), (1, 0), (1, 1) - (0, 0), (1, 1), (0, 1)
	right   1 5 7 - 1 7 3	 1  0  0		(0  ), (), ()
	back	5 4 6 - 5 6 7	 0  0  1
	prjLeft	4 0 2 - 4 2 6    -1 0  0
	top		2 3 6 - 2 7 6    0  1  0
	bottom	4 5 1 - 4 1 0	 0 -1  0
	*/
	
	// front
	ZSetTriangle(primitive_triangleVerts + 0, primitive_cubeVerts[0], primitive_cubeVerts[1], primitive_cubeVerts[3]);
	ZSetTriangle(primitive_triangleVerts + 3, primitive_cubeVerts[0], primitive_cubeVerts[3], primitive_cubeVerts[2]);
	// right
	ZSetTriangle(primitive_triangleVerts + 6, primitive_cubeVerts[1], primitive_cubeVerts[5], primitive_cubeVerts[7]);
	ZSetTriangle(primitive_triangleVerts + 9, primitive_cubeVerts[1], primitive_cubeVerts[7], primitive_cubeVerts[3]);
	// back
	ZSetTriangle(primitive_triangleVerts + 12, primitive_cubeVerts[5], primitive_cubeVerts[4], primitive_cubeVerts[6]);
	ZSetTriangle(primitive_triangleVerts + 15, primitive_cubeVerts[5], primitive_cubeVerts[6], primitive_cubeVerts[7]);
	// prjLeft
	ZSetTriangle(primitive_triangleVerts + 18, primitive_cubeVerts[4], primitive_cubeVerts[0], primitive_cubeVerts[2]);
	ZSetTriangle(primitive_triangleVerts + 21, primitive_cubeVerts[4], primitive_cubeVerts[2], primitive_cubeVerts[6]);
	// top
	ZSetTriangle(primitive_triangleVerts + 24, primitive_cubeVerts[2], primitive_cubeVerts[3], primitive_cubeVerts[6]);
	ZSetTriangle(primitive_triangleVerts + 27, primitive_cubeVerts[2], primitive_cubeVerts[7], primitive_cubeVerts[6]);
	// bottom
	ZSetTriangle(primitive_triangleVerts + 30, primitive_cubeVerts[4], primitive_cubeVerts[5], primitive_cubeVerts[1]);
	ZSetTriangle(primitive_triangleVerts + 33, primitive_cubeVerts[4], primitive_cubeVerts[1], primitive_cubeVerts[0]);
}

// Mesh CreateCube()
// {
	

// 	Vec2 v;
	
// }

static const GLfloat g_testTriangleVerts[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};

void Win32_InitTestScene()
{
	// entityTransform = {};

    // entityTransform2 = {};
    // entityTransform2.rot.y = 90;
    // entityTransform2.pos.x = -1;
    // entityTransform2.pos.z = 1;

    // entityTransform3 = {};
    // entityTransform3.rot.y = 270;
    // entityTransform3.pos.x = 1;
    // entityTransform3.pos.z = 1;

	// Transform* obj = g_renderObjects;

	// *obj = {};
	// obj->pos.z = -2;
	// g_numRenderObjects++;
	// obj++;
	
    // *obj = {};
    // obj->rot.y = 90;
    // obj->pos.x = -2;
	// g_numRenderObjects++;
	// obj++;

    // *obj = {};
    // obj->rot.y = 270;
    // obj->pos.x = 2;
	// g_numRenderObjects++;
	// obj++;
    
    // *obj = {};
    // obj->rot.y = 180;
    // obj->pos.z = 2;
	// g_numRenderObjects++;
	// obj++;
    

}
#endif
#if 0
// Cut this stuff out and move to app/delete altogether
void Win32_InitOpenGLTestAssets()
{
	// TODO: Remove texture test
	testBuffer = {};
	testBuffer.ptrMemory = (void*)testBufferPixels;
	testBuffer.memSize = sizeof(testBufferPixels);
	testBuffer.width = TEST_BUFFER_WIDTH;
	testBuffer.height = TEST_BUFFER_HEIGHT;
	testBuffer.bytesPerPixel = 4;
	testBuffer.pitch = TEST_BUFFER_WIDTH;

    testBuffer2 = {};
	testBuffer2.ptrMemory = (void*)testBufferPixels2;
	testBuffer2.memSize = sizeof(testBufferPixels2);
	testBuffer2.width = TEST_BUFFER_WIDTH;
	testBuffer2.height = TEST_BUFFER_HEIGHT;
	testBuffer2.bytesPerPixel = 4;
	testBuffer2.pitch = TEST_BUFFER_WIDTH;

    testBuffer3 = {};
	testBuffer3.ptrMemory = (void*)testBufferPixels3;
	testBuffer3.memSize = sizeof(testBufferPixels3);
	testBuffer3.width = TEST_BUFFER_WIDTH;
	testBuffer3.height = TEST_BUFFER_HEIGHT;
	testBuffer3.bytesPerPixel = 4;
	testBuffer3.pitch = TEST_BUFFER_WIDTH;

	//Com_ZeroMemory((u8*)testBufferPixels, TEST_BUFFER_PIXEL_COUNT * sizeof(u32));
	u8 colour[4];
	// rgba
	colour[0] = 255;
	colour[1] = 0;
	colour[2] = 0;
	colour[3] = 128;
	COM_SetMemoryPattern((u8*)testBufferPixels, TEST_BUFFER_MEM_SIZE, colour, 4);

	colour[0] = 0;
	colour[1] = 255;
	colour[2] = 0;
	colour[3] = 255;
    COM_SetMemoryPattern((u8*)testBufferPixels2, TEST_BUFFER_MEM_SIZE, colour, 4);
	
	colour[0] = 0;
	colour[1] = 0;
	colour[2] = 255;
	colour[3] = 255;
    COM_SetMemoryPattern((u8*)testBufferPixels3, TEST_BUFFER_MEM_SIZE, colour, 4);
	
	// 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/**
	--- setup textures ---
	> enable textures
	> generate texture ids, as many as required
	> loop through textures
		> load texture data and settings via
		glTexImage2D
		glTexParameteri
		glBindTexture -> set the current texture settings to the specified texture id
	*/
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glGenTextures(NUM_TEST_TEXTURES, g_textureHandles);

#if 1
	Win32_Platform_R_RegisterTexture(testBuffer.ptrMemory, 32, 32);
	Win32_Platform_R_RegisterTexture(testBuffer2.ptrMemory, 32, 32);
	Win32_Platform_R_RegisterTexture(testBuffer3.ptrMemory, 32, 32);
#endif

#if 0
	// tex 1
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer.ptrMemory);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// S, T, R: texture coordinates after transform - clamp, wrap, etc
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, g_nextTexture);
	++g_nextTexture;

	// tex 2
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer2.ptrMemory);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, g_nextTexture);
	++g_nextTexture;

	// tex 3
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer3.ptrMemory);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, g_nextTexture);
	++g_nextTexture;
#endif
	/*
	Handle 1, 2, 3
	*/
}
#endif
#if 0

void Win32_DebugPrintMatrix(char* label, f32* matrix)
{
	char output[256];
	sprintf_s(output, "%s\n/ %.2f, %.2f, %.2f, %.2f \\\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n\\ %.2f, %.2f, %.2f, %.2f /\n",
		label,
		matrix[0], matrix[4], matrix[8], matrix[12],
		matrix[1], matrix[5], matrix[9], matrix[13],
		matrix[2], matrix[6], matrix[10], matrix[14],
		matrix[3], matrix[7], matrix[11], matrix[15]
		);
	OutputDebugStringA(output);
}

void Win32_DebugPrintVec3(char* label, Vec3* v)
{
	char output[256];
	sprintf_s(output, "%s\n: %.2f, %.2f, %.2f\n",
		label,
		v->x, v->y, v->z
		);
	OutputDebugStringA(output);
}

void Win32_DebugPrintTransform(char* label, Transform* t)
{
	char output[256];
	sprintf_s(output, "%s\nPos: %.2f, %.2f, %.2f\nRot %.2f, %.2f, %.2f\n",
		label,
		t->pos.x, t->pos.y, t->pos.z,
		t->rot.x, t->rot.y, t->rot.z
		);
	OutputDebugStringA(output);

	sprintf_s(output, "Forward: %.2f, %.2f, %.2f\nUp %.2f, %.2f, %.2f\nLeft %.2f, %.2f, %.2f\n\n",
		t->forward.x, t->forward.y, t->forward.z,
		t->up.x, t->up.y, t->up.z,
		t->left.x, t->left.y, t->left.z
		);
	OutputDebugStringA(output);
}

#endif

#if 0
void Win32_ApplyInputToTransform(InputTick input, Transform* t, GameTime time)
{
	testInput.movement = { 0, 0, 0 };
	testInput.rotation = { 0, 0, 0 };

	/////////////////////////////////////////////////
	// READ ROTATION
	/////////////////////////////////////////////////
	if (input.yawLeft) { testInput.rotation.y += 1; }
	if (input.yawRight) { testInput.rotation.y += -1; }

	if (input.pitchUp) { testInput.rotation.x += -1; }
	if (input.pitchDown) { testInput.rotation.x += 1; }

	if (input.rollLeft) { testInput.rotation.z += 1; }
	if (input.rollRight) { testInput.rotation.z += -1; }

	// x = pitch, y = yaw, z = roll
	f32 sensitivity = 10.0f;
	i8 inverted = -1;

	//t->rot.x += testInput.rotation.x * (testInput.rotSpeed * time.deltaTime);
	t->rot.x -= (((f32)input.mouseMovement[1] * sensitivity) * time.deltaTime) * inverted;
	t->rot.x = COM_CapAngleDegrees(t->rot.x);

	//t->rot.y += testInput.rotation.y * (testInput.rotSpeed * time.deltaTime);
	t->rot.y -= ((f32)input.mouseMovement[0] * sensitivity) * time.deltaTime;
	t->rot.y = COM_CapAngleDegrees(t->rot.y);

	t->rot.z += testInput.rotation.z * (testInput.rotSpeed * time.deltaTime);
	t->rot.z = COM_CapAngleDegrees(t->rot.z);

	if (input.moveLeft) { testInput.movement.x += -1; }
	if (input.moveRight) { testInput.movement.x += 1; }

	if (input.moveForward) { testInput.movement.z += -1; }
	if (input.moveBackward) { testInput.movement.z += 1; }

	if (input.moveDown) { testInput.movement.y += -1; }
	if (input.moveUp) { testInput.movement.y += 1; }
	
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
	Vec3_SetMagnitude(&left, ((testInput.speed * time.deltaTime) * testInput.movement.x));
	Vec3_SetMagnitude(&up, ((testInput.speed * time.deltaTime) * testInput.movement.y));
	Vec3_SetMagnitude(&forward, ((testInput.speed * time.deltaTime) * testInput.movement.z));
	
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

void Win32_ProcessTestInput(InputTick input, GameTime gameTime, RenderScene* scene)
{
	//OutputDebugStringA("******* FRAME *******\n");
	Transform* t;

	t = &scene->cameraTransform;
	//t = &entityTransform;

	if (input.reset)
	{
		scene->cameraTransform.pos = { 0, 0, 2 };
		scene->cameraTransform.rot = { 0, 0, 0 };
		scene->cameraTransform.scale = { 1, 1, 1 };

		//entityTransform.pos = { 0, 0, 0 };
		//entityTransform.rot = { 0, 0, 0 };
		//entityTransform.scale = { 1, 1, 1 };
		return;
	}
	else
	{
		Win32_ApplyInputToTransform(input, t, gameTime);
	}
}
#endif
