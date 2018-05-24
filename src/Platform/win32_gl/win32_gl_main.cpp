#pragma once

// #include <gl/gl.h>
// #include "win32_system_include.h"

// #include "../common/com_module.h"
//#include "win32_main.h"
#include "win32_gl_text.h"
//#include "win32_gl_primitives.h"

/*
Reading material

// Tutorials
http://www.songho.ca/opengl/
http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
https://www.wikihow.com/Make-a-Cube-in-OpenGL

// API Reference
https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml
http://docs.gl/
https://msdn.microsoft.com/en-us/library/windows/desktop/dd318361(v=vs.85).aspx

*/

HGLRC g_openglRC = NULL;

i32 Win32_InitOpenGL(HWND window)
{
	if (g_openglRC != NULL) { return 1; }
    //Win32_InitTestScene();

	HDC windowContext = GetDC(window);
    /** 
     * "Negotiate" with opengl for a supported Pixel Format Descriptor
     */

    // Create 'desired' pixel format we want a close match to
    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion = 1;
    desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    desiredPixelFormat.cColorBits = 24;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
    
    int suggestedPixelFormatIndex = ChoosePixelFormat(windowContext, &desiredPixelFormat);

    PIXELFORMATDESCRIPTOR suggestedPixelFormat;

    // Actually get the pixel format windows has suggested
    DescribePixelFormat(windowContext, suggestedPixelFormatIndex,sizeof(suggestedPixelFormat), &suggestedPixelFormat);

	// Apply the format we want
    SetPixelFormat(windowContext, suggestedPixelFormatIndex, &suggestedPixelFormat);

	// Create a context for our window
	// HDC "HandleDeviceContext"
	// HGLRC "HandleOpenGLRenderingContext"
	g_openglRC = wglCreateContext(windowContext);

	if (g_openglRC == NULL)
	{
		MessageBox(0, "wglCreateContext failed", "Error", MB_OK | MB_ICONINFORMATION);
		AssertAlways(false);
	}

    if (wglMakeCurrent(windowContext, g_openglRC))
    {

    }
    else
    {
        GLenum err = glGetError();
        
        MessageBox(0, "wglMakeCurrentFailed", "Error", MB_OK | MB_ICONINFORMATION);
        AssertAlways(false);
    }

    ReleaseDC(window, windowContext);

	// Depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Transparent textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable alpha rejection for transparency
	glAlphaFunc(GL_GREATER, 0.5);
	glEnable(GL_ALPHA_TEST);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Enable back-face culling
	glEnable(GL_CULL_FACE);

	//Win32_InitOpenGLTestAssets();
	// Setup and Generate texture handles
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures(NUM_TEST_TEXTURES, g_textureHandles);


    return 1;
}

i32 Win32_R_Shutdown()
{
	if (g_openglRC == NULL) { return 1; }
	wglDeleteContext(g_openglRC);
	g_openglRC = NULL;
	return 1;
}

////////////////////////////////////////////////////////////////////
// Projection
////////////////////////////////////////////////////////////////////

void R_Setup3DProjectionA(i32 fov)
{
	if (fov <= 0) { fov = 90; }
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
}

void R_SetupOrthoProjection(f32 halfScreenHeight)
{
	glMatrixMode(GL_PROJECTION);

	f32 halfWidth = halfScreenHeight * win32_aspectRatio;
	f32 halfHeight = halfScreenHeight;

	f32 prj[16];
	M4x4_SetOrthoProjection(prj, -halfWidth, halfWidth, -halfHeight, halfHeight, 0, 1000);
	glLoadMatrixf(prj);
}

void R_Setup3DProjectionB(i32 fov)
{
	if (fov <= 0) { fov = 90; }
	glMatrixMode(GL_PROJECTION);
	
	f32 prj[16];
	M4x4_SetToIdentity(prj);
	f32 prjNear = 0.1f;
	f32 prjFar = 1000;
	f32 prjLeft = -0.075f * win32_aspectRatio;
	f32 prjRight = 0.075f * win32_aspectRatio;
	f32 prjTop = 0.075f;
	f32 prjBottom = -0.075f;

	M4x4_SetProjection(prj, prjNear, prjFar, prjLeft, prjRight, prjTop, prjBottom);

	glLoadMatrixf(prj);
}

