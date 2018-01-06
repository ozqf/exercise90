/**********************************************************************
 * Core implementation of Windows platform layer
 *********************************************************************/
#pragma once

#include "../Shared/shared.h"
#include "../interface/platform_interface.h"
#include "../interface/app_interface.h"
#include "../interface/app_stub.h"

#include "win32_main.h"
#include "win32_draw.cpp"
#include "win32_debug.cpp"
#include "win32_gl.h"

#include <windows.h>
#include <stdio.h>

global_variable PlatformInterface platInterface;
global_variable AppInterface app;
global_variable void* gameModule;
global_variable char* appModulePath = "base/gamex86.dll";

global_variable bool globalRunning = true;

global_variable MemoryBlock gameMemory;

global_variable HWND appWindow;

global_variable win32_offscreen_buffer globalBackBuffer;

global_variable int xOffset = 0;
global_variable int yOffset = 0;

/**********************************************************************
 * TIMING
 *********************************************************************/
global_variable double global_timePassed = 0;
global_variable double global_secondsPerTick = 0;
global_variable __int64 global_timeCount = 0;
global_variable LARGE_INTEGER tick, tock;

f32 Win32_InitFloatTime()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    global_secondsPerTick = 1.0 / (double)frequency.QuadPart;
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    global_timeCount = counter.QuadPart;
    return 0;
}

f32 Win32_FloatTime()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    __int64 interval = counter.QuadPart - global_timeCount;
    global_timeCount = counter.QuadPart;

    double secondsGoneBy = (double)interval * global_secondsPerTick;
    global_timePassed += secondsGoneBy;
    return (float)global_timePassed;
}

/****************************************************************
Command line
****************************************************************/
// Nice array of points to the start of each token in the launch param string
#define MAX_LAUNCH_PARAMS 50
global_variable char *launchParams[MAX_LAUNCH_PARAMS];
i32 numLaunchParams = 0;

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

/**********************************************************************
 * Attach to application DLL
 *********************************************************************/

i32 Win32_GetViewPortMinX() { return 0; }
i32 Win32_GetViewPortMinY() { return 0; }
i32 Win32_GetViewPortMaxX() { return 1280; }
i32 Win32_GetViewPortMaxY() { return 768; }

void Win32_InitPlatformInterface()
{
    
}

void Win32_LinkToApplication()
{
    app = GetAppInterfaceStub(platInterface);
}

/****************************************************************
ALLOC MAIN MEMORY
TODO: Return error code if it fails?
****************************************************************/
void Win32_Alloc(MemoryBlock *mem)
{
    i32 size = MegaBytes(2);
    mem->size = size;
    mem->ptrMemory = VirtualAlloc(0, mem->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}

void Win32_Free(MemoryBlock *mem)
{
    if (mem)
    {
        // Reset to 0
        char* cursor = (char*)mem->ptrMemory;
        for (int i = 0; i < mem->size; ++i)
        {
            *cursor = 0;
            cursor++;
        }
        VirtualFree(mem->ptrMemory, 0, MEM_RELEASE);
        mem->ptrMemory = 0;
    }
}

/**********************************************************************
 * PRIMITIVE FILE I/O
/*********************************************************************/
void Win32_FreeFileMemory(void* mem)
{
    VirtualFree(mem, 0, MEM_RELEASE);
}

void * Win32_ReadEntireFile(char *fileName)
{
	void *result = 0;
	HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER fileSize;
		if (GetFileSizeEx(fileHandle, &fileSize))
		{
			LPDWORD bytesRead = 0;
			u32 fileSize32 = SafeTruncateUInt64(fileSize.QuadPart);
			result = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (result)
			{
				if (ReadFile(fileHandle, result, fileSize32, bytesRead, 0) && fileSize32 == (u32)bytesRead)
				{
					// File read successfully
				}
				else
				{
					Win32_FreeFileMemory(result);
					result = 0;
				}
			}
			else
			{
				// TODO: Logging
			}
		}
		
		CloseHandle(fileHandle);
	}
	return result;
}

