#pragma once

// #include <gl/gl.h>
// #include "win32_system_include.h"

// #include "../Shared/shared.h"
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
	switch (scene->projectionMode)
	{
		case RENDER_PROJECTION_MODE_3D:
		{
			R_Setup3DProjectionB(scene->fov);
		} break;
		case RENDER_PROJECTION_MODE_ORTHOGRAPHIC:
		{
			R_SetupOrthoProjection(scene->orthographicHalfHeight);
		} break;
		case RENDER_PROJECTION_MODE_IDENTITY:
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
		} break;
		case RENDER_PROJECTION_MODE_3D_OLD:
		{
			R_Setup3DProjectionA(scene->fov);
		} break;
		default :
		{
			R_Setup3DProjectionB(scene->fov);
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
void R_SetModelViewMatrix(Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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

#if 1 // Extra pos/rot/scale from matrix - broken... ?

	Vec4 camPos = M4x4_GetPosition(view->matrix.cells);
	Vec4 camRot = M4x4_GetEulerAnglesDegrees(view->matrix.cells);
	// f32 fAngZ = atan2f(openglM[1], openglM[5]);
    // f32 fAngY = atan2f(openglM[8], openglM[10]);
    // f32 fAngX = -asinf(openglM[9]);
	//f32* m = view->
	
	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);

	f32 x = model->matrix.posX;
	f32 y = model->matrix.posY;
	f32 z = model->matrix.posZ;

	f32 rotY = M4x4_GetAngleX(model->matrix.cells) * RAD2DEG;
	f32 rotX = M4x4_GetAngleY(model->matrix.cells) * RAD2DEG;
	f32 rotZ = M4x4_GetAngleZ(model->matrix.cells) * RAD2DEG;

	f32 scaleX = M4x4_GetScaleX(model->matrix.cells);
	f32 scaleY = M4x4_GetScaleY(model->matrix.cells);
	f32 scaleZ = M4x4_GetScaleZ(model->matrix.cells);

	glTranslatef(x, y, z);
	glRotatef(rotX, 0, 1, 0);
	glRotatef(rotY, 1, 0, 0);
	glRotatef(rotZ, 0, 0, 1);

	glScalef(scaleX, scaleY, scaleZ);

	char buf[512];
	sprintf_s(buf, 512,
"\nView: Rot: %3.3f, %3.3f, %3.3f\nModel: Rot: %3.3f, %3.3f, %3.3f",
camRot.x, camRot.y, camRot.z, rotX, rotY, rotZ
);
	OutputDebugStringA(buf);

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
}

////////////////////////////////////////////////////////////////////
// ModelView - Billboard
// Faces the camera (more or less)
////////////////////////////////////////////////////////////////////
void R_SetModelViewMatrix_Billboard(Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// http://www.songho.ca/opengl/gl_transform.html
	// First, transform the camera (viewing matrix) from world space to eye space
	// Notice all values are negated, because we move the whole scene with the
	// inverse of camera transform
	// x = pitch, y = yaw, z = roll
	
	Vec4 camPos = M4x4_GetPosition(view->matrix.cells);
	Vec4 camRot = M4x4_GetEulerAnglesDegrees(view->matrix.cells);
	
	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);

	Vec4 modelPos = M4x4_GetPosition(model->matrix.cells);
	Vec4 modelRot = M4x4_GetEulerAnglesDegrees(model->matrix.cells);

	glTranslatef(modelPos.x, modelPos.y, modelPos.z);
	glRotatef(modelRot.y, 0, 1, 0);

	//glTranslatef(modelPos.x, modelPos.y, modelPos.z);
	//glRotatef(view->rot.z, 0, 0, 1);
	//glRotatef(view->rot.x, 1, 0, 0);
	//glRotatef(view->rot.y, 0, 1, 0);
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

void R_RenderPrimitive(Transform* camera, Transform* objTransform, RendObj* obj)
{
	glDisable(GL_TEXTURE_2D);
	R_SetModelViewMatrix(camera, objTransform);
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
			Vec4 pos = M4x4_GetPosition(objTransform->matrix.cells);
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
void R_RenderLine(Transform* camera, Transform* objTransform, RendObj* obj)
{
	glDisable(GL_TEXTURE_2D);
	//DebugBreak();
	R_SetModelViewMatrix(camera, objTransform);
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

void R_RenderAsciCharArray(Transform* camera, Transform* objTransform, RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	

	RendObj_AsciCharArray* c = &obj->data.charArray;
	glColor3f(c->r, c->g, c->b);
	R_SetupTestTexture(4);
	Vec4 pos = M4x4_GetPosition(objTransform->matrix.cells);
	R_LoadAsciCharArrayGeometry(
		c->chars, ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS,
		pos.x, pos.y, c->size, win32_aspectRatio);
	//R_LoadAsciCharGeometry(c->asciChar, ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS, 0, 0, 8, win32_aspectRatio);
}

void R_RenderBillboard(Transform* camera, Transform* objTransform, RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	R_SetModelViewMatrix_Billboard(camera, objTransform);
	RendObj_Billboard* b = &obj->data.billboard;
	R_SetupTestTexture(b->textureIndex);
	R_RenderTestGeometry_ColouredQuad(b->r, b->g, b->b, b->a);
}

void R_RenderSprite(Transform* camera, Transform* objTransform, RendObj* obj)
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
			Vec4 pos = M4x4_GetPosition(objTransform->matrix.cells);
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

void R_RenderMesh(Transform* camera, Transform* objTransform, RendObj* obj)
{
	RendObj_ColouredMesh* meshRend = &obj->data.mesh;
	AssertAlways(meshRend->mesh != NULL);
	Mesh* mesh = meshRend->mesh;

	GLuint* texHandles = g_textureHandles;

	glEnable(GL_TEXTURE_2D);
	glColor3f(meshRend->r, meshRend->g, meshRend->b);
	R_SetModelViewMatrix(camera, objTransform);
	R_SetupTestTexture(meshRend->textureIndex);
	
	//f32* meshVerts = mesh->verts;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, mesh->verts);
	glTexCoordPointer(2, GL_FLOAT, 0, mesh->uvs);
	glDrawArrays(GL_TRIANGLES, 0, mesh->numVerts);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void R_RenderEntity(Transform* camera, RenderListItem* item)
{
	switch(item->obj.type)
	{
		case RENDOBJ_TYPE_MESH:
		{
			R_RenderMesh(camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_SPRITE:
		{
			R_RenderSprite(camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_LINE:
		{
			R_RenderLine(camera, &item->transform,  &item->obj);
		} break;
		
		case RENDOBJ_TYPE_PRIMITIVE:
		{
			R_RenderPrimitive(camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_ASCI_CHAR_ARRAY:
		{
			R_RenderAsciCharArray(camera, &item->transform,  &item->obj);
		} break;
		
		case RENDOBJ_TYPE_BILLBOARD:
		{
			R_RenderBillboard(camera, &item->transform,  &item->obj);
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
        R_RenderEntity(&scene->cameraTransform, &scene->sceneItems[i]);
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
