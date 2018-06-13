#pragma once



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
	printf("RENDERER Generated %d texture handles\n", NUM_TEST_TEXTURES);

	printf("RENDERER Initialised\n");

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

// NOTE: Win32_R_SetupFrame Must have been called already!
void Win32_RenderFrame(RenderScene* scene, GameTime* time)
{
	if (g_openglRC == NULL) { return; }
	glClear(GL_DEPTH_BUFFER_BIT);
    R_RenderScene(scene);

}
