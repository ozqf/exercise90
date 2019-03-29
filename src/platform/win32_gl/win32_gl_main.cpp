#pragma once

#include "win32_gl_module.cpp"

i32 Win32_InitOpenGL(HWND window)
{
	if (g_openglRC != NULL) { return 1; }
    //Win32_InitTestScene();
	
	g_window = window;
	Assert(g_window);

	HDC windowContext = GetDC(g_window);
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
	// ancient opengl 1.1 - default for windows :(
	// HDC "HandleDeviceContext"
	// HGLRC "HandleOpenGLRenderingContext"
	g_openglRC = wglCreateContext(windowContext);

	// TODO: More modern opengl: Context escalation via WGL_ARB_create_context
	// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
	// https://hero.handmade.network/episode/code/day242/#2226
	// https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/

	if (g_openglRC == NULL)
	{
		MessageBox(0, "wglCreateContext failed", "Error", MB_OK | MB_ICONINFORMATION);
		AssertAlways(false);
	}

    if (wglMakeCurrent(windowContext, g_openglRC))
    {
		// This function will also escalate the context if possible
		Win32_GetExtensions(windowContext, &g_extensions);
	}
    else
    {
        GLenum err = glGetError();
        
        MessageBox(0, "wglMakeCurrentFailed", "Error", MB_OK | MB_ICONINFORMATION);
        AssertAlways(false);
    }

    ReleaseDC(g_window, windowContext);

	// Depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Light test
	#if 0
	glEnable(GL_LIGHTING);

	GLfloat values[4] = { 1, 1, 1, 1 };
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, values);
	glEnable(GL_COLOR_MATERIAL);
	// position is in object space
	GLfloat pos[4] = { 0, 1, 0, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	#endif

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
	glGenTextures(REND_NUM_TEXTURE_HANDLES, g_textureHandles);

	Win32_R_InitEmbeddedTextures(&g_platform);

	COM_CALL_PRINT(g_platform.Log, 128, "RENDERER Generated %d texture handles\n", REND_NUM_TEXTURE_HANDLES);

	COM_CALL_PRINT(g_platform.Log, 64, "RENDERER Initialised\n");

    return 1;
}

i32 Win32_R_Shutdown()
{
	if (g_openglRC == NULL) { return 1; }
	wglDeleteContext(g_openglRC);
	g_openglRC = NULL;
	return 1;
}

ScreenInfo Win32_R_GetScreenInfo()
{
	return g_screenInfo;
}

////////////////////////////////////////////////////////////////////
// FRAME LOOP
////////////////////////////////////////////////////////////////////

// Create screen info data and clear buffers for a new frame
ScreenInfo Win32_R_SetupFrame()
{
	ScreenInfo info = { 1, 1, 1 };
	if (g_openglRC == NULL) { return info; }
	/*if (renderedOnce == false)
    {
        renderedOnce = true;
        MessageBox(0, "Render", "Error", MB_OK | MB_ICONINFORMATION);
    }*/

    //HDC deviceContext = GetDC(g_window);

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
	GetClientRect(g_window, &r);
	win32_aspectRatio = (f32)r.right / (f32)r.bottom;

	info.width = r.right;
	info.height = r.bottom;
	info.aspectRatio = win32_aspectRatio;

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
	// Toned down to be less eye bleeding atm
	//glClearColor(0.5f, 0.0f, 0.5f, 0.0f);
	// Appears you don't have to manually clear to black
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return info;
}

void Win32_R_FinishFrame()
{
	if (g_openglRC == NULL) { return; }
    HDC deviceContext = GetDC(g_window);

	// Finished, display
    SwapBuffers(deviceContext);

    ReleaseDC(g_window, deviceContext);
}

// NOTE: Win32_R_SetupFrame Must have been called already!
void Win32_RenderFrame(RenderScene* scene, PlatformTime* time)
{
	if (g_openglRC == NULL) { return; }
	glClear(GL_DEPTH_BUFFER_BIT);
    R_RenderScene(scene);

}
