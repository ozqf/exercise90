#pragma once

#include "win32_main.h"

enum ZMouseMode mouseMode = Free;
enum ZMouseMode mouseState = Free;

#define Z_ALLOW_MOUSE_LOCK 1

void Win32_CaptureMouse()
{
    if (mouseState == Captured) { return; }
    mouseState = Captured;
    ShowCursor(false);
    RECT selfRect;
    GetWindowRect(appWindow, &selfRect);
    ClipCursor(&selfRect);
}

void Win32_ReleaseMouse()
{
    if (mouseState == Free) { return; }
    mouseState = Free;
    ShowCursor(true);
    ClipCursor(NULL);
}

void Win32_CheckMouseState()
{
    if (mouseMode == Captured && g_windowActive)
    {
        Win32_CaptureMouse();
    }
    else
    {
        Win32_ReleaseMouse();
    }
}

void Win32_SetMouseMode(enum ZMouseMode mode)
{
    // if (mode == mouseMode)
    // {
    //     return;
    // }
    mouseMode = mode;
    Win32_CheckMouseState();
}

void Win32_InitInput()
{
    //DebugBreak();

    g_win32_mousePos.x = 0;
    g_win32_mousePos.y = 0;
    g_win32_mousePosLast.x = 0;
    g_win32_mousePosLast.y = 0;
    g_win32_mousePosMove.x = 0;
    g_win32_mousePosMove.y = 0;

// Register a handle for receiving raw mouse input via WM_INPUT
// Copied from MSDN article
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    Rid[0].dwFlags = RIDEV_INPUTSINK;
    Rid[0].hwndTarget = appWindow;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}

void Win32_PollMouse(i32* ptrMoveX, i32* ptrMoveY)
{
#if 0
    // Output mouse position. Sampling occurred in WM_INPUT EVENT
    char buf[128];
    sprintf_s(buf, 128, "Mouse pos: %d, %d\nMove: %d, %d\n",
              g_win32_mousePos.x,
              g_win32_mousePos.y,
              g_win32_mousePosMove.x,
              g_win32_mousePosMove.y);
    OutputDebugString(buf);
#endif
    // input->mouseMovement[0] = g_win32_mousePosMove.x;
    // input->mouseMovement[1] = g_win32_mousePosMove.y;
    *ptrMoveX = g_win32_mousePosMove.x;
    *ptrMoveY = g_win32_mousePosMove.y;
    // Clear mouse input and accumulate for next read
    g_win32_mousePosMove.x = 0;
    g_win32_mousePosMove.y = 0;

    // static i32 lastMouseX;
    // static i32 lastMouseY;
#if 0
    if (GetCursorPos(&g_win32_mousePos))
    {
        if (ScreenToClient(appWindow, &g_win32_mousePos))
        {
            g_win32_mousePosMove.x = g_win32_mousePos.x - g_win32_mousePosLast.x;
            g_win32_mousePosMove.y = g_win32_mousePos.y - g_win32_mousePosLast.y;
            g_win32_mousePosLast.x = g_win32_mousePos.x;
            g_win32_mousePosLast.y = g_win32_mousePos.y;

            if (g_win32_mousePosMove.x > 9999 || g_win32_mousePosMove.y > 9999)
            {
                Assert(false);
            }

            input->mouse[0] = g_win32_mousePos.x;
	        input->mouse[1] = g_win32_mousePos.y;
	        input->mouseMovement[0] = g_win32_mousePosMove.x;
	        input->mouseMovement[1] = g_win32_mousePosMove.y;
            if (g_win32_mousePosMove.x != 0 || g_win32_mousePosMove.y != 0)
            {
                char buf[128];
                sprintf_s(buf, 128, "Mouse pos: %d, %d\nMove: %d, %d\n",
	        	    g_win32_mousePos.x,
    	        	g_win32_mousePos.y,
	        	    g_win32_mousePosMove.x,
	        	    g_win32_mousePosMove.y);
                OutputDebugString(buf);
            }

        }
    }
#endif
}

void Win32_SetMouseScreenPosition(i32* posX, i32* posY)
{
    if (GetCursorPos(&g_win32_mousePos))
    {
        if (ScreenToClient(appWindow, &g_win32_mousePos))
        {
            *posX = g_win32_mousePos.x;
            *posY = g_win32_mousePos.y;
            // input->mouse[0] = g_win32_mousePos.x;
            // input->mouse[1] = g_win32_mousePos.y;
        }
    }
}

