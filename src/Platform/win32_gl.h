#pragma once

#include "../Shared/shared.h"

#include "win32_main.h"
#include <gl/gl.h>
#include <windows.h>

#define TEST_BUFFER_WIDTH 32
#define TEST_BUFFER_HEIGHT 32
#define TEST_BUFFER_PIXEL_COUNT (32 * 32)
#define TEST_BUFFER_MEM_SIZE (TEST_BUFFER_PIXEL_COUNT * 4)

win32_offscreen_buffer testBuffer;
i32 testBufferPixels[TEST_BUFFER_PIXEL_COUNT];

i32 InitOpenGL(HWND window)
{
	// TODO: Remove texture test
	testBuffer = {};
	testBuffer.ptrMemory = (void*)testBufferPixels;
	testBuffer.memSize = sizeof(testBufferPixels);
	testBuffer.width = TEST_BUFFER_WIDTH;
	testBuffer.height = TEST_BUFFER_HEIGHT;
	testBuffer.bytesPerPixel = 4;
	testBuffer.pitch = TEST_BUFFER_WIDTH;

	//Com_ZeroMemory((u8*)testBufferPixels, TEST_BUFFER_PIXEL_COUNT * sizeof(u32));
	Com_SetMemory((u8*)testBufferPixels, TEST_BUFFER_MEM_SIZE, 0xAA);

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

    return 1;
}

//bool renderedOnce = false;
void Win32RenderFrame(HWND window)
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
	//f32 aspectRatio = 

	/**
	 * - Fixed function pipeline -
	 * At a high level this is a sequence of commands to the driver. They are executed when the driver wants
	 * Very stateful
	 * Binding textures: Setting the current texture state so they can be referred to.
	 * 
	 */

    //glViewport(x, y, width, height);
	glViewport(0, 0, r.right, r.bottom);

	// Horrible texture loading api stuff
	glTexImage2D(
		GL_TEXTURE_2D,	// target: type of texture
		0, // level: mipmaps
		GL_RGBA, // iternalFormat: How opengl should handle it on it's side. (a suggestion at least)
		32,		// width
		32,		// height
		0,		// border
		GL_BGRA_EXT,		// format: how pixels are stored
		GL_UNSIGNED_BYTE,	// data type of pixels?
		testBuffer.ptrMemory
	);

    //glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	// Create a handle for this texture on the card
	// TODO: Move this to somewhere sensible
	static GLuint textureHandle = 0;
	static i32 init = false;
	if (!init)
	{
		init = true;
		glGenTextures(1, &textureHandle);
	}

	// Set current texture and current texturing modes
	glBindTexture(GL_TEXTURE_2D, textureHandle);

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

	f32 size = 1.0f;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
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

	glEnd();

	// Finished, display
    SwapBuffers(deviceContext);

    ReleaseDC(window, deviceContext);
}
