#pragma once

#include "win32_module.cpp"

/****************************************************************
Command line
****************************************************************/

void Win32_ReadCommandLine(LPSTR lpCmdLine)
{
    while (*lpCmdLine && (g_numLaunchParams < MAX_LAUNCH_PARAMS))
    {
        // While not null && If not a valid ascii character (spaces etc) move until a printable character is hit
        while (*lpCmdLine && ((*lpCmdLine <= 32) && (*lpCmdLine > 126)))
        {
            lpCmdLine++;
        }

        if (*lpCmdLine)
        {
            // Mark a new token start
            g_launchParams[g_numLaunchParams] = lpCmdLine;
            g_numLaunchParams++;

            // Subdivide the string up by changing the character at the end of a valid section to NULL

            // While null && while it is a valid ascii code (printable character a-z + numbers etc, see ascii table) continue to read until end
            while (*lpCmdLine && ((*lpCmdLine > 32) && (*lpCmdLine <= 126)))
            {
                lpCmdLine++;
            }

            // If lpCmdLine != NULL, change this character to a null to terminate the string segment here
            if (*lpCmdLine)
            {
                *lpCmdLine = 0;
                lpCmdLine++;
            }
        }
    }

    // Set final character to NULL
    g_launchParams[g_numLaunchParams] = NULL;
}

void Win32_Shutdown()
{
    globalRunning = false;
}

/*********************************************************************
Windows generic concat string
*********************************************************************/
// void Win32_SPrintf_s(char* buffer, u32 charCount, char* format, ...)
// {
//     sprintf_s(buffer, charCount, format, ...);
// }

/*********************************************************************
Windows misc
*********************************************************************/
win32_window_dimension Win32_GetWindowDimension(HWND window)
{
    win32_window_dimension result;
    RECT clientRect;
    GetClientRect(window, &clientRect);
    result.width = clientRect.right - clientRect.left;
    result.height = clientRect.bottom - clientRect.top;
    return result;
}

