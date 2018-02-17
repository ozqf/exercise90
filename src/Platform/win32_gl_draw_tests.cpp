#pragma once

#include <gl/gl.h>
#include <math.h>

#include "../Shared/shared.h"
#include "win32_main.h"

#include "../Shared/linmath.h"


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
