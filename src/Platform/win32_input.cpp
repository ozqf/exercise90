#pragma once

#include "win32_main.h"

enum ZMouseMode mouseMode = Free;

#define Z_ALLOW_MOUSE_LOCK 1

void Win32_SetMouseMode(enum ZMouseMode mode)
{
    if (mode == mouseMode)
    {
        return;
    }
    mouseMode = mode;
    #if Z_ALLOW_MOUSE_LOCK
    if (mouseMode == Free)
    {
        ShowCursor(true);
        ClipCursor(NULL);
    }
    else
    {
        ShowCursor(false);
        RECT selfRect;
        GetWindowRect(appWindow, &selfRect);
        ClipCursor(&selfRect);
    }
    #endif
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

void Win32_PollMouse(InputTick *input)
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
    input->mouseMovement[0] = g_win32_mousePosMove.x;
    input->mouseMovement[1] = g_win32_mousePosMove.y;
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

void Win32_SetMouseScreenPosition(InputTick *input)
{
    if (GetCursorPos(&g_win32_mousePos))
    {
        if (ScreenToClient(appWindow, &g_win32_mousePos))
        {
            input->mouse[0] = g_win32_mousePos.x;
            input->mouse[1] = g_win32_mousePos.y;
        }
    }
}

void Win32_TickInput(InputTick *input)
{
    if (mouseMode == Captured)
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
        Win32_PollMouse(input);
    }
    else
    {
        inputTick.mouseMovement[0] = 0;
        inputTick.mouseMovement[1] = 0;
        Win32_SetMouseScreenPosition(input);
    }
}
