#ifndef WIN32_GL_H
#define WIN32_GL_H


#include <gl/gl.h>
#include <windows.h>



i32 InitOpenGL(HWND window)
{
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

    //glViewport(x, y, width, height);
	glViewport(0, 0, r.right, r.bottom);
    //glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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

	// lower triangle
	glVertex2f(-size, -size);
	glVertex2f(size, -size);
	glVertex2f(size, size);
	// upper triangle
	glVertex2f(-size, -size);
	glVertex2f(size, size);
	glVertex2f(-size, size);

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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glEnd();

	// Finished, display
    SwapBuffers(deviceContext);

    ReleaseDC(window, deviceContext);
}

#endif