bool32 Win32_WriteEntireFile(char *fileName, u32 memorySize, void *memory)
{
	bool32 result = false;

	HANDLE fileHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		DWORD bytesWritten;
		if (WriteFile(fileHandle, memory, memorySize, &bytesWritten, 0))
		{
			// Make sure entire file was written
			result = (bytesWritten == memorySize);
		}
		else
		{
			// TODO logging
		}
	}
	else
	{
		// TODO logging
	}
	return result;
}

void Win32_PrintDebug(char* str)
{
    OutputDebugStringA(str);
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
    } break;

    case WM_ACTIVATEAPP:
    {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
    } break;

    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
        u32 VKCode = wParam;
        bool wasDown = ((lParam & (1 << 30)) != 0);
        bool isDown = ((lParam & (1 << 31)) == 0);
        if (VKCode == 'W')
        {
            OutputDebugStringA("W\n");
            printf("Up");
            //toneHz += 10;
        }
        else if (VKCode == 'A' && isDown)
        {
            xOffset++;
            printf("Left");
        }
        else if (VKCode == 'S')
        {
            //toneHz -= 10;
            printf("Down");
        }
        else if (VKCode == 'D')
        {
            xOffset--;
            printf("Right");
        }
        else if (VKCode == 'Q')
        {
        }
        else if (VKCode == 'E')
        {
        }
        else if (VKCode == VK_UP)
        {
        }
        else if (VKCode == VK_DOWN)
        {
        }
        else if (VKCode == VK_LEFT)
        {
        }
        else if (VKCode == VK_RIGHT)
        {
        }
        else if (VKCode == VK_SPACE)
        {
            //printf("Spacebar superstar\n");
            //global_timePassed = 0;
            //MessageBox(0, "You hit the space bar!!.", "Test", MB_OK | MB_ICONINFORMATION);
        }
        else if (VKCode == VK_ESCAPE)
        {
            if (isDown)
            {
                OutputDebugStringA("ESC is down");
            }
            if (wasDown)
            {
                OutputDebugStringA("ESC was down ");
            }
        }

        if (VKCode == VK_F4 && (lParam & (1 << 29)))
        {
            globalRunning = false;
        }
    } break;

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
	MessageBox(0, "Start breakpoint", "Started", MB_OK | MB_ICONINFORMATION);

    Win32_ReadCommandLine(lpCmdLine);

    InitDebug();
    printf("Debug init\n");
    printf("File %s, line: %d\n", __FILE__, __LINE__);
    gameMemory = {};
    Win32_Alloc(&gameMemory);

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

    Win32_InitPlatformInterface();
    Win32_LinkToApplication();

    // register window class, returns an atom. 0 if register failed
    if (RegisterClass(&WindowClass))
    {
        appWindow = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            "Exercise 90",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, // window style
            CW_USEDEFAULT,  // Default position x
            CW_USEDEFAULT,  // Default position y
            r.right - r.left, // CW_USEDEFAULT,
            r.bottom - r.top, // CW_USEDEFAULT,
            0,
            0,
            hInstance,
            0);
        
        if (appWindow)
        {
            if (InitOpenGL(appWindow) == false)
            {
                MessageBox(0, "InitOpenGL failed", "Error", MB_OK | MB_ICONINFORMATION);
                globalRunning = false;
            }

            float previousTime = Win32_InitFloatTime();

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

                float newTime = Win32_FloatTime();
                float deltaTime = newTime - previousTime;
                previousTime = newTime;
                
                // char buf[64];
                // sprintf_s(buf, 64, "Total time: %3.7f. DeltaTime: %3.7f\n", newTime, deltaTime);
                // OutputDebugString(buf);
                
                Win32RenderFrame(appWindow);

                /* Stuff to add:
                > PlatformReadNetworkPackets();
                    > game->ReadNetworkPackets();

                > game->Frame(time);
                > game->FixedFrame(time);

                Render();
                */
            }
        }
        else
        {
            // Oh dear
        }
    }
    else
    {
        // Oh dear
    }
}