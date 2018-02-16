#pragma once

#include <gl/gl.h>
#include <math.h>

#include "../Shared/shared.h"
#include "win32_main.h"

#include "../Shared/linmath.h"


#define TEST_BUFFER_WIDTH 32
#define TEST_BUFFER_HEIGHT 32
#define TEST_BUFFER_PIXEL_COUNT (32 * 32)
#define TEST_BUFFER_MEM_SIZE (TEST_BUFFER_PIXEL_COUNT * 4)

#define NUM_GL_PRIMITIVE_MODES 3
global_variable i32 g_gl_primitive_mode = 0;
global_variable f32 g_gl_primitive_degrees_X = 0;
global_variable f32 g_gl_primitive_degrees_Y = 0;

global_variable f32 win32_aspectRatio = 0;
global_variable GL_Test_Input testInput;
global_variable Transform cameraTransform;
global_variable Transform entityTransform;

global_variable int xOffset = 0;
global_variable int yOffset = 0;

win32_offscreen_buffer testBuffer;
i32 testBufferPixels[TEST_BUFFER_PIXEL_COUNT];

win32_offscreen_buffer testBuffer2;
i32 testBufferPixels2[TEST_BUFFER_PIXEL_COUNT];

win32_offscreen_buffer testBuffer3;
i32 testBufferPixels3[TEST_BUFFER_PIXEL_COUNT];

f32 mat[TRANSFORM_MATRIX_SIZE];

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

#define NUM_TEST_TEXTURES 3
GLuint textureHandles[NUM_TEST_TEXTURES];

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
	Com_SetMemory((u8*)testBufferPixels, TEST_BUFFER_MEM_SIZE, 0x44);
    Com_SetMemory((u8*)testBufferPixels2, TEST_BUFFER_MEM_SIZE, 0x88);
    Com_SetMemory((u8*)testBufferPixels3, TEST_BUFFER_MEM_SIZE, 0xCC);

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
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glGenTextures(NUM_TEST_TEXTURES, textureHandles);

	// tex 1
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer.ptrMemory);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// S, T, R: texture coordinates after transform - clamp, wrap, etc
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, 1);

	// tex 2
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer2.ptrMemory);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, 2);

	// tex 3
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer3.ptrMemory);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, 3);

	/*
	Handle 1, 2, 3
	*/
}

void Win32_DrawScreenSpaceTest1()
{
    
	// Horrible texture loading api stuff
	glTexImage2D(
		GL_TEXTURE_2D,          // target: type of texture
		0,                      // level: mipmaps
		GL_RGBA,                // iternalFormat: How opengl should handle it on it's side. (a suggestion at least)
		32,		                // width
		32,		                // height
		0,		                // border
		GL_BGRA_EXT,		    // format: how pixels are stored
		GL_UNSIGNED_BYTE,       // data type of pixels?
		testBuffer.ptrMemory
	);

	// Create a handle for this texture on the card
	// TODO: Move this to somewhere sensible
	static GLuint textureHandle = 0;
	static i32 init = false;
	if (!init)
	{
		init = true;
		//glGenTextures(1, &textureHandle);
	}

	// Set current texture and current texturing modes
	//glBindTexture(GL_TEXTURE_2D, textureHandle);

	// pixel is close to screen Disable bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// S, T, R: texture coordinates after transform - clamp, wrap, etc
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
	// glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
	// glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	// glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, GL_CLAMP);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// enable texturing
	glEnable(GL_TEXTURE_2D);

	/*
	gl load identity/identity matrix = matrix that has no effect:
	1 0 0 0
	0 1 0 0
	0 0 1 0
	0 0 0 1
	*/

	/*
	two matrices, modelView and Projection - ignoring model view, apparently
	related to old hardware lighting
	*/

	// If desired, transform texture coordinates. Not necessarily needed...?
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    /*
    glBegin -> glEnd "delimit the vertices of a primitive or a group of like primitives"
    */
	// Begin vertex stuff
	glBegin(GL_TRIANGLES);
	// glVertex + 
	// > f2 (2D Float)
	// > i2 (2D int)
	// > f3 (3D Float)
	// etc...
	// triangle verts are counter-clockwise
	//i32 halfW = width / 2;
	//i32 halfH = height / 2;

	f32 size = 0.8f;

	// lower triangle. Bottom prjLeft -> Bottom Right -> Top Right
	//glColor3f(1, 0, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	//glColor3f(0, 1, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(size, -size);

	//glColor3f(0, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	// upper triangle
	// glColor3f(1, 0, 0);
	// glVertex2f(-size, -size);
	// glColor3f(0, 0, 1);
	// glVertex2f(size, size);
	// glColor3f(1, 1, 0);
	// glVertex2f(-size, size);

	// // Foreground
	// size = 0.1f;
	// glColor3f(1, 1, 1);
	// glVertex3f(-size, -size, -1);
	// glColor3f(1, 1, 1);
	// glVertex3f(size, -size, -1);
	// glColor3f(1, 1, 1);
	// glVertex3f(size, size, -1);

    // end vertex stuff
	glEnd();

}