void R_SetupProjection(RenderScene* scene)
{
	switch (scene->settings.projectionMode)
	{
		case RENDER_PROJECTION_MODE_3D:
		{
			R_Setup3DProjectionB(scene->settings.fov);
		} break;
		case RENDER_PROJECTION_MODE_ORTHOGRAPHIC:
		{
			R_SetupOrthoProjection(scene->settings.orthographicHalfHeight);
		} break;
		case RENDER_PROJECTION_MODE_IDENTITY:
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
		} break;
		case RENDER_PROJECTION_MODE_3D_OLD:
		{
			R_Setup3DProjectionA(scene->settings.fov);
		} break;
		default :
		{
			R_Setup3DProjectionB(scene->settings.fov);
		} break;
	}
}

void R_SetupTestTexture(i32 textureIndex)
{
	// g_gl_primitive_mode
	GLuint texToBind = g_textureHandles[textureIndex];
    glBindTexture(GL_TEXTURE_2D, texToBind);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

}

////////////////////////////////////////////////////////////////////
// Build Display list
////////////////////////////////////////////////////////////////////
void R_BuildDisplayList()
{
    
}

////////////////////////////////////////////////////////////////////
// ModelView
// Standard 3D transformation
////////////////////////////////////////////////////////////////////

// DO NOT CHANGE - This actually works!
void R_SetViewModelMatrixByEuler(Transform* view, Transform* model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	Vec3 camPos = view->pos;
	Vec3 camRot = Transform_GetEulerAnglesDegrees(view);

	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);

	// *slightly* improvement on horrible jitter from physics transforms.
	// Yeah... also breaks projectile rotations so no ta.
	//wd(model->rotation.cells, 0.0001f);
	Vec3 modelRot = Transform_GetEulerAnglesDegrees(model);
	
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(modelRot.y, 0, 1, 0);
	glRotatef(modelRot.x, 1, 0, 0);
	glRotatef(modelRot.z, 0, 0, 1);
	glScalef(model->scale.x, model->scale.y, model->scale.z);
}

// BROKEN
void R_SetViewModelMatrixDirect(Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	 // Calculate model/view manually

	// pull out complete camera transform to M4x4
	// offset by model position.
	// multiply by model rotation
	M4x4 view4x4;
	M4x4 mv4x4;
	M4x4_SetToIdentity(mv4x4.cells);
	Transform_ToM4x4(view, &view4x4);
	M4x4_Copy(view4x4.cells, mv4x4.cells);
	mv4x4.posX += model->pos.x;
	mv4x4.posY += model->pos.y;
	mv4x4.posZ += model->pos.z;
	
	M4x4 model4x4;
	Transform_ToM4x4(model, &model4x4);

	// Clear position, position has already been applied above
	M4x4_SetPosition(model4x4.cells, 0, 0, 0);

	M4x4_Multiply(mv4x4.cells, model4x4.cells, mv4x4.cells);

	glLoadMatrixf((GLfloat*)mv4x4.cells);
}

// BROKEN
void R_SetViewModelMatrixNaive(Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// VIEW
	M4x4 viewM;
	Transform_ToM4x4(view, &viewM);
	viewM.posX = -viewM.posX;
	viewM.posY = -viewM.posY;
	viewM.posZ = -viewM.posZ;

	// MODEL
	M4x4 modelM;
	Transform_ToM4x4(model, &modelM);
	M4x4 result;
	M4x4_Multiply(viewM.cells, modelM.cells, result.cells);

	// COMBINE
	glLoadMatrixf((GLfloat*)result.cells);
}

// BROKEN
void R_SetViewModelMatrixNaive2(Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// --- VIEW ---

	// Split matrices
	M4x4 viewRot;
	Transform_ToM4x4(view, &viewRot);
	M4x4_ClearPosition(viewRot.cells);

	M4x4 viewPos;
	Transform_ToM4x4(view, &viewPos);
	M4x4_ClearRotation(viewPos.cells);

	// viewPos.posX = -viewPos.posX;
	// viewPos.posY = -viewPos.posY;
	// viewPos.posZ = -viewPos.posZ;
	M4x4_Invert(viewPos.cells);
	// M4x4_Invert(viewRot.cells);

	// MODEL
	M4x4 modelRot;
	M4x4 modelPos;
	Transform_ToM4x4(model, &modelRot);
	M4x4_ClearPosition(modelRot.cells);

	Transform_ToM4x4(model, &modelPos);
	M4x4_ClearRotation(modelPos.cells);
	
	// COMBINE
	M4x4 result;
	M4x4_SetToIdentity(result.cells);
	
	M4x4_Multiply(result.cells, viewRot.cells, result.cells);
	M4x4_Multiply(result.cells, viewPos.cells, result.cells);
	M4x4_Multiply(result.cells, modelPos.cells, result.cells);
	M4x4_Multiply(result.cells, modelRot.cells, result.cells);
	
	
	
	glLoadMatrixf((GLfloat*)result.cells);
}