/*********************************************************************
Windows message callback
*********************************************************************/
LRESULT CALLBACK Win32_MainWindowCallback(
    HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (uMsg)
    {
    case WM_SIZE:
    {
    }
    break;

    #if 1
    case WM_INPUT:
    {
        /*
        Taken from
        https://msdn.microsoft.com/en-us/library/windows/desktop/ee418864(v=vs.100).aspx
        article on reading mouse input
        */
        UINT dwSize = 40;
        static BYTE lpb[40];

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
                        lpb, &dwSize, sizeof(RAWINPUTHEADER));

        RAWINPUT *raw = (RAWINPUT *)lpb;

        if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            int xPosRelative = raw->data.mouse.lLastX;
            int yPosRelative = raw->data.mouse.lLastY;

            g_win32_mousePosMove.x += raw->data.mouse.lLastX;
            g_win32_mousePosMove.y += raw->data.mouse.lLastY;
        }
    }
    break;
    #endif

    case WM_LBUTTONDOWN:
    {
        //SysInputEvent ev = NewInputEvent(Z_INPUT_CODE_MOUSE_1, 1);
        //Win32_WritePlatformCommand(&g_input, (u8 *)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(SysInputEvent));
        Sys_WriteInputEvent(&g_input, Z_INPUT_CODE_MOUSE_2, 1);
    }
    break;

    case WM_LBUTTONUP:
    {
        //SysInputEvent ev = NewInputEvent(Z_INPUT_CODE_MOUSE_1, 0);
        //Win32_WritePlatformCommand(&g_input, (u8 *)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(SysInputEvent));
        Sys_WriteInputEvent(&g_input, Z_INPUT_CODE_MOUSE_1, 0);
    }
    break;

    case WM_RBUTTONDOWN:
    {
        //SysInputEvent ev = NewInputEvent(Z_INPUT_CODE_MOUSE_2, 1);
        //Win32_WritePlatformCommand(&g_input, (u8 *)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(SysInputEvent));
        Sys_WriteInputEvent(&g_input, Z_INPUT_CODE_MOUSE_2, 1);
    }
    break;

    case WM_RBUTTONUP:
    {
        Sys_WriteInputEvent(&g_input, Z_INPUT_CODE_MOUSE_2, 0);
        //SysInputEvent ev = NewInputEvent(Z_INPUT_CODE_MOUSE_2, 0);
        //Win32_WritePlatformCommand(&g_input, (u8 *)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(SysInputEvent));
    }
    break;

    case WM_CLOSE:
    {
        //PostQuitMessage(0);
        // TODO: Handle this with a message to the user?
        Win32_Shutdown();
        printf("WM_CLOSE\n");
    }
    break;

    case WM_DESTROY:
    {
        // Handle this as an error - recreate window?
        Win32_Shutdown();
        printf("WM_DESTROY\n");
    }
    break;

    case WM_SETFOCUS:
    {
        g_windowActive = 1;
        Win32_CheckMouseState();
    }
    break;

    case WM_KILLFOCUS:
    {
        g_windowActive = 0;
        Win32_CheckMouseState();
    }
    break;

    case WM_ACTIVATEAPP:
    {
        //printf("WM_ACTIVATEAPP\n");
    }
    break;

    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
        u32 VKCode = wParam;
        bool wasDown = ((lParam & (1 << 30)) != 0);
        bool isDown = ((lParam & (1 << 31)) == 0);

        // Debugging keys!
        // tilde == 223
        if (VKCode == 223 && wasDown)
        {
            Win32_ToggleDebugInput();
            break;
        }
        if (g_debugInputActive && isDown)
        {
            Win32_DebugReadKey(VKCode, wParam, lParam);
            break;
        }
		// F1 -> quick minimise
		if (VKCode == 112)
		{
			PLAT_LOG(64, "PLATFORM: Minimise\n");
			/*
			https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-showwindow
			https://docs.microsoft.com/en-us/windows/desktop/api/shellapi/ns-shellapi-_notifyicondataa
			SW_SHOW
			SW_HIDE
			SW_MAXIMIZE
			SW_MINIMIZE
			SW_RESTORE
			
			BOOL Shell_NotifyIconA(
					DWORD dwMessage,
					PNOTIFYICONDATAA lpData
			);
			*/
			/*
			https://docs.microsoft.com/en-us/windows/desktop/shell/notification-area
			Notifications area
			*/
			ShowWindow(appWindow, SW_MINIMIZE);
			ShowWindow(consoleHandle, SW_MINIMIZE);
			//Win32_ExecuteTextCommand("MINIMISED");
			break;
		}
		
		// F1 -> quick minimise
		if (VKCode == 113)
		{
			PLAT_PRINT(64, "PLATFORM: Instant exit\n");
			globalRunning = false;
		}

        // if (VKCode == 'P' && isDown)
        // {
        //     MessageBox(0, "Dead stop!", "Stop!", MB_OK | MB_ICONINFORMATION);
        //     DebugBreak();
        // }

        // if (VKCode == '`' && isDown)
        // {
        //     Assert(Win32_LinkToApplication());
        // }

        // if (VKCode == 'T' && isDown)
        // {
        //     Assert(Win32_LinkToApplication());
        // }

        if (VKCode == VK_F4 && (lParam & (1 << 29)))
        {
            globalRunning = false;
        }

        if (g_singleFrameStepMode == 1 && VKCode == VK_SHIFT && isDown)
        {
            g_singleFrameRun = 1;
        }

        // Write to command buffer
        // TODO: Not thread safe or anything!
        u32 inputCode = Win32_KeyCode_To_Input_Code(VKCode);
        if (inputCode != 0)
        {
            Sys_WriteInputEvent(&g_input, inputCode, (i32)isDown);
            //SysInputEvent ev = NewInputEvent(inputCode, (i32)isDown);
            //Win32_WritePlatformCommand(&g_input, (u8 *)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(SysInputEvent));
            break;
        }
        else
        {
            //printf("PLATFORM Unknown VK_CODE %d\n", VKCode);
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT Paint;
        // passing address of paint, presumably to have it filled in with current screen details
        HDC DeviceContext = BeginPaint(window, &Paint);
        int x = Paint.rcPaint.left;
        int y = Paint.rcPaint.top;
        int width = Paint.rcPaint.right - Paint.rcPaint.left;
        int height = Paint.rcPaint.bottom - Paint.rcPaint.top;

        win32_window_dimension dimension = Win32_GetWindowDimension(window);
        Win32CopyBufferToWindow(DeviceContext, dimension.width, dimension.height, &globalBackBuffer, x, y, width, height);

        EndPaint(window, &Paint);
    }
    break;

    default:
    {
        //OutputDebugStringA("WM_Default\n");
        result = DefWindowProc(window, uMsg, wParam, lParam);
    }
    break;
    }

    return result;
}

