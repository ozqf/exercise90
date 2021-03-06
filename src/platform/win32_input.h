#pragma once

#include "win32_module.cpp"

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
    mouseMode = mode;
    Win32_CheckMouseState();
}

void Win32_InitInput()
{
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
    *ptrMoveX = g_win32_mousePosMove.x;
    *ptrMoveY = g_win32_mousePosMove.y;
    // Clear mouse input and accumulate for next read
    g_win32_mousePosMove.x = 0;
    g_win32_mousePosMove.y = 0;
}

void Win32_SetMouseScreenPosition(i32* posX, i32* posY)
{
    if (GetCursorPos(&g_win32_mousePos))
    {
        if (ScreenToClient(appWindow, &g_win32_mousePos))
        {
            *posX = g_win32_mousePos.x;
            *posY = g_win32_mousePos.y;
        }
    }
}

void Win32_SendMouseScreenPosition(ByteBuffer* cmdBuffer)
{
    i32 mousePosX = 0;
    i32 mousePosY = 0;

    
    RECT selfRect;
    GetClientRect(appWindow, &selfRect);
    i32 width = (selfRect.right - selfRect.left);
    i32 height = (selfRect.bottom - selfRect.top);
    i32 halfWidth = width / 2;
    i32 halfHeight = height / 2;
    
    Win32_SetMouseScreenPosition(&mousePosX, &mousePosY);

    SysInputEvent ev;

    Sys_CreateInputEvent(&ev, Z_INPUT_CODE_MOUSE_POS_X, mousePosX);
    Sys_EnqueueEvent(cmdBuffer, SYS_CAST_EVENT_TO_BASE(&ev));

    Sys_CreateInputEvent(&ev, Z_INPUT_CODE_MOUSE_POS_Y, mousePosY);
    Sys_EnqueueEvent(cmdBuffer, SYS_CAST_EVENT_TO_BASE(&ev));
}