void R_SetModelViewMatrix(RenderSceneSettings* settings, Transform *view, Transform *model)
{
	if (settings->viewModelMode > 3) { settings->viewModelMode = 0; }

	if (settings->viewModelMode == 1)
	{
		R_SetViewModelMatrixDirect(view, model);
		//R_SetViewModelMatrixByEuler(view, model);
	}
	else if (settings->viewModelMode == 2)
	{
		R_SetViewModelMatrixNaive(view, model);
		//R_SetViewModelMatrixByEuler(view, model);
	}
	else if (settings->viewModelMode == 3)
	{
		R_SetViewModelMatrixNaive2(view, model);
		//R_SetViewModelMatrixByEuler(view, model);
	}
	else
	{
		//R_SetViewModelMatrixDirect(view, model);
		R_SetViewModelMatrixByEuler(view, model);
	}
	
	// TODO: Figure out how to construct ViewModel matrix entirely from
	// matrices with no euler angles as they are causing awful jittering
	// with transforms from physics engine
#if 0
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	 // Calculate model/view manually

	// pull out complete camera transform to M4x4
	// offset by model position.
	// multiply by model rotation
	M4x4 view4x4;
	M4x4 mv4x4;
	M4x4_SetToIdentity(mv4x4.cells);
	Transform_ToM4x4(view, &view4x4);
	M4x4_Copy(view4x4.cells, mv4x4.cells);
	mv4x4.posX += model->pos.x;
	mv4x4.posY += model->pos.y;
	mv4x4.posZ += model->pos.z;
	

	M4x4 model4x4;
	Transform_ToM4x4(model, &model4x4);

	// Clear position, position has already been applied above
	M4x4_SetPosition(model4x4.cells, 0, 0, 0);

	M4x4_Multiply(mv4x4.cells, model4x4.cells, mv4x4.cells);

	glLoadMatrixf((GLfloat*)mv4x4.cells);
	

#endif
	// VIEW
#if 0 // Trying to do view without euler angles
	// // ModelView_M = View_M * Model_M
	//f32 viewM[16];
	M4x4 viewM;
	Transform_ToM4x4(view, &viewM);
	//Vec3 angles;
	viewM.posX = -viewM.posX;
	viewM.posY = -viewM.posY;
	viewM.posZ = -viewM.posZ;
	//M4x4_Invert(viewM.cells);
#endif
#if 0 // View via euler angles.
	M4x4 viewM;
	M4x4_SetToIdentity(viewM.cells);
	Vec3 camRot = Transform_GetEulerAngles(view);
	Vec3 camPos = view->pos;
	M4x4_RotateZ(viewM.cells, -camRot.z);
	M4x4_RotateX(viewM.cells, -camRot.x);
	M4x4_RotateY(viewM.cells, -camRot.y);
	M4x4_SetPosition(viewM.cells, -camPos.x, -camPos.y, -camPos.z);
	// glRotatef(-camRot.z, 0, 0, 1);
	// glRotatef(-camRot.x, 1, 0, 0);
	// glRotatef(-camRot.y, 0, 1, 0);
	// glTranslatef(-camPos.x, -camPos.y, -camPos.z);
#endif

#if 0
	// MODEL
	//f32 modelM[16];
	M4x4 modelM;
	Transform_ToM4x4(model, &modelM);
	//f32 result[16];
	M4x4 result;
	M4x4_Multiply(viewM.cells, modelM.cells, result.cells);
	//M4x4_Multiply(modelM.cells, viewM.cells, result.cells);

	// COMBINE
	glLoadMatrixf((GLfloat*)result.cells);
#endif


#if 1 // Calculate model/view using opengl fixed function operations
	// http://www.songho.ca/opengl/gl_transform.html
	// First, transform the camera (viewing matrix) from world space to eye space
	// Notice all values are negated, because we move the whole scene with the
	// inverse of camera transform
	// x = pitch, y = yaw, z = roll
	// Working order
	// view: Z/X/Y or roll/pitch/yaw
	// model is reversed: Y/X/Z
#if 0 // matrices directly... requires matrix multiplication to work
	f32* cells = view->matrix.cells;
	glLoadMatrixf((GLfloat*)cells);

	cells = model->matrix.cells;
	glLoadMatrixf((GLfloat*)cells);
#endif

#if 0 // Extra pos/rot/scale from matrix - broken... ?

	Vec3 camPos = view->pos;
	Vec3 camRot = Transform_GetEulerAnglesDegrees(view);
	// f32 fAngZ = atan2f(openglM[1], openglM[5]);
    // f32 fAngY = atan2f(openglM[8], openglM[10]);
    // f32 fAngX = -asinf(openglM[9]);
	//f32* m = view->

	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);

	M3x3_ClearTinyValues(model->rotation.cells, 0.0001f);
	Vec3 modelRot = Transform_GetEulerAnglesDegrees(model);
	
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
#if 0
	if (modelRot.y != ZNaN())
	{
		//glRotatef(modelRot.y, 0, 1, 0);
	}
	
	if (modelRot.x != ZNaN())
	{
		//glRotatef(modelRot.x, 1, 0, 0);
	}
	
	if (modelRot.z != ZNaN())
	{
		//glRotatef(modelRot.z, 0, 0, 1);
	}
