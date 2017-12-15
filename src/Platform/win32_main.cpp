#ifndef WIN32_MAIN_CPP
#define WIN32_MAIN_CPP

#include "../Shared/shared.h"
#include "win32_main.h"
#include "win32_draw.cpp"
#include <windows.h>

bool globalRunning = true;

global_variable MemoryBlock gameMemory;

global_variable win32_offscreen_buffer globalBackBuffer;

global_variable int xOffset = 0;
global_variable int yOffset = 0;

/****************************************************************
ALLOC MAIN MEMORY
****************************************************************/
void PlatformAlloc(MemoryBlock *mem)
{
    mem->size = 1024;
    mem->ptrMemory = VirtualAlloc(0, mem->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
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
            //toneHz += 10;
        }
        else if (VKCode == 'A' && isDown)
        {
            xOffset++;
        }
        else if (VKCode == 'S')
        {
            //toneHz -= 10;
        }
        else if (VKCode == 'D')
        {
            xOffset--;
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
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";

    // register window class, returns an atom. 0 if register failed
    if (RegisterClass(&WindowClass))
    {
        HWND window = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            "Handmade Hero",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            hInstance,
            0);
        MessageBox(0, "Message Box.", "Msg Bx", MB_OK | MB_ICONINFORMATION);
        if (window)
        {

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
            }
        }
    }
}

#endif