void Win32_TickInput(ByteBuffer* cmdBuffer)
{
    i32 mouseMoveX = 0;
    i32 mouseMoveY = 0;
    
    RECT selfRect;
    GetWindowRect(appWindow, &selfRect);
    i32 width = (selfRect.right - selfRect.left);
    i32 height = (selfRect.bottom - selfRect.top);
    i32 halfWidth = width / 2;
    i32 halfHeight = height / 2;
    
    if (mouseMode == Captured && g_windowActive)
    {
        #if Z_ALLOW_MOUSE_LOCK
        SetCursorPos(selfRect.left + halfWidth, selfRect.top + halfHeight);
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

    SysInputEvent ev;

    Sys_CreateInputEvent(&ev, Z_INPUT_CODE_MOUSE_MOVE_X, mouseMoveX);
    Sys_EnqueueEvent(cmdBuffer, SYS_CAST_EVENT_TO_BASE(&ev));

    Sys_CreateInputEvent(&ev, Z_INPUT_CODE_MOUSE_MOVE_Y, mouseMoveY);
    Sys_EnqueueEvent(cmdBuffer, SYS_CAST_EVENT_TO_BASE(&ev));

    Win32_SendMouseScreenPosition(cmdBuffer);
}
#if 0
u32 Win32_StringToVKCode(char* string)
{
    char str[64];
    COM_ZeroMemory((u8*)str, 64);
    COM_CopyStringLimited(string, str, 64);
    COM_StrToUpperCase(str);
    if (COM_CompareStrings(str, "A") == 0)                  return (u32)'A';
    if (COM_CompareStrings(str, "B") == 0)                  return (u32)'B';
    if (COM_CompareStrings(str, "C") == 0)                  return (u32)'C';
    if (COM_CompareStrings(str, "D") == 0)                  return (u32)'D';
    if (COM_CompareStrings(str, "E") == 0)                  return (u32)'E';
    if (COM_CompareStrings(str, "F") == 0)                  return (u32)'F';
    if (COM_CompareStrings(str, "G") == 0)                  return (u32)'G';
    if (COM_CompareStrings(str, "H") == 0)                  return (u32)'H';
    if (COM_CompareStrings(str, "I") == 0)                  return (u32)'I';
    if (COM_CompareStrings(str, "J") == 0)                  return (u32)'J';
    if (COM_CompareStrings(str, "K") == 0)                  return (u32)'K';
    if (COM_CompareStrings(str, "L") == 0)                  return (u32)'L';
    if (COM_CompareStrings(str, "M") == 0)                  return (u32)'M';
    if (COM_CompareStrings(str, "N") == 0)                  return (u32)'N';
    if (COM_CompareStrings(str, "O") == 0)                  return (u32)'O';
    if (COM_CompareStrings(str, "P") == 0)                  return (u32)'P';
    if (COM_CompareStrings(str, "Q") == 0)                  return (u32)'Q';
    if (COM_CompareStrings(str, "R") == 0)                  return (u32)'R';
    if (COM_CompareStrings(str, "S") == 0)                  return (u32)'S';
    if (COM_CompareStrings(str, "T") == 0)                  return (u32)'T';
    if (COM_CompareStrings(str, "U") == 0)                  return (u32)'U';
    if (COM_CompareStrings(str, "V") == 0)                  return (u32)'V';
    if (COM_CompareStrings(str, "W") == 0)                  return (u32)'W';
    if (COM_CompareStrings(str, "X") == 0)                  return (u32)'X';
    if (COM_CompareStrings(str, "Y") == 0)                  return (u32)'Y';
    if (COM_CompareStrings(str, "Z") == 0)                  return (u32)'Z';
    if (COM_CompareStrings(str, "0") == 0)                  return (u32)'0';
    if (COM_CompareStrings(str, "1") == 0)                  return (u32)'1';
    if (COM_CompareStrings(str, "2") == 0)                  return (u32)'2';
    if (COM_CompareStrings(str, "3") == 0)                  return (u32)'3';
    if (COM_CompareStrings(str, "4") == 0)                  return (u32)'4';
    if (COM_CompareStrings(str, "5") == 0)                  return (u32)'5';
    if (COM_CompareStrings(str, "6") == 0)                  return (u32)'6';
    if (COM_CompareStrings(str, "7") == 0)                  return (u32)'7';
    if (COM_CompareStrings(str, "8") == 0)                  return (u32)'8';
    if (COM_CompareStrings(str, "9") == 0)                  return (u32)'9';
    if (COM_CompareStrings(str, "ESCAPE") == 0)             return VK_ESCAPE;
    if (COM_CompareStrings(str, "UP") == 0)                 return VK_UP;
    if (COM_CompareStrings(str, "DOWN") == 0)               return VK_DOWN;
    if (COM_CompareStrings(str, "LEFT") == 0)               return VK_LEFT;
    if (COM_CompareStrings(str, "RIGHT") == 0)              return VK_RIGHT;
    if (COM_CompareStrings(str, "SPACE") == 0)              return VK_SPACE;
    if (COM_CompareStrings(str, "CONTROL") == 0)            return VK_CONTROL;
    if (COM_CompareStrings(str, "SHIFT") == 0)              return VK_SHIFT;

    return Z_INPUT_CODE_NULL;
}
#endif
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
    if (vkCode == 112)              return Z_INPUT_CODE_F1;
    if (vkCode == 113)              return Z_INPUT_CODE_F2;
    if (vkCode == 114)              return Z_INPUT_CODE_F3;
    if (vkCode == 115)              return Z_INPUT_CODE_F4;
    if (vkCode == 116)              return Z_INPUT_CODE_F5;
    if (vkCode == 117)              return Z_INPUT_CODE_F6;
    if (vkCode == 118)              return Z_INPUT_CODE_F7;
    if (vkCode == 119)              return Z_INPUT_CODE_F8;
    if (vkCode == 120)              return Z_INPUT_CODE_F9;
    if (vkCode == 121)              return Z_INPUT_CODE_F10;
    if (vkCode == 122)              return Z_INPUT_CODE_F11;
    if (vkCode == 123)              return Z_INPUT_CODE_F12;

    return Z_INPUT_CODE_NULL;
}