#endif
#if 0
	glRotatef(modelRot.y, 0, 1, 0);
	glRotatef(modelRot.x, 1, 0, 0);
	glRotatef(modelRot.z, 0, 0, 1);
	glScalef(model->scale.x, model->scale.y, model->scale.z);
#endif
// 	char buf[512];
// 	sprintf_s(buf, 512,
// "\nView: Rot: %3.3f, %3.3f, %3.3f\nModel: Rot: %3.3f, %3.3f, %3.3f",
// camRot.x, camRot.y, camRot.z, rotX, rotY, rotZ
// );
// 	OutputDebugStringA(buf);

#endif

#if 0 // Previous system, using Transform with pos/rot/scale vectors

	glRotatef(-view->rot.z, 0, 0, 1);
	glRotatef(-view->rot.x, 1, 0, 0);
	glRotatef(-view->rot.y, 0, 1, 0);
	glTranslatef(-view->pos.x, -view->pos.y, -view->pos.z);

	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(model->rot.y, 0, 1, 0);
	glRotatef(model->rot.x, 1, 0, 0);
	glRotatef(model->rot.z, 0, 0, 1);
	
	glScalef(model->scale.x, model->scale.y, model->scale.z);
#endif
	// transform the object (model matrix)
	// The result of GL_MODELVIEW matrix will be:
	// ModelView_M = View_M * Model_M
#endif
}

////////////////////////////////////////////////////////////////////
// ModelView - Billboard
// Faces the camera (more or less)
////////////////////////////////////////////////////////////////////
void R_SetModelViewMatrix_Billboard(RenderSceneSettings* settings, Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	Vec3 camPos = view->pos;
	Vec3 camRot = Transform_GetEulerAnglesDegrees(view);

	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);

	// *slightly* improvement on horrible jitter from physics transforms.
	// Yeah... also breaks projectile rotations so no ta.
	//wd(model->rotation.cells, 0.0001f);
	Vec3 modelRot = Transform_GetEulerAnglesDegrees(model);
	
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(camRot.y, 0, 1, 0);
	// glRotatef(modelRot.y, 0, 1, 0);
	// glRotatef(modelRot.x, 1, 0, 0);
	// glRotatef(modelRot.z, 0, 0, 1);
	glScalef(model->scale.x, model->scale.y, model->scale.z);
#if 0
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// http://www.songho.ca/opengl/gl_transform.html
	// First, transform the camera (viewing matrix) from world space to eye space
	// Notice all values are negated, because we move the whole scene with the
	// inverse of camera transform
	// x = pitch, y = yaw, z = roll
	
	Vec3 camRot = Transform_GetEulerAnglesDegrees(view);
	
	glRotatef(-view->pos.z, 0, 0, 1);
	glRotatef(-view->pos.x, 1, 0, 0);
	glRotatef(-view->pos.y, 0, 1, 0);
	glTranslatef(-camRot.x, -camRot.y, -camRot.z);

	//Vec4 modelPos = M4x4_GetPosition(model->matrix.cells);
	Vec3 modelRot = Transform_GetEulerAnglesDegrees(model);

	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(modelRot.y, 0, 1, 0);

	//glTranslatef(modelPos.x, modelPos.y, modelPos.z);
	//glRotatef(view->rot.z, 0, 0, 1);
	//glRotatef(view->rot.x, 1, 0, 0);
	//glRotatef(view->rot.y, 0, 1, 0);
