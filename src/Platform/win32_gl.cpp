#pragma once

// #include <gl/gl.h>
// #include "win32_system_include.h"

// #include "../Shared/shared.h"
#include "win32_main.h"
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

i32 Win32_InitOpenGL(HWND window)
{
    Win32_InitTestScene();

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
	HGLRC openglRC = wglCreateContext(windowContext);

	if (openglRC == NULL)
	{
		MessageBox(0, "wglCreateContext failed", "Error", MB_OK | MB_ICONINFORMATION);
		AssertAlways(false);
	}


    if (wglMakeCurrent(windowContext, openglRC))
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

	Win32_InitOpenGLTestAssets();

    return 1;
}

////////////////////////////////////////////////////////////////////
// Projection
////////////////////////////////////////////////////////////////////
void R_SetupProjection()
{
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
	M4x4_SetOrthoProjection(prj, -halfWidth, halfWidth, -halfHeight, halfHeight, -1, 1);
	glLoadMatrixf(prj);
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
	
	glRotatef(-view->rot.z, 0, 0, 1);
	glRotatef(-view->rot.x, 1, 0, 0);
	glRotatef(-view->rot.y, 0, 1, 0);
	glTranslatef(-view->pos.x, -view->pos.y, -view->pos.z);
	
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(model->rot.z, 0, 0, 1);
	glRotatef(model->rot.x, 1, 0, 0);
	glRotatef(model->rot.y, 0, 1, 0);
	
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
	
	glRotatef(-view->rot.z, 0, 0, 1);
	glRotatef(-view->rot.x, 1, 0, 0);
	glRotatef(-view->rot.y, 0, 1, 0);
	glTranslatef(-view->pos.x, -view->pos.y, -view->pos.z);
	
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	//glRotatef(view->rot.z, 0, 0, 1);
	//glRotatef(view->rot.x, 1, 0, 0);
	glRotatef(view->rot.y, 0, 1, 0);
}

////////////////////////////////////////////////////////////////////
// Draw Quad
////////////////////////////////////////////////////////////////////
void R_DrawQuad(f32 posX, f32 posY, f32 halfWidth, f32 halfHeight, f32 red, f32 green, f32 blue)
{
	f32 minX = posX - halfWidth;
	f32 maxX = posX + halfWidth;
	f32 minY = posY + halfHeight;
	f32 maxY = posY - halfHeight;
	
	glBegin(GL_TRIANGLES);
	glColor3f(red, green, blue);
	glVertex2f(-minX, -minY);
	glColor3f(red, green, blue);
	glVertex2f(maxX, -minY);
	glColor3f(red, green, blue);
	glVertex2f(maxX, maxY);

	// upper triangle
	glColor3f(red, green, blue);
	glVertex2f(-minX, -minY);
	glColor3f(red, green, blue);
	glVertex2f(maxX, maxY);
	glColor3f(red, green, blue);
	glVertex2f(-minX, maxY);
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
	glVertex2f(-size, -size);
	glColor3f(0, 0, 1);
	glVertex2f(size, size);
	glColor3f(1, 1, 0);
	glVertex2f(-size, size);
    glEnd();
}

void R_RenderTestGeometry_ColouredQuad(f32 r, f32 g, f32 b)
{
    glBegin(GL_TRIANGLES);

	f32 size = 1;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
	glColor3f(r, g, b);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	glColor3f(r, g, b);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(size, -size);

	glColor3f(r, g, b);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	// upper triangle
	glColor3f(r, g, b);
	glVertex2f(-size, -size);
	glColor3f(r, g, b);
	glVertex2f(size, size);
	glColor3f(r, g, b);
	glVertex2f(-size, size);
    glEnd();
}

void R_RenderPrimitive(Transform* camera, RendObj* obj)
{
	glDisable(GL_TEXTURE_2D);
	R_SetModelViewMatrix(camera, &obj->transform);
	//R_SetupTestTexture();
	RendObj_Primitive* prim = &obj->obj.primitive;
	switch (prim->primitiveType)
	{
		case REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD:
		{
			R_RenderTestGeometry_ColouredQuad(prim->red, prim->green, prim->blue);
		} break;

		case REND_PRIMITIVE_TYPE_RAINBOW_QUAD:
		{
			R_RenderTestGeometry_RainbowQuad();
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

void R_RenderBillboard(Transform* camera, RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);
	R_SetModelViewMatrix_Billboard(camera, &obj->transform);
	R_SetupTestTexture(obj->obj.billboard.textureIndex);
	R_RenderTestGeometry_RainbowQuad();
}

void R_RenderMesh(Transform* camera, RendObj* obj)
{
	RendObj_ColouredMesh* meshRend = &obj->obj.mesh;
	AssertAlways(meshRend->mesh != NULL);
	Mesh* mesh = meshRend->mesh;

	GLuint* texHandles = g_textureHandles;

	glEnable(GL_TEXTURE_2D);
	glColor3f(meshRend->r, meshRend->g, meshRend->b);
	R_SetModelViewMatrix(camera, &obj->transform);
	R_SetupTestTexture(meshRend->textureIndex);
	
	//f32* meshVerts = mesh->verts;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, mesh->verts);
	glTexCoordPointer(2, GL_FLOAT, 0, mesh->uvs);
	glDrawArrays(GL_TRIANGLES, 0, mesh->numVerts);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void R_RenderEntity(Transform* camera, RendObj* obj)
{
	switch(obj->type)
	{
		case RENDOBJ_TYPE_MESH:
		{
			R_RenderMesh(camera, obj);
		} break;

		case RENDOBJ_TYPE_PRIMITIVE:
		{
			R_RenderPrimitive(camera, obj);
		} break;

		case RENDOBJ_TYPE_BILLBOARD:
		{
			R_RenderBillboard(camera, obj);
		} break;
	}
}

void R_RenderScene(RenderScene* scene)
{
	R_SetupProjection();
	//R_SetupOrthoProjection(8);
    for (u32 i = 0; i < scene->numObjects; ++i)
    {
        R_RenderEntity(&scene->cameraTransform, &scene->rendObjects[i]);

        //R_SetModelViewMatrix(&cameraTransform, &g_renderObjects[i]);
        //R_SetupTestTexture();
		//R_DrawQuad(0, 0, 1.5, 1.5, 1, 0, 0);
        //R_RenderTestGeometry();
    }
}
 
////////////////////////////////////////////////////////////////////
// FRAME LOOP
////////////////////////////////////////////////////////////////////
//bool renderedOnce = false;
void Win32_RenderFrame(HWND window, RenderScene* scene)
{
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

	//Win32_ProcessTestInput(inputTick, time);
    R_RenderScene(scene);

	// Finished, display
    SwapBuffers(deviceContext);

    ReleaseDC(window, deviceContext);
}