void Win32_SendAppInput()
{
    Win32_TickInput(&g_input);
    //////////////////////////////////////////////////////////////////
    // Command handling:
    // > Mark buffer as ended
    // > copy current state of command buffer
    // > Swap platform buffer between A and B
    // > Send copy to app
    //////////////////////////////////////////////////////////////////
    //*(u8*)g_input.ptrWrite = 0;
    g_input.ptrEnd = g_input.ptrWrite;
    ByteBuffer frameCommands = g_input;
    //OutputDebugStringA("Swap platform buffers\n");
    if (g_input.ptrStart == g_commandBufferA)
    {
        g_input.ptrStart = g_commandBufferB;
        g_input.ptrWrite = g_commandBufferB;
        g_input.ptrEnd = g_commandBufferB;
    }
    else if (g_input.ptrStart == g_commandBufferB)
    {
        g_input.ptrStart = g_commandBufferA;
        g_input.ptrWrite = g_commandBufferA;
        g_input.ptrEnd = g_commandBufferA;
    }
    else
    {
        ILLEGAL_CODE_PATH
    }
    g_app.AppInput(&g_time, frameCommands);
    COM_ZeroMemory(frameCommands.ptrStart, frameCommands.capacity);
}

void Win32_RunAppFrame()
{
    if (g_app.isValid)
    {
        g_app.AppUpdate(&g_time);
    }
}

internal void Win32_ShowInitError(i32 err)
{
    char buf[512];
    sprintf_s(buf, 512, "Code %d\n", err);
    Win32_Error(buf, "Platform startup error");
}

/**********************************************************************
 * WIN32 ENTRY POINT
 *********************************************************************/