void Win32_DrawMinimumTriangle()
{
    glEnable(GL_TEXTURE_2D);
    // Horrible texture loading api stuff
	glTexImage2D(
		GL_TEXTURE_2D,          // target: type of texture
		0,                      // level: mipmaps
		GL_RGBA,                // iternalFormat: How opengl should handle it on it's side. (a suggestion at least)
		32,		                // width
		32,		                // height
		0,		                // border
		GL_BGRA_EXT,		    // format: how pixels are stored
		GL_UNSIGNED_BYTE,       // data type of pixels?
		testBuffer.ptrMemory
	);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// S, T, R: texture coordinates after transform - clamp, wrap, etc
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glEnable(GL_TEXTURE_2D);

    glBegin(GL_TRIANGLES);
    
	f32 size = 0.8f;

	// lower triangle. Bottom prjLeft -> Bottom Right -> Top Right
	//glColor3f(1, 0, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	//glColor3f(0, 1, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(size, -size);

	//glColor3f(0, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

    glEnd();
}

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

/**
 * Minimum required to draw a textured triangle via fixed pipeline
 */
void Win32_DrawToggleTriangle(GL_Test_Input input)
{
    //glEnable(GL_TEXTURE_2D);
    // Horrible texture loading api stuff
	// glTexImage2D(
	// 	GL_TEXTURE_2D,          // target: type of texture
	// 	0,                      // level: mipmaps
	// 	GL_RGBA,                // iternalFormat: How opengl should handle it on it's side. (a suggestion at least)
	// 	32,		                // width
	// 	32,		                // height
	
	// 	GL_BGRA_EXT,		    // format: how pixels are stored
	// 	GL_UNSIGNED_BYTE,       // data type of pixels?
	// 	testBuffer.ptrMemory
	// );

    // switch (g_gl_primitive_mode)
    // {
    //     case 1: {
    //         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer2.ptrMemory);
    //     } break;

    //     case 2: {
    //         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer3.ptrMemory);
    //     } break;

    //     default: {
    //         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer.ptrMemory);
    //     } break;
    // }

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, testBuffer.ptrMemory);
    // Set current texture
	GLuint texToBind = textureHandles[g_gl_primitive_mode];
    glBindTexture(GL_TEXTURE_2D, texToBind);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(entityTransform.pos.x, entityTransform.pos.y, entityTransform.pos.z);
	
	// Apply rotations around axis
	// glRotatef(entityTransform.rot.x, 1.0f, 0.0f, 0.0f);
	// glRotatef(entityTransform.rot.y, 0.0f, 1.0f, 0.0f);
	// glRotatef(entityTransform.rot.z, 0.0f, 0.0f, 1.0f);

	// glRotatef(entityTransform.rot.z, 0.0f, 0.0f, 1.0f);
	// glRotatef(entityTransform.rot.y, 0.0f, 1.0f, 0.0f);
	// glRotatef(entityTransform.rot.x, 1.0f, 0.0f, 0.0f);

	glRotatef(entityTransform.rot.y, 0.0f, 1.0f, 0.0f);
	glRotatef(entityTransform.rot.x, 1.0f, 0.0f, 0.0f);
	glRotatef(entityTransform.rot.z, 0.0f, 0.0f, 1.0f);
	
	/*
	Calculate projection matrix
	TODO: crudely applying aspect ratio. What is the correct way to do this?
	*/

	glMatrixMode(GL_PROJECTION);

	f32 prj[16];
	M4x4_SetToIdentity(prj);
	f32 prjNear = 1;
	f32 prjFar = 1000;
	f32 prjLeft = -0.5f * win32_aspectRatio;
	f32 prjRight = 0.5f * win32_aspectRatio;
	f32 prjTop = 0.5f;
	f32 prjBottom = -0.5f;

	M4x4_SetProjection(prj, prjNear, prjFar, prjLeft, prjRight, prjTop, prjBottom);

	glLoadMatrixf(prj);

    glBegin(GL_TRIANGLES);

	// Set vertex Array
	// glVertexPointer(3, GL_FLOAT, 0, pointer);

	f32 size = 0.8f;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
	glColor3f(1, 0, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	glColor3f(0, 1, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(size, -size);

	glColor3f(0, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

    glEnd();
}

void Win32_DrawTriangleAGeometry()
{
	
}

void Win32_DrawCameraTriangle()
{
	GLuint texToBind = textureHandles[g_gl_primitive_mode];
    glBindTexture(GL_TEXTURE_2D, texToBind);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	////////////////////////////////////////////////////////////////////
	// ModelView
	////////////////////////////////////////////////////////////////////

	glMatrixMode(GL_MODELVIEW);

// USE_OPENGL_VIEWMODEL works... the others less so...
#define USE_OPENGL_VIEWMODEL
//#define USE_CUSTOM_VIEWMODEL_2 1
//#define USE_CUSTOM_VIEWMODEL 1

#ifdef USE_OPENGL_VIEWMODEL

	//glPushMatrix();
	glLoadIdentity();

	// http://www.songho.ca/opengl/gl_transform.html
	// First, transform the camera (viewing matrix) from world space to eye space
	// Notice all values are negated, because we move the whole scene with the
	// inverse of camera transform
	glRotatef(-cameraTransform.rot.y, 0, 1, 0);
	glRotatef(-cameraTransform.rot.z, 0, 0, 1);
	glRotatef(-cameraTransform.rot.x, 1, 0, 0);
	
	// glRotatef(-cameraTransform.rot.x, 1, 0, 0);
	// glRotatef(-cameraTransform.rot.z, 0, 0, 1);
	// glRotatef(-cameraTransform.rot.y, 0, 1, 0);

	//Vec3 axis;
	// axis = cameraTransform.up;
	// glRotatef(-cameraTransform.rot.y, axis.x, axis.y, axis.z);
	// axis = cameraTransform.left;
	// glRotatef(-cameraTransform.rot.x, axis.x, axis.y, axis.z);
	// axis = cameraTransform.forward;
	// glRotatef(-cameraTransform.rot.z, axis.x, axis.y, axis.z);
	
	glTranslatef(-cameraTransform.pos.x, -cameraTransform.pos.y, -cameraTransform.pos.z);
	
	// transform the object (model matrix)
	// The result of GL_MODELVIEW matrix will be:
	// ModelView_M = View_M * Model_M
	glTranslatef(entityTransform.pos.x, entityTransform.pos.y, entityTransform.pos.z);

	// axis = entityTransform.up;
	// glRotatef(entityTransform.rot.y, axis.x, axis.y, axis.z);
	// axis = entityTransform.left;
	// glRotatef(entityTransform.rot.x, axis.x, axis.y, axis.z);
	// axis = entityTransform.forward;
	// glRotatef(entityTransform.rot.z, axis.x, axis.y, axis.z);

	glRotatef(entityTransform.rot.y, 0, 1, 0);
	glRotatef(entityTransform.rot.x, 1, 0, 0);
	glRotatef(entityTransform.rot.z, 0, 0, 1);

#endif

#ifdef USE_CUSTOM_VIEWMODEL

	f32 mv[16];
	M4x4_SetToIdentity(mv);

	f32 pitch = cameraTransform.rot.x * DEG2RAD;
	f32 yaw = cameraTransform.rot.y * DEG2RAD;

	f32 rotX = pitch * (f32)sin(yaw);
	f32 rotY = yaw;// (f32)sin(pitch);
	f32 rotZ = pitch * (f32)cos(yaw);

	M4x4_SetRotation(
		mv,
		rotX,
		rotY,
		rotZ
	);

	M4x4_SetRotation(
		mv,
		cameraTransform.rot.x * DEG2RAD,
		cameraTransform.rot.y * DEG2RAD,
		cameraTransform.rot.z * DEG2RAD
	);

	f32 model[16];
	M4x4_SetToIdentity(model);
	M4x4_SetRotation(model, entityTransform.rot.x,entityTransform.rot.y, entityTransform.rot.z);

	M4x4_Multiply(model, mv, model);

	Vec3 mvTranslation = {};
	mvTranslation.x += -cameraTransform.pos.x + entityTransform.pos.x;
	mvTranslation.y += -cameraTransform.pos.y + entityTransform.pos.y;
	mvTranslation.z += -cameraTransform.pos.z + entityTransform.pos.z;

	M4x4_SetMove(mv, mvTranslation.x, mvTranslation.y, mvTranslation.z);

	glLoadMatrixf(mv);

	f32 test[16];
	M4x4_SetCameraMatrix(test, cameraTransform.left, cameraTransform.up, cameraTransform.forward, cameraTransform.pos);
	Win32_DebugPrintTransform("Camera Transform", &cameraTransform);
	Win32_DebugPrintTransform("Entity Transform", &entityTransform);
	Win32_DebugPrintMatrix("Test Rot matrix", test);
#endif

#ifdef USE_CUSTOM_VIEWMODEL_2
	f32 mv[16];
	M4x4_SetToIdentity(mv);

	/////////////////////////////////////////////
	// Apply Rotation
	/////////////////////////////////////////////
	Vec3 eye, centre;

	eye.x = cameraTransform.pos.x;
	eye.y = cameraTransform.pos.y;
	eye.z = cameraTransform.pos.z;
	centre.x = eye.x + cameraTransform.forward.x;
	centre.y = eye.y + cameraTransform.forward.y;
	centre.z = eye.z + cameraTransform.forward.z;
	
	M4x4_LookAt(mv, &eye, &centre, &cameraTransform.up);

	/////////////////////////////////////////////
	// Apply Translation
	/////////////////////////////////////////////
	Vec3 mvTranslation = {};
	// mvTranslation.x += -cameraTransform.pos.x + entityTransform.pos.x;
	// mvTranslation.y += -cameraTransform.pos.y + entityTransform.pos.y;
	// mvTranslation.z += -cameraTransform.pos.z + entityTransform.pos.z;
	mvTranslation.x += -cameraTransform.pos.x + entityTransform.pos.x;
	mvTranslation.y += -cameraTransform.pos.y + entityTransform.pos.y;
	mvTranslation.z += -cameraTransform.pos.z + entityTransform.pos.z;
	M4x4_SetMove(mv, mvTranslation.x, mvTranslation.y, mvTranslation.z);

	glLoadMatrixf(mv);

	//M4x4_SetCameraMatrix(test, cameraTransform.left, cameraTransform.up, cameraTransform.forward, cameraTransform.pos);
	Win32_DebugPrintVec3("Eye Position", &eye);
	Win32_DebugPrintVec3("Look At Position", &centre);
	Win32_DebugPrintTransform("Camera Transform", &cameraTransform);
	Win32_DebugPrintTransform("Entity Transform", &entityTransform);
	//Win32_DebugPrintMatrix("Test Rot matrix", mv);
#endif

	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	Win32_DebugPrintMatrix("View Model Matrix", (f32*)&modelView);
	// char output[256];
	// sprintf_s(output, "ModelView\n/ %.2f, %.2f, %.2f, %.2f \\\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n\\ %.2f, %.2f, %.2f, %.2f /\n",
	// 	modelView[0], modelView[4], modelView[8], modelView[12],
	// 	modelView[1], modelView[5], modelView[9], modelView[13],
	// 	modelView[2], modelView[6], modelView[10], modelView[14],
	// 	modelView[3], modelView[7], modelView[11], modelView[15]
	// 	);
	// OutputDebugStringA(output);

	//glPopMatrix();

	////////////////////////////////////////////////////////////////////
	// Projection
	////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_PROJECTION);

	f32 prj[16];
	M4x4_SetToIdentity(prj);
	f32 prjNear = 1;
	f32 prjFar = 1000;
	f32 prjLeft = -0.5f * win32_aspectRatio;
	f32 prjRight = 0.5f * win32_aspectRatio;
	f32 prjTop = 0.5f;
	f32 prjBottom = -0.5f;

	M4x4_SetProjection(prj, prjNear, prjFar, prjLeft, prjRight, prjTop, prjBottom);

	glLoadMatrixf(prj);


	////////////////////////////////////////////////////////////////////
	// Geometry
	////////////////////////////////////////////////////////////////////
    glBegin(GL_TRIANGLES);

	// Set vertex Array
	// glVertexPointer(3, GL_FLOAT, 0, pointer);

	f32 size = 0.8f;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
	glColor3f(1, 0, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	glColor3f(0, 1, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(size, -size);

	glColor3f(0, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

    glEnd();
}

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

	if (input.rollLeft) { testInput.rotation.z += -1; }
	if (input.rollRight) { testInput.rotation.z += 1; }

	// /this rotation is world axis aligned and thus broken!
	t->rot.x += testInput.rotation.x * (testInput.rotSpeed * time.deltaTime);
	t->rot.x = COM_CapAngleDegrees(t->rot.x);

	t->rot.y += testInput.rotation.y * (testInput.rotSpeed * time.deltaTime);
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

	char output[256];
	sprintf_s(output, "FRAME MOVE\n\nPos: %.2f, %.2f, %.2f\n",
		frameMove.x, frameMove.y, frameMove.z
		);
	OutputDebugStringA(output);
}

void Win32_ApplyInputToTransform_OldNaiveRotationIgnoring(InputTick input, Transform* transform, GameTime time)
{
	testInput.movement = { 0, 0, 0 };
	testInput.rotation = { 0, 0, 0 };

	if (input.reset)
	{
		transform->pos = { 0, 0, 0 };
		transform->rot = { 0, 0, 0 };
		transform->scale = { 1, 1, 1 };
		return;
	}

	if (input.moveLeft) { testInput.movement.x += -1; }
	if (input.moveRight) { testInput.movement.x += 1; }

	if (input.moveForward) { testInput.movement.z += -1; }
	if (input.moveBackward) { testInput.movement.z += 1; }
	
	if (input.moveDown) { testInput.movement.y += -1; }
	if (input.moveUp) { testInput.movement.y += 1; }
	
	
	if (input.yawLeft) { testInput.rotation.y += -1; }
	if (input.yawRight) { testInput.rotation.y += 1; }

	if (input.pitchUp) { testInput.rotation.x += -1; }
	if (input.pitchDown) { testInput.rotation.x += 1; }

	if (input.rollLeft) { testInput.rotation.z += -1; }
	if (input.rollRight) { testInput.rotation.z += 1; }

	transform->pos.x += testInput.movement.x * testInput.speed;
	transform->pos.y += testInput.movement.y * testInput.speed;
	transform->pos.z += testInput.movement.z * testInput.speed;

	transform->rot.x += testInput.rotation.x * testInput.rotSpeed;
	transform->rot.x = COM_CapAngleDegrees(transform->rot.x);
	transform->rot.y += testInput.rotation.y * testInput.rotSpeed;
	transform->rot.y = COM_CapAngleDegrees(transform->rot.y);
	transform->rot.z += testInput.rotation.z * testInput.rotSpeed;
	transform->rot.z = COM_CapAngleDegrees(transform->rot.z);
}

void Win32_DrawGLTest(InputTick input, GameTime gameTime)
{
	OutputDebugStringA("******* FRAME *******\n");
	Transform* t;

	t = &cameraTransform;
	//t = &entityTransform;

	if (input.reset)
	{
		cameraTransform.pos = { 0, 0, 2 };
		cameraTransform.rot = { 0, 0, 0 };
		cameraTransform.scale = { 1, 1, 1 };

		entityTransform.pos = { 0, 0, 0 };
		entityTransform.rot = { 0, 0, 0 };
		entityTransform.scale = { 1, 1, 1 };
		return;
	}
	else
	{
		Win32_ApplyInputToTransform(input, t, gameTime);
	}

	Win32_DrawCameraTriangle();
}
