/**********************************************************************
 * Core implementation of Windows platform layer
 *********************************************************************/
#pragma once

#include <stdio.h>

#include "win32_main.h"

#include "win32_timing.cpp"
#include "win32_input.cpp"
#include "win32_draw.cpp"
#include "win32_debug.cpp"
#include "win32_data.cpp"
#include "win32_fileIO.h"
#include "win32_gl/win32_gl_interface.h"

// Last as it probably calls into the others
#include "win32_app_interface.cpp"

/****************************************************************
Command line
****************************************************************/

void Win32_ReadCommandLine(LPSTR lpCmdLine)
{
    while (*lpCmdLine && (numLaunchParams < MAX_LAUNCH_PARAMS))
    {
        // While not null && If not a valid ascii character (spaces etc) move until a printable character is hit
        while (*lpCmdLine && ((*lpCmdLine <= 32) && (*lpCmdLine > 126)))
        {
            lpCmdLine++;
        }

        if (*lpCmdLine)
        {
            // Mark a new token start
            launchParams[numLaunchParams] = lpCmdLine;
            numLaunchParams++;

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
    launchParams[numLaunchParams] = NULL;
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
internal LRESULT CALLBACK Win32_MainWindowCallback(
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
        InputEvent ev = NewInputEvent(Z_INPUT_CODE_MOUSE_1, 1);
        Buf_WriteObject(&g_cmdBuf, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
    } break;

    case WM_LBUTTONUP:
    {
        InputEvent ev = NewInputEvent(Z_INPUT_CODE_MOUSE_1, 0);
        Buf_WriteObject(&g_cmdBuf, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
    } break;

    case WM_RBUTTONDOWN:
    {
        InputEvent ev = NewInputEvent(Z_INPUT_CODE_MOUSE_2, 1);
        Buf_WriteObject(&g_cmdBuf, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
    } break;

    case WM_RBUTTONUP:
    {
        InputEvent ev = NewInputEvent(Z_INPUT_CODE_MOUSE_2, 0);
        Buf_WriteObject(&g_cmdBuf, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
    } break;

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
    } break;

    case WM_KILLFOCUS:
    {
        g_windowActive = 0;
        Win32_CheckMouseState();
    } break;

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
            Win32_DebugReadKey(VKCode, lParam);
            break;
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

        // Write to command buffer
        // TODO: Not thread safe or anything!
        u32 inputCode = Win32_KeyCode_To_Input_Code(VKCode);
        if (inputCode != 0)
        {
            InputEvent ev = NewInputEvent(inputCode, (i32)isDown);
            Buf_WriteObject(&g_cmdBuf, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
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

/**********************************************************************
 * EXECUTE TEXT COMMAND
 *********************************************************************/
u8 Win32_ExecTestCommand(char* str, char** tokens, i32 numTokens)
{
    if (COM_CompareStrings(tokens[0], "QUIT") == 0 || COM_CompareStrings(tokens[0], "EXIT") == 0)
    {
        Win32_Shutdown();
        return 1;
    }
    else if (COM_CompareStrings(tokens[0], "HELP") == 0)
    {
        printf("--- Command List ---\n");
        printf("  QUIT or EXIT - shutdown\n");
        printf("  VERSION - show version info\n");
        printf("  MANIFEST - List data files entries\n");
        printf("  RESTART APP/RENDERER/SOUND/GAME - Reload subsystem\n");
        return 0;
    }
	else if (COM_CompareStrings(tokens[0], "VERSION") == 0)
	{
		printf("PLATFORM Built %s: %s\n", __DATE__, __TIME__);
		return 1;
	}
    else if (COM_CompareStrings(tokens[0], "MANIFEST") == 0)
    {
        Win32_DebugPrintDataManifest();
        return 1;
    }
    else if (COM_CompareStrings(tokens[0], "RESTART") == 0)
    {
        if (numTokens != 2)
        {
            printf("  Must specify: APP, RENDERER or SOUND\n");
            return 1;
        }

        if (COM_CompareStrings(tokens[1], "APP") == 0)
        {
            printf("  Restarting app\n");
            g_appLink.timestamp = {};
            return 1;
        }
        else if (COM_CompareStrings(tokens[1], "RENDERER") == 0)
        {
            printf("  Restarting renderer\n");
            g_rendererLink.timestamp = {};
            return 1;
        }
        else if (COM_CompareStrings(tokens[1], "SOUND") == 0)
        {
            printf("  Restarting sound\n");
            g_soundLink.timestamp = {};
            return 1;
        }
    }
    return 0;
}

void Win32_ParseTextCommand(char* str, i32 firstChar, i32 length)
{
    if (length <= 1)
    {
        return;
    }
    COM_ZeroMemory((u8*)g_textCommandBuffer, TEXT_COMMAND_BUFFER_SIZE);
    COM_COPY(str, g_textCommandBuffer, length);
    //printf("EXEC \"%s\" (%d chars)\n", g_textCommandBuffer, length);

    char copy[128];
    char* tokens[32];

    i32 tokensCount = COM_ReadTokens(g_textCommandBuffer, copy, tokens);
    if (tokensCount == 0)
    {
        return;
    }
#if 0
    printf("PRINT TOKENS (%d)\n", tokensCount);
    for (int i = 0; i < tokensCount; ++i)
    {
        //char* str2 = (char*)(copy + tokens[i]);
        printf("%s\n", tokens[i]);
    }
#endif
    if (!Win32_ExecTestCommand(g_textCommandBuffer, tokens, tokensCount))
    {
        if (!g_app.AppParseCommandString(g_textCommandBuffer, tokens, tokensCount))
        {
            printf(" Unknown command %s\n", g_textCommandBuffer);
        }
    }
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
    // Spawn debugging windows cmd
    FILE* stream;
    AllocConsole();
    freopen_s(&stream, "conin$","r",stdin);
    freopen_s(&stream, "conout$","w",stdout);
    freopen_s(&stream, "conout$","w",stderr);
    consoleHandle = GetConsoleWindow();
    MoveWindow(consoleHandle,1,1,680,480,1);
    printf("[%s] Console initialized.\n", __FILE__);




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

    i32 windowWidth = 1280;
    i32 windowHeight = 720;

    RECT r;
    r.top = r.left = 0;
    r.right = windowWidth;
    r.bottom = windowHeight;


    ///////////////////////////////////////////////////////////////////////////////////////////

    // Adjust desired rect to include area of window including border.
    // top or left may well now be negative
    //
    AdjustWindowRect(&r, WindowClass.style, false);

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
            g_gameTime = {};

            // Make sure assets are ready before scene!
            SharedAssets_Init();

            //Win32_CheckApplicationLink();
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

#if 0
            if (!Win32_LinkToAppStub())
            {
                MessageBox(0, "Failed to attach to app stub", "Error", MB_OK | MB_ICONINFORMATION);
                return 1;
            }
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

#if 0 // Force app to give up CPU time if not current focus target
                if (g_windowActive == 0)
                {
                    Sleep(50);
                }
#endif

                float newTime = Win32_FloatTime();
                g_gameTime.deltaTime = newTime - previousTime;
                previousTime = newTime;

                g_gameTime.frameNumber++;

                // Check App reload
                g_appLink.checkTick -= g_gameTime.deltaTime;
                if (g_appLink.checkTick <= 0)
                {
                    g_appLink.checkTick = 0.1f;
                    if (Win32_CheckFileModified(g_appLink.path, &g_appLink.timestamp))
                    {
                        Win32_LinkToApplication();
                    }
                }
                // Check renderer reload
                g_rendererLink.checkTick -= g_gameTime.deltaTime;
                if (g_rendererLink.checkTick <= 0)
                {
                    g_rendererLink.checkTick = 0.1f;
                    if (Win32_CheckFileModified(g_rendererLink.path, &g_rendererLink.timestamp))
                    {
                        //DebugBreak();
                        if (Win32_LinkToRenderer() && g_app.isvalid)
                        {
                            g_app.AppRendererReloaded();
                        }
                    }
                }
                // Check sound reload
                g_soundLink.checkTick -= g_gameTime.deltaTime;
                if (g_soundLink.checkTick <= 0)
                {
                    g_soundLink.checkTick = 0.1f;
                    if (Win32_CheckFileModified(g_soundLink.path, &g_soundLink.timestamp))
                    {
                        //DebugBreak();
                        // if (Win32_LinkToSound() && g_app.isvalid)
                        // {
                        //     g_app.AppSoundReloaded();
                        // }
                    }
                }

                // Keeping this, helped me find a buffer overrun due to crazy timing behaviour
                if (g_gameTime.deltaTime < 0)
                {
                    MessageBox(0, "Error: Negative timeDelta", "Error", MB_OK | MB_ICONINFORMATION);
                    return 1;
                }
                
                // char buf[64];
                // sprintf_s(buf, 64, "Total time: %3.7f. DeltaTime: %3.7f\n", newTime, time.deltaTime);
                // OutputDebugString(buf);

                Win32_TickInput(&g_cmdBuf);

                // Called before app update as app update will call the renderer
                // this should be buffered up so the two can be desynced!
                if (g_rendererLink.moduleState == 1)
                {
                    g_renderer.R_SetupFrame(appWindow);  
                }

                //////////////////////////////////////////////////////////////////
                // Command handling:
                // > Mark buffer as ended
                // > copy current state of command buffer
                // > Swap platform buffer between A and B
                // > Send copy to app
                //////////////////////////////////////////////////////////////////
                //*(u8*)g_cmdBuf.ptrWrite = 0;
                g_cmdBuf.ptrEnd = g_cmdBuf.ptrWrite;
                ByteBuffer frameCommands = g_cmdBuf;
				//OutputDebugStringA("Swap platform buffers\n");
                if (g_cmdBuf.ptrStart == g_commandBufferA)
                {
                    g_cmdBuf.ptrStart = g_commandBufferB;
                    g_cmdBuf.ptrWrite = g_commandBufferB;
                    g_cmdBuf.ptrEnd = g_commandBufferB;
					g_cmdBuf.count = 0;
                }
                else if (g_cmdBuf.ptrStart == g_commandBufferB)
                {
                    g_cmdBuf.ptrStart = g_commandBufferA;
                    g_cmdBuf.ptrWrite = g_commandBufferA;
                    g_cmdBuf.ptrEnd = g_commandBufferA;
					g_cmdBuf.count = 0;
                }
                else
                {
                    ILLEGAL_CODE_PATH
                }

                //Win32_R_SetupFrame(appWindow);
                if (g_app.isvalid)
                {
                    g_app.AppUpdate(&g_gameTime, frameCommands);
                }
				//OutputDebugStringA("Zero platform buffer\n");
				COM_ZeroMemory(frameCommands.ptrStart, frameCommands.capacity);
                
                if (g_debugInputActive)
                {
                    Platform_R_DrawScene(&g_debugScene);
                }
                
                if (g_rendererLink.moduleState == 1)
                {
                    g_renderer.R_FinishFrame(appWindow);
                }

                Win32_CheckTextBuffer();
                //Win32_R_FinishFrame(appWindow);
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
    return 0;
}