#endif
}

////////////////////////////////////////////////////////////////////
// Draw Quad
////////////////////////////////////////////////////////////////////
void R_DrawSpriteGeometry(f32 posX, f32 posY, f32 posZ, RendObj_Sprite* sprite)
{
	f32 hw = sprite->width * 0.5f;
	f32 hh = sprite->height * 0.5f;
	f32 minX = posX - hw;
	f32 maxX = posX + hw;
	f32 minY = posY - hh;
	f32 maxY = posY + hh;

	minX /= win32_aspectRatio;
	maxX /= win32_aspectRatio;
	
	glBegin(GL_TRIANGLES);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvLeft, sprite->uvBottom);
	glVertex3f(minX, minY, posZ);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvRight, sprite->uvBottom);
	glVertex3f(maxX, minY, posZ);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvRight, sprite->uvTop);
	glVertex3f(maxX, maxY, posZ);

	// upper triangle
	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvLeft, sprite->uvBottom);
	glVertex3f(minX, minY, posZ);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvRight, sprite->uvTop);
	glVertex3f(maxX, maxY, posZ);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvLeft, sprite->uvTop);
	glVertex3f(minX, maxY, posZ);

	glEnd();
}

////////////////////////////////////////////////////////////////////
// Geometry
////////////////////////////////////////////////////////////////////
void R_RenderTestGeometry_RainbowQuad()
{
    glBegin(GL_TRIANGLES);

	f32 size = 1;

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

	// upper triangle
	glColor3f(1, 0, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);
	
	glColor3f(0, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	glColor3f(1, 1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-size, size);
    glEnd();
}

void R_RenderTestGeometry_ColouredQuad(f32 r, f32 g, f32 b, f32 a)
{
    glBegin(GL_TRIANGLES);

	f32 size = 1;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
	glColor4f(r, g, b, a);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	glColor4f(r, g, b, a);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(size, -size);

	glColor4f(r, g, b, a);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	// upper triangle
	glColor4f(r, g, b, a);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	glColor4f(r, g, b, a);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	glColor4f(r, g, b, a);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-size, size);

    glEnd();
}

void R_RenderAABBGeometry(f32 x, f32 y, f32 z, f32 sizeX, f32 sizeY, f32 sizeZ, f32 red, f32 green, f32 blue)
{
	f32 halfX = sizeX / 2;
	f32 halfY = sizeY / 2;
	f32 halfZ = sizeZ / 2;
	// Vec3 a = { x - halfX, y - halfY, z - halfZ };
	// Vec3 b = { x + halfX, y + halfY, z + halfZ };
	Vec3 a = { -halfX, -halfY, -halfZ };
	Vec3 b = { halfX, halfY, halfZ };

	glLineWidth(5.0);
	glBegin(GL_LINES);
	glColor3f(red, green, blue);
	// front face
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, a.y, a.z);

	glVertex3f(b.x, a.y, a.z);
	glVertex3f(b.x, b.y, a.z);

	glVertex3f(b.x, b.y, a.z);
	glVertex3f(a.x, b.y, a.z);

	glVertex3f(a.x, b.y, a.z);
	glVertex3f(a.x, a.y, a.z);

	// back face
	glVertex3f(a.x, a.y, b.z);
	glVertex3f(b.x, a.y, b.z);

	glVertex3f(b.x, a.y, b.z);
	glVertex3f(b.x, b.y, b.z);

	glVertex3f(b.x, b.y, b.z);
	glVertex3f(a.x, b.y, b.z);

	glVertex3f(a.x, b.y, b.z);
	glVertex3f(a.x, a.y, b.z);

	// connecting lines
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(a.x, a.y, b.z);

	glVertex3f(b.x, a.y, a.z);
	glVertex3f(b.x, a.y, b.z);

	glVertex3f(b.x, b.y, a.z);
	glVertex3f(b.x, b.y, b.z);

	glVertex3f(a.x, b.y, a.z);
	glVertex3f(a.x, b.y, b.z);
	glEnd();
}