int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
	int initialisationError = 0;
    errno_t err = 0;

    //DebugBreak();

    #if 0
	Win32_AttachErrorHandlers();
	char* crashString = NULL;
	printf(crashString);
    #endif

    
    /*errno_t freopen(
       FILE** pFile,
       const char *path,
       const char *mode,
       FILE *stream
    );*/
    
    SYSTEMTIME t;
    GetSystemTime(&t);
    COM_STRING(timeStr, 128, "%d/%d/%d - %d:%d:%d\n",
        t.wYear,
        t.wMonth,
        t.wDay,
        t.wHour,
        t.wMinute,
        t.wSecond
    );
    COM_STRING(logFileName, 128, "ex90_log_%d_%d_%d - %d_%d_%d.txt",
        t.wYear,
        t.wMonth,
        t.wDay,
        t.wHour,
        t.wMinute,
        t.wSecond
    );

    // Open log file
    err = fopen_s(&g_logFile, logFileName, "w");
    if (err != 0) { Win32_ShowInitError(err); }

    // Spawn debugging windows cmd
    #if 1
    FILE *stream;
    AllocConsole();
    freopen_s(&stream, "conin$", "r", stdin);
    freopen_s(&stream, "conout$", "w", stdout);
    freopen_s(&stream, "conout$", "w", stderr);
    consoleHandle = GetConsoleWindow();
    MoveWindow(consoleHandle, 1, 1, 680, 600, 1);
    printf("[%s] Console initialized.\n", __FILE__);
    #endif

    // Print stdout to a file
    #if 0 // Currently not working. Only logs from platform, not DLLs
    FILE *stream;
    //AllocConsole();

	//AllocConsole();
	//err = freopen_s(&stream, "conin$", "r", stdin);
    //if (err != 0) { Win32_ShowInitError(err); }

	err = freopen_s(&stream, logFileName, "w", stdout);
    if (err != 0) { Win32_ShowInitError(err); }

	//err = freopen_s(&stream, "logerr.txt", "w", stderr);
    //if (err != 0) { Win32_ShowInitError(err); }

    //consoleHandle = GetConsoleWindow();
    //MoveWindow(consoleHandle, 1, 1, 680, 600, 1);
    printf("[%s] Log initialized. Session started %s\n", __FILE__, timeStr);
    #endif

    Win32_BuildTextCommandList();
    
    //MessageBox(0, "Start breakpoint", "Started", MB_OK | MB_ICONINFORMATION);
    //DebugBreak();

    Win32_ReadCommandLine(lpCmdLine);
    //Assert(false);

    // self info for game dll
    Win32_InitPlatformInterface();

    // Prepare module link paths before trying to
    // link to any of them

    g_appLink.path = "base/gamex86.dll";
    g_appLink.pathForCopy = "base/gamex86copy.dll";
    g_rendererLink.path = "win32gl.dll";
    g_rendererLink.pathForCopy = "win32glcopy.dll";
    g_soundLink.path = "win32sound.dll";
    g_soundLink.pathForCopy = "win32soundcopy.dll";

    InitDebug();
    //printf("File %s, line: %d\n", __FILE__, __LINE__);

    // Created basic window
    WNDCLASS WindowClass = {}; // {} initialises all variables to 0 (cpp only)
    WindowClass.style =
        CS_OWNDC // allocates a unique device context for each window in the window class
        | CS_HREDRAW | CS_VREDRAW;

    Win32ResizeDIBSection(&globalBackBuffer, 1280, 720);

    //win32LoadXInput();

    WindowClass.lpfnWndProc = Win32_MainWindowCallback;
    WindowClass.hInstance = hInstance;
    //	WindowClass.hIcon
    WindowClass.lpszClassName = "Exercise90WindowClass";

    i32 windowWidth = 640;//1280;
    i32 windowHeight = 400;//720;

    RECT r;
    r.top = r.left = 0;
    r.right = windowWidth;
    r.bottom = windowHeight;
    g_screenInfo.width = windowWidth;
    g_screenInfo.height = windowHeight;
    g_screenInfo.aspectRatio = (f32)windowWidth / (f32)windowHeight;

    ///////////////////////////////////////////////////////////////////////////////////////////

    // Adjust desired rect to include area of window including border.
    // top or left may well now be negative
    //
    AdjustWindowRect(&r, WindowClass.style, false);

    if (initialisationError)
    {
        if (err != 0) { Win32_ShowInitError(initialisationError); }
        return initialisationError;
    }

    // register window class, returns an atom. 0 if register failed
    if (RegisterClass(&WindowClass))
    {
        appWindow = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            "Exercise 90",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, // window style
            CW_USEDEFAULT,                    // Default position x
            CW_USEDEFAULT,                    // Default position y
            r.right - r.left,                 // CW_USEDEFAULT,
            r.bottom - r.top,                 // CW_USEDEFAULT,
            0,
            0,
            hInstance,
            0);

        if (appWindow)
        {
            if (Win32_LinkToRenderer() == false)
            {
                MessageBox(0, "InitOpenGL failed", "Error", MB_OK | MB_ICONINFORMATION);
                globalRunning = false;
                return 1;
            }

            // Input requires appWindow handle to register for mouse events
            Win32_InitInput();

            // Init Sound
            Win32_LinkToSound();

            float previousTime = Win32_InitFloatTime();
            g_time = {};

            // Make sure assets are ready before scene!
            SharedAssets_Init();
            
#if 1
            if (!Win32_LinkToApplication())
            {
                MessageBox(0, "Failed to init sound", "Error", MB_OK | MB_ICONINFORMATION);
                return 1;
            }
            // Set timestamp so that we don't instantly reload the app
            Win32_UpdateFileTimeStamp(g_appLink.path, &g_appLink.timestamp);
            Win32_UpdateFileTimeStamp(g_rendererLink.path, &g_rendererLink.timestamp);
            Win32_UpdateFileTimeStamp(g_soundLink.path, &g_soundLink.timestamp);
#endif

            /****************************************************************
            Game loop
            ****************************************************************/
            while (globalRunning)
            {
                MSG message;
                while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
                {
                    if (message.message == WM_QUIT)
                    {
                        globalRunning = false;
                    }
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }

				Win32_AttachErrorHandlers();

                // Force app to give up CPU time if not current focus target
                // or on a laptop!
                if (g_lowPowerMode)
                {
                    if (g_windowActive == 0)
                    {
                        Sleep(16);
                    }
                    else
                    {
                        Sleep(6);
                    }
                }
                
                //////////////////////////////////////////////
                // TIMING
                //////////////////////////////////////////////
                float newTime = Win32_FloatTime();
                g_time.deltaTime = newTime - previousTime;
                previousTime = newTime;
				g_time.frameNumber++;

                //////////////////////////////////////////////
                // CHECK HOT DLL RELOAD
                //////////////////////////////////////////////
                g_appLink.checkTick -= g_time.deltaTime;
                if (g_appLink.checkTick <= 0)
                {
                    g_appLink.checkTick = 0.1f;
                    if (Win32_CheckFileModified(g_appLink.path, &g_appLink.timestamp))
                    {
                        Win32_LinkToApplication();
                    }
                }
                
                g_rendererLink.checkTick -= g_time.deltaTime;
                if (g_rendererLink.checkTick <= 0)
                {
                    g_rendererLink.checkTick = 0.1f;
                    if (Win32_CheckFileModified(g_rendererLink.path, &g_rendererLink.timestamp))
                    {
                        if (Win32_LinkToRenderer() && g_app.isValid)
                        {
                            g_app.AppRendererReloaded();
                        }
                    }
                }
                
                // g_soundLink.checkTick -= g_time.deltaTime;
                // if (g_soundLink.checkTick <= 0)
                // {
                //     g_soundLink.checkTick = 0.1f;
                //     if (Win32_CheckFileModified(g_soundLink.path, &g_soundLink.timestamp))
                //     {
                //         if (Win32_LinkToSound() && g_app.isValid)
                //         {
                //             g_app.AppSoundReloaded();
                //         }
                //     }
                // }

                // Keeping this, helped me find a buffer overrun due to crazy timing behaviour
                if (g_time.deltaTime < 0)
                {
                    MessageBox(0, "Error: Negative timeDelta", "Error", MB_OK | MB_ICONINFORMATION);
                    return 1;
                }

                
                //Win32_CheckTextBuffer();
                Win32_ExecuteTextCommands();
                Win32_SendAppInput();
                Win32_RunAppFrame();
                
                // Render
                if (g_rendererLink.moduleState == 1)
                {
                    //g_time.percentToNextFixedFrame =
                    //    g_fixedFrameAccumulator / g_fixedFrameTime;
                    //COM_ClampF32(&g_time.percentToNextFixedFrame, 0, 1);
                    g_screenInfo = g_renderer.R_SetupFrame(appWindow);
                    g_app.AppRender(&g_time, g_screenInfo);
                    if (g_debugInputActive)
                    {
                        Platform_R_DrawScene(&g_debugScene);
                    }
                    g_renderer.R_FinishFrame(appWindow);
                }

                #if 0 // previous implementation of timing outside app layer
                if (g_singleFrameStepMode == 1)
                {
                    // Keep sending input
                    Win32_SendAppInput();

                    if (g_singleFrameRun == 1)
                    {
                        g_singleFrameRun = 0;
                        // Force delta time or the game will get very confused
                        g_time.deltaTime = g_fixedFrameTime;
                        printf("\n**** PLATFORM Step into frame %d ****\n", g_time.platformFrameNumber);
						g_time.singleFrame = 1;
                        Win32_RunAppFrame();
                    }
                }
                else
                {
                    g_fixedFrameAccumulator += g_time.deltaTime;
                    if (g_fixedFrameAccumulator >= g_fixedFrameTime)
                    {
                        g_fixedFrameAccumulator -= g_fixedFrameTime;
                        g_time.deltaTime = g_fixedFrameTime;
					    g_time.singleFrame = 0;
                        Win32_SendAppInput();
                        Win32_RunAppFrame();
                    }
                }
				
                // Render
                if (g_rendererLink.moduleState == 1)
                {
                    g_time.percentToNextFixedFrame =
                        g_fixedFrameAccumulator / g_fixedFrameTime;
                    COM_ClampF32(&g_time.percentToNextFixedFrame, 0, 1);
                    g_screenInfo = g_renderer.R_SetupFrame(appWindow);
                    g_app.AppRender(&g_time, g_screenInfo);
                    if (g_debugInputActive)
                    {
                        Platform_R_DrawScene(&g_debugScene);
                    }
                    g_renderer.R_FinishFrame(appWindow);
                }
                #endif
            }
        }
        else
        {
            // Oh dear
            MessageBox(0, "Failed to acquire app window", "Error", MB_OK | MB_ICONINFORMATION);
            return 1;
        }
    }
    else
    {
        // Oh dear
        MessageBox(0, "Failed to register window class", "Error", MB_OK | MB_ICONINFORMATION);
        return 1;
    }
	
	if (g_app.isValid)
	{
		g_app.AppShutdown();
	}
    return 0;
}
