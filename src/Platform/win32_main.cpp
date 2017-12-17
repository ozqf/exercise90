#ifndef WIN32_MAIN_CPP
#define WIN32_MAIN_CPP

#include "../Shared/shared.h"
#include "win32_main.h"
#include "win32_draw.cpp"
#include "win32_debug.cpp"

#include <gl/gl.h>
#include "win32_gl.h"

#include <windows.h>
#include <stdio.h>

bool globalRunning = true;

global_variable MemoryBlock gameMemory;

global_variable HWND appWindow;

global_variable win32_offscreen_buffer globalBackBuffer;

global_variable int xOffset = 0;
global_variable int yOffset = 0;

/****************************************************************
ALLOC MAIN MEMORY
TODO: Return error code if it fails?
****************************************************************/
void PlatformAlloc(MemoryBlock *mem)
{
    i32 size = MegaBytes(2);
    mem->size = size;
    mem->ptrMemory = VirtualAlloc(0, mem->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}

void PlatformFree(MemoryBlock *mem)
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

void PlatformFreeFileMemory(void* mem)
{
    VirtualFree(mem, 0, MEM_RELEASE);
}

void * PlatformReadEntireFile(char *fileName)
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
					PlatformFreeFileMemory(result);
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

bool32 PlatformWriteEntireFile(char *fileName, u32 memorySize, void *memory)
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
internal LRESULT CALLBACK Win32MainWindowCallback(HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        globalRunning = false;
        OutputDebugStringA("WM_CLOSE\n");
    }
    break;

    case WM_DESTROY:
    {
        // Handle this as an error - recreate window?
        globalRunning = false;
        OutputDebugStringA("WM_DESTROY\n");
    }
    break;

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
            printf("Spacebar superstar\n");
            MessageBox(0, "You hit the space bar!!.", "Test", MB_OK | MB_ICONINFORMATION);
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
/*********************************************************************/
int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
	MessageBox(0, "Start breakpoint", "Started", MB_OK | MB_ICONINFORMATION);

    InitDebug();
    printf("Debug init\n");
    printf("File %s, line: %d\n", __FILE__, __LINE__);
    gameMemory = {};
    PlatformAlloc(&gameMemory);

    // Created basic window
    WNDCLASS WindowClass = {}; // {} initialises all variables to 0 (cpp only)
    WindowClass.style =
        CS_OWNDC // allocates a unique device context for each window in the window class
        | CS_HREDRAW | CS_VREDRAW;

    Win32ResizeDIBSection(&globalBackBuffer, 1280, 720);

    //win32LoadXInput();
    
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = hInstance;
    //	WindowClass.hIcon
    WindowClass.lpszClassName = "Exercise90WindowClass";

    // register window class, returns an atom. 0 if register failed
    if (RegisterClass(&WindowClass))
    {
        appWindow = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            "Exercise 90",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
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

#endif