void R_RenderPrimitive(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	glDisable(GL_TEXTURE_2D);
	R_SetModelViewMatrix(settings, camera, objTransform);
	//R_SetupTestTexture();
	RendObj_Primitive* prim = &obj->data.primitive;
	switch (prim->primitiveType)
	{
		case REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD:
		{
			R_RenderTestGeometry_ColouredQuad(prim->red, prim->green, prim->blue, prim->alpha);
		} break;

		case REND_PRIMITIVE_TYPE_RAINBOW_QUAD:
		{
			R_RenderTestGeometry_RainbowQuad();
		} break;

		case REND_PRIMITIVE_TYPE_AABB:
		{
			Vec3 pos = objTransform->pos;
			R_RenderAABBGeometry(
				pos.x, pos.y, pos.z,
				prim->sizeX, prim->sizeY, prim->sizeZ,
				prim->red, prim->green, prim->blue);
		} break;

		default:
		{
			R_RenderTestGeometry_RainbowQuad();
		} break;
	}
}

/*
glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
to switch on,
glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
*/
void R_RenderLine(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	glDisable(GL_TEXTURE_2D);
	//DebugBreak();
	R_SetModelViewMatrix(settings, camera, objTransform);
	RendObj_Line* line = &obj->data.line;
	//glDisable(GL_DEPTH_TEST);
	//glTranslatef(0.0f,0.0f,-10.0f);

	// glLineWidth(10.0);
	// glBegin(GL_LINES);
	// glColor3f(1, 1, 1);
	// glVertex3f(-1, -1, -1);
	// glVertex3f(1, 1, 1);
	// glEnd();

	glLineWidth(10.0);
	glBegin(GL_LINES);

	glColor3f(line->colourA.x, line->colourA.y, line->colourA.z);
	glVertex3f(line->a.x, line->a.y, line->a.z);
	
	glColor3f(line->colourB.x, line->colourB.y, line->colourB.z);
	glVertex3f(line->b.x, line->b.y, line->b.z);
	
	glEnd();
	//glEnable(GL_DEPTH_TEST);
}

void R_RenderAsciChar(RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RendObj_AsciChar* c = &obj->data.asciChar;
	R_SetupTestTexture(4);
	R_LoadAsciCharGeometry(c->asciChar, ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS, 0, 0, 8, win32_aspectRatio);
}

void R_RenderAsciCharArray(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	

	RendObj_AsciCharArray* c = &obj->data.charArray;
	glColor3f(c->r, c->g, c->b);
	R_SetupTestTexture(4);
	Vec3 pos = objTransform->pos;
	R_LoadAsciCharArrayGeometry(
		c->chars, ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS,
		pos.x, pos.y, c->size, win32_aspectRatio);
	//R_LoadAsciCharGeometry(c->asciChar, ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS, 0, 0, 8, win32_aspectRatio);
}

void R_RenderBillboard(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	R_SetModelViewMatrix_Billboard(settings, camera, objTransform);
	RendObj_Billboard* b = &obj->data.billboard;
	R_SetupTestTexture(b->textureIndex);
	R_RenderTestGeometry_ColouredQuad(b->r, b->g, b->b, b->a);
}

void R_RenderSprite(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	//DebugBreak();
	glEnable(GL_TEXTURE_2D);
	RendObj_Sprite* sprite = &obj->data.sprite;
	switch (sprite->mode)
	{
		case SPRITE_MODE_MESH:
		{

		} break;

		case SPRITE_MODE_UI:
		{
			Vec3 pos = objTransform->pos;
			//R_SetupOrthoProjection(8);
			//glMatrixMode(GL_PROJECTION);
			//glLoadIdentity();
			
            // char buf[64];
            // sprintf_s(buf, 64, "SPRITE POS: %3.7f, %3.7f, %3.7f\n", objTransform->pos.x, objTransform->pos.y, objTransform->pos.z);
            // OutputDebugString(buf);


			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			R_SetupTestTexture(sprite->textureIndex);
			//R_RenderTestGeometry_RainbowQuad();
			R_DrawSpriteGeometry(pos.x, pos.y, pos.z, sprite);

		} break;

		case SPRITE_MODE_BILLBOARD:
		{

		} break;
	}
}

