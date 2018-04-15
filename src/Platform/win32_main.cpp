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
internal LRESULT CALLBACK Win32_MainWindowCallback(HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

#if 0
    case WM_MOUSEMOVE:
    {
        POINT mouseP;
        if (GetCursorPos(&mouseP))
        {
            if (ScreenToClient(appWindow, &mouseP))
            {
                char buf[64];
                sprintf_s(buf, 64, "Mouse pos: %d, %d\n", mouseP.x, mouseP.y);
                OutputDebugString(buf);
            }
        }
    }
    break;
#endif

    case WM_LBUTTONDOWN:
    {
        inputTick.attack1 = 1;
    } break;

    case WM_LBUTTONUP:
    {
        inputTick.attack1 = 0;
    } break;

    case WM_RBUTTONDOWN:
    {
        inputTick.attack2 = 1;
    } break;

    case WM_RBUTTONUP:
    {
        inputTick.attack2 = 0;
    } break;

    case WM_CLOSE:
    {
        //PostQuitMessage(0);
        // TODO: Handle this with a message to the user?
        Win32_Shutdown();
        OutputDebugStringA("WM_CLOSE\n");
    }
    break;

    case WM_DESTROY:
    {
        // Handle this as an error - recreate window?
        Win32_Shutdown();
        OutputDebugStringA("WM_DESTROY\n");
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
        OutputDebugStringA("WM_ACTIVATEAPP\n");
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
        if (VKCode == 'R')
        {
            inputTick.reset = isDown;
        }
        else if (VKCode == 'Z' && isDown)
        {
            //Win32_R_Shutdown();
            Win32_CloseRendererLink();
        }
        else if (VKCode == 'X' && isDown)
        {
            //Win32_InitOpenGL(appWindow);
            //app.AppRendererReloaded();
            Win32_LinkToRenderer();
            if (app.isvalid)
            {
                app.AppRendererReloaded();
            }
        }
        else if (VKCode == 'P' && isDown)
        {
            MessageBox(0, "Dead stop!", "Stop!", MB_OK | MB_ICONINFORMATION);
        }
        else if (VKCode == 'T')
        {
            inputTick.debug_cycleTexture = isDown;
            // //global_timePassed = 0;
            // //MessageBox(0, "You hit the space bar!!.", "Test", MB_OK | MB_ICONINFORMATION);
            // if (wasDown && !isDown)
            // {
            //     //global_timePassed = 0;
            //     //MessageBox(0, "You hit the space bar!!.", "Test", MB_OK | MB_ICONINFORMATION);

            //     ++g_gl_primitive_mode;
            //     if (g_gl_primitive_mode >= NUM_GL_PRIMITIVE_MODES)
            //     {
            //         g_gl_primitive_mode = 0;
            //     }
            //     char output[256];
            //     sprintf_s(output, "g_gl_primitive_mode: %d\n", g_gl_primitive_mode);
            //     OutputDebugStringA(output);
            // }
        }
        else if (VKCode == 'W')
        {
            inputTick.moveForward = isDown;
            // if (isDown)
            // {
            //     testInput.movement.z  = 1;
            // }
            //OutputDebugStringA("W\n");
            //printf("Up\n");
            //toneHz += 10;
        }
        else if (VKCode == 'A')
        {
            inputTick.moveLeft = isDown;
            //xOffset++;
            //printf("Left\n");
        }
        else if (VKCode == 'S')
        {
            inputTick.moveBackward = isDown;
            // if (isDown)
            // {
            //     testInput.movement.z  = -1;
            // }
            //toneHz -= 10;
            //printf("Down\n");
        }
        else if (VKCode == 'D')
        {
            inputTick.moveRight = isDown;
            //xOffset--;
            //printf("Right\n");
        }
        else if (VKCode == 'Q')
        {
            inputTick.rollLeft = isDown;
        }
        else if (VKCode == 'E')
        {
            inputTick.rollRight = isDown;
        }
        else if (VKCode == VK_UP)
        {
            inputTick.pitchUp = isDown;
        }
        else if (VKCode == VK_DOWN)
        {
            inputTick.pitchDown = isDown;
        }
        else if (VKCode == VK_LEFT)
        {
            inputTick.yawLeft = isDown;
        }
        else if (VKCode == VK_RIGHT)
        {
            inputTick.yawRight = isDown;
        }
        else if (VKCode == VK_SPACE)
        {
            inputTick.moveUp = isDown;
        }
        else if (VKCode == VK_CONTROL)
        {
            inputTick.moveDown = isDown;
        }
        else if (VKCode == VK_ESCAPE)
        {
            inputTick.escape = isDown;
        }

        if (VKCode == VK_F4 && (lParam & (1 << 29)))
        {
            globalRunning = false;
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
 * WIN32 ENTRY POINT
 *********************************************************************/
int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    //MessageBox(0, "Start breakpoint", "Started", MB_OK | MB_ICONINFORMATION);
    //DebugBreak();

    Win32_ReadCommandLine(lpCmdLine);
    //Assert(false);

    InitDebug();
    printf("Debug init\n");
    printf("File %s, line: %d\n", __FILE__, __LINE__);

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
    r.right = 1280;
    r.bottom = 720;

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

            float previousTime = Win32_InitFloatTime();
            g_gameTime = {};

            // Make sure assets are ready before scene!
            SharedAssets_Init();

            // Init interfaces and attach external stuff
            Win32_InitPlatformInterface();

            //Win32_CheckApplicationLink();
#if 1
            if (!Win32_LinkToApplication())
            {
                return 1;
            }
            // Set timestamp so that we don't instantly reload the app
            Win32_UpdateFileTimeStamp(appModulePath, &g_appModuleTimestamp);
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

                if (g_windowActive == 0)
                {
                    Sleep(50);
                }

                float newTime = Win32_FloatTime();
                g_gameTime.deltaTime = newTime - previousTime;
                previousTime = newTime;

                g_gameTime.frameNumber++;

                g_checkAppReloadTick -= g_gameTime.deltaTime;
                if (g_checkAppReloadTick <= 0)
                {
                    g_checkAppReloadTick = 1;
                    if (Win32_CheckFileModified(appModulePath, &g_appModuleTimestamp))
                    {
                        Win32_LinkToApplication();
                    }
                    //Win32_CheckApplicationLink();
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

                Win32_TickInput(&inputTick);

                if (renderModuleState == 1)
                {
                    renderer.R_SetupFrame(appWindow);  
                }
                //Win32_R_SetupFrame(appWindow);
                if (app.isvalid)
                {
                    app.AppUpdate(&g_gameTime, &inputTick);
                }
                
                if (renderModuleState == 1)
                {
                    renderer.R_FinishFrame(appWindow);
                }
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