void Win32_TickInput(ByteBuffer* cmdBuffer)
{
    i32 mouseMoveX = 0;
    i32 mouseMoveY = 0;
    i32 mousePosX = 0;
    i32 mousePosY = 0;
    
    InputEvent ev;

    if (mouseMode == Captured && g_windowActive)
    {
#if Z_ALLOW_MOUSE_LOCK
        RECT selfRect;
        GetWindowRect(appWindow, &selfRect);
        i32 halfWidth = (selfRect.right - selfRect.left) / 2;
        i32 halfHeight = (selfRect.bottom - selfRect.top) / 2;
        SetCursorPos(selfRect.left + halfWidth, selfRect.top + halfHeight);

        // SetCursorPos(selfRect.right - selfRect.left, selfRect.bottom - selfRect.top);

        // ClipCursor(&selfRect);
#endif
        Win32_PollMouse(&mouseMoveX, &mouseMoveY);
    }
    else
    {
        g_win32_mousePosMove.x = 0;
        g_win32_mousePosMove.y = 0;
        mouseMoveX = 0;
        mouseMoveY = 0;
    }
#if 0
	if (mouseMoveX != 0 || mouseMoveY != 0)
	{
		OutputDebugStringA("Mouse moved\n");
	}
#endif

    BufferItemHeader header = {};
    header.type = PLATFORM_EVENT_CODE_INPUT;
    header.size = sizeof(InputEvent);

    ev = NewInputEvent(Z_INPUT_CODE_MOUSE_MOVE_X, mouseMoveX);
    Buf_WriteObject(cmdBuffer, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
    
    ev = NewInputEvent(Z_INPUT_CODE_MOUSE_MOVE_Y, mouseMoveY);
    Buf_WriteObject(cmdBuffer, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
    
    ev = NewInputEvent(Z_INPUT_CODE_MOUSE_POS_X, mousePosX);
    Buf_WriteObject(cmdBuffer, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
    
    ev = NewInputEvent(Z_INPUT_CODE_MOUSE_POS_Y, mousePosY);
    Buf_WriteObject(cmdBuffer, (u8*)&ev, PLATFORM_EVENT_CODE_INPUT, sizeof(InputEvent));
}

u32 Win32_KeyCode_To_Input_Code(u32 vkCode)
{
    if (vkCode == 'A')              return Z_INPUT_CODE_A;
    if (vkCode == 'B')              return Z_INPUT_CODE_B;
    if (vkCode == 'C')              return Z_INPUT_CODE_C;
    if (vkCode == 'D')              return Z_INPUT_CODE_D;
    if (vkCode == 'E')              return Z_INPUT_CODE_E;
    if (vkCode == 'F')              return Z_INPUT_CODE_F;
    if (vkCode == 'G')              return Z_INPUT_CODE_G;
    if (vkCode == 'H')              return Z_INPUT_CODE_H;
    if (vkCode == 'I')              return Z_INPUT_CODE_I;
    if (vkCode == 'J')              return Z_INPUT_CODE_J;
    if (vkCode == 'K')              return Z_INPUT_CODE_K;
    if (vkCode == 'L')              return Z_INPUT_CODE_L;
    if (vkCode == 'M')              return Z_INPUT_CODE_M;
    if (vkCode == 'N')              return Z_INPUT_CODE_N;
    if (vkCode == 'O')              return Z_INPUT_CODE_O;
    if (vkCode == 'P')              return Z_INPUT_CODE_P;
    if (vkCode == 'Q')              return Z_INPUT_CODE_Q;
    if (vkCode == 'R')              return Z_INPUT_CODE_R;
    if (vkCode == 'S')              return Z_INPUT_CODE_S;
    if (vkCode == 'T')              return Z_INPUT_CODE_T;
    if (vkCode == 'U')              return Z_INPUT_CODE_U;
    if (vkCode == 'V')              return Z_INPUT_CODE_V;
    if (vkCode == 'W')              return Z_INPUT_CODE_W;
    if (vkCode == 'X')              return Z_INPUT_CODE_X;
    if (vkCode == 'Y')              return Z_INPUT_CODE_Y;
    if (vkCode == 'Z')              return Z_INPUT_CODE_Z;
    if (vkCode == '0')              return Z_INPUT_CODE_0;
    if (vkCode == '1')              return Z_INPUT_CODE_1;
    if (vkCode == '2')              return Z_INPUT_CODE_2;
    if (vkCode == '3')              return Z_INPUT_CODE_3;
    if (vkCode == '4')              return Z_INPUT_CODE_4;
    if (vkCode == '5')              return Z_INPUT_CODE_5;
    if (vkCode == '6')              return Z_INPUT_CODE_6;
    if (vkCode == '7')              return Z_INPUT_CODE_7;
    if (vkCode == '8')              return Z_INPUT_CODE_8;
    if (vkCode == '9')              return Z_INPUT_CODE_9;
    if (vkCode == VK_ESCAPE)        return Z_INPUT_CODE_ESCAPE;
    if (vkCode == VK_UP)            return Z_INPUT_CODE_UP;
    if (vkCode == VK_DOWN)          return Z_INPUT_CODE_DOWN;
    if (vkCode == VK_LEFT)          return Z_INPUT_CODE_LEFT;
    if (vkCode == VK_RIGHT)         return Z_INPUT_CODE_RIGHT;
    if (vkCode == VK_SPACE)         return Z_INPUT_CODE_SPACE;
    if (vkCode == VK_CONTROL)       return Z_INPUT_CODE_CONTROL;
    if (vkCode == VK_SHIFT)         return Z_INPUT_CODE_SHIFT;

    return Z_INPUT_CODE_NULL;
}