void R_RenderMesh(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	RendObj_ColouredMesh* meshRend = &obj->data.mesh;
	AssertAlways(meshRend->mesh != NULL);
	Mesh* mesh = meshRend->mesh;

	GLuint* texHandles = g_textureHandles;

	glEnable(GL_TEXTURE_2D);
	glColor3f(meshRend->r, meshRend->g, meshRend->b);
	R_SetModelViewMatrix(settings, camera, objTransform);
	R_SetupTestTexture(meshRend->textureIndex);
	
	//f32* meshVerts = mesh->verts;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, mesh->verts);
	glTexCoordPointer(2, GL_FLOAT, 0, mesh->uvs);
	glDrawArrays(GL_TRIANGLES, 0, mesh->numVerts);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void R_RenderEntity(RenderSceneSettings* settings, Transform* camera, RenderListItem* item)
{
	switch(item->obj.type)
	{
		case RENDOBJ_TYPE_MESH:
		{
			R_RenderMesh(settings, camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_SPRITE:
		{
			R_RenderSprite(settings, camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_LINE:
		{
			R_RenderLine(settings, camera, &item->transform,  &item->obj);
		} break;
		
		case RENDOBJ_TYPE_PRIMITIVE:
		{
			R_RenderPrimitive(settings, camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_ASCI_CHAR_ARRAY:
		{
			R_RenderAsciCharArray(settings, camera, &item->transform,  &item->obj);
		} break;
		
		case RENDOBJ_TYPE_BILLBOARD:
		{
			R_RenderBillboard(settings, camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_ASCI_CHAR:
		{
			R_RenderAsciChar(&item->obj);
		} break;
	}
}

void R_RenderScene(RenderScene* scene)
{
	R_SetupProjection(scene);
	//R_SetupOrthoProjection(8);
    for (u32 i = 0; i < scene->numObjects; ++i)
    {
		if (scene->sceneItems[i].obj.flags & RENDOBJ_FLAG_DEBUG)
		{
			scene->sceneItems[i].obj.flags |= RENDOBJ_FLAG_DEBUG;
			//DebugBreak();
		}
        R_RenderEntity(&scene->settings, &scene->cameraTransform, &scene->sceneItems[i]);
    }

	// for (u32 i = 0; i < scene->numUIObjects; ++i)
	// {
	// 	R_RenderEntity(&scene->cameraTransform, &scene->uiItems[i]);
	// }
}
 
////////////////////////////////////////////////////////////////////
// FRAME LOOP
////////////////////////////////////////////////////////////////////
void Win32_R_SetupFrame(HWND window)
{
	if (g_openglRC == NULL) { return; }
	/*if (renderedOnce == false)
    {
        renderedOnce = true;
        MessageBox(0, "Render", "Error", MB_OK | MB_ICONINFORMATION);
    }*/

    HDC deviceContext = GetDC(window);

    // Clear screen

	RECT r = {};
	/*
	// get window rect is the rect of the window on this screen
	// What happens on double monitors...?
	GetWindowRect(window, &r);
	// this will calculate the screen from the window rect, roughly,
	// doesn't take into account the size of the title bar at the top
	i32 x = 0, y = 0;
	i32 width = r.right - r.left;
	i32 height = r.bottom - r.top;
	*/
	GetClientRect(window, &r);
	win32_aspectRatio = (f32)r.right / (f32)r.bottom;

	/**
	 * - Fixed function pipeline -
	 * At a high level this is a sequence of commands to the driver. They are executed when the driver wants
	 * Very stateful
	 * Binding textures: Setting the current texture state so they can be referred to.
	 * 
	 */

    //glViewport(x, y, width, height);
	glViewport(0, 0, r.right, r.bottom);

    // Magenta (debugging)
	//glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Win32_R_FinishFrame(HWND window)
{
	if (g_openglRC == NULL) { return; }
    HDC deviceContext = GetDC(window);

	// Finished, display
    SwapBuffers(deviceContext);

    ReleaseDC(window, deviceContext);
}

//bool renderedOnce = false;
void Win32_RenderFrame(RenderScene* scene, GameTime* time)
{
	if (g_openglRC == NULL) { return; }
    //Win32_R_SetupFrame(appWindow);
	glClear(GL_DEPTH_BUFFER_BIT);
	//Win32_ProcessTestInput(inputTick, time);
    R_RenderScene(scene);

}
