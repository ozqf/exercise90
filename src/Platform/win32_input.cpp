#pragma once

#include "win32_main.h"


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
    #define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
    #endif
    #ifndef HID_USAGE_GENERIC_MOUSE
    #define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
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
#if 1
    // Output mouse position. Sampling occurred in WM_INPUT EVENT
    char buf[128];
    sprintf_s(buf, 128, "Mouse pos: %d, %d\nMove: %d, %d\n",
              g_win32_mousePos.x,
              g_win32_mousePos.y,
              g_win32_mousePosMove.x,
              g_win32_mousePosMove.y);
    OutputDebugString(buf);
	input->mouseMovement[0] = g_win32_mousePosMove.x;
	input->mouseMovement[1] = g_win32_mousePosMove.y;
	// Clear mouse input and accumulate for next read
	g_win32_mousePosMove.x = 0;
	g_win32_mousePosMove.y = 0;
#endif
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
