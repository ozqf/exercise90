#pragma once
/*
Functions exported to platform DLL
Anything with platform_ is a function to export
*/

#include "win32_system_include.h"

f32 Platform_GetViewPortMinX() { return 0; }
f32 Platform_GetViewPortMinY() { return 0; }
f32 Platform_GetViewPortMaxX() { return 1280; }
f32 Platform_GetViewPortMaxY() { return 768; }
void Platform_ClearScreen(void) {}
void Platform_DrawBlitItems(BlitItem *items, i32 numItems) {}

/****************************************************************
ALLOC MAIN MEMORY
TODO: Return error code if it fails?
****************************************************************/
void Platform_Alloc(MemoryBlock *mem)
{
    i32 size = MegaBytes(2);
    mem->size = size;
    mem->ptrMemory = VirtualAlloc(0, mem->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void Platform_Free(MemoryBlock *mem)
{
    if (mem)
    {
        // Reset to 0
        char *cursor = (char *)mem->ptrMemory;
        for (int i = 0; i < mem->size; ++i)
        {
            *cursor = 0;
            cursor++;
        }
        VirtualFree(mem->ptrMemory, 0, MEM_RELEASE);
        mem->ptrMemory = 0;
    }
}

void Win32_PrintDebug(char *str)
{
    OutputDebugStringA(str);
}

/**********************************************************************
 * PLATFORM INTERFACE FUNCTIONS
 *********************************************************************/
void Platform_R_DrawScene(RenderScene* scene)
{
    Win32_RenderFrame(appWindow, scene);
}


/**********************************************************************
 * Attach to application DLL
 *********************************************************************/

void Win32_InitPlatformInterface()
{
    // platInterface.PlatformGetViewPortMinX = Platform_GetViewPortMinX;
    // platInterface.PlatformGetViewPortMinY = Platform_GetViewPortMinY;
    // platInterface.PlatformGetViewPortMaxX = Platform_GetViewPortMaxX;
    // platInterface.PlatformGetViewPortMaxY = Platform_GetViewPortMaxY;
    // platInterface.PlatformClearScreen = Platform_ClearScreen;
    // platInterface.PlatformRenderBlitItems = Platform_DrawBlitItems;
    platInterface.PlatformRenderScene = Platform_R_DrawScene;
}

u8 Win32_LinkToApplication()
{
    gameModule = LoadLibraryA(appModulePath);
    if (gameModule != NULL)
    {
        Func_LinkToApp *linkToApp = (Func_LinkToApp *)GetProcAddress(gameModule, "LinkToApp");
        if (linkToApp != NULL)
        {
            app = linkToApp(platInterface);
            return 1;
            // if (app != NULL)
            // {
            //     return 1;
            // }
            // else
            // {
            //     MessageBox(0, "No app returned from dll link", "Error", MB_OK | MB_ICONINFORMATION);
            //     return 0;
            // }
        }
        else
        {
            MessageBox(0, "Failed to find dll link function", "Error", MB_OK | MB_ICONINFORMATION);
            return 0;
        }
    }
    else
    {
        MessageBox(0, "Failed to find game dll", "Error", MB_OK | MB_ICONINFORMATION);
        return 0;
    }
    //app = GetAppInterfaceStub(platInterface);
}

u8 Win32_LinkToAppStub()
{
    app = GetAppInterfaceStub(platInterface);
    return 1;
}

