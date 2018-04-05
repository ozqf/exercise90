#pragma once
/*
Functions exported to platform DLL
Anything with platform_ is a function to export
*/

#include "win32_system_include.h"
#include "win32_fileIO.h"

/****************************************************************
ALLOC MAIN MEMORY
Returns true if alloc succeeded
****************************************************************/
u8 Platform_Alloc(MemoryBlock *mem, u32 bytesToAllocate)
{
    mem->size = bytesToAllocate;
    mem->ptrMemory = VirtualAlloc(0, mem->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    return (mem->ptrMemory != NULL);
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
    Win32_RenderFrame(scene);
}

// Returns number of textures loaded
i32 Platform_LoadDebugTextures(Heap* heap)
{
    // BlockRef testBmpRef = Platform_LoadFileIntoHeap(heap, "base/BitmapTest.bmp");
    // Win32_IO_ReadBMPTest(&testBmpRef);

    Win32_ReadBMPToHeap(heap, "base/BitmapTest.bmp");
    Win32_ReadBMPToHeap(heap, "base/charset.bmp");
    Win32_ReadBMPToHeap(heap, "base/brbrick2.bmp");
    Win32_ReadBMPToHeap(heap, "base/BKEYA0.bmp");
    
    return g_nextTexture;
}

/**********************************************************************
 * Attach to application DLL
 *********************************************************************/

void Win32_InitPlatformInterface()
{
    platInterface.Platform_Malloc = Platform_Alloc;
    platInterface.Platform_Free = Platform_Free;
    platInterface.Platform_LoadFileIntoHeap = Platform_LoadFileIntoHeap;
    platInterface.Platform_LoadDebugTextures = Platform_LoadDebugTextures;
    
    platInterface.Platform_SetMouseMode = Win32_SetMouseMode;

    platInterface.Platform_RenderScene = Platform_R_DrawScene;
}

void Win32_CloseAppLink()
{
    app.AppShutdown();
    FreeLibrary(gameModule);
    app.isvalid = false;
}

u8 Win32_LinkToApplication()
{
    if (app.isvalid == 1)
    {
        Win32_CloseAppLink();
    }
    Win32_CopyFile(appModulePath, appModulePathCopy);
    gameModule = LoadLibraryA(appModulePathCopy);
    if (gameModule != NULL)
    {
        Func_LinkToApp *linkToApp = (Func_LinkToApp *)GetProcAddress(gameModule, "LinkToApp");
        if (linkToApp != NULL)
        {
            app = linkToApp(platInterface);
            if (!app.AppInit())
            {
                Win32_Error("Init App failed", "Error");
                return 1;
            }
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
            Win32_Error("Failed to find dll link function", "Error");
            return 0;
        }
    }
    else
    {
        Win32_Error("Failed to find game dll", "Error");
        return 0;
    }
    //app = GetAppInterfaceStub(platInterface);
}

void Win32_CheckApplicationLink()
{
    //DebugBreak();
    FILETIME time;// = NULL;
    HANDLE fileHandle = CreateFileA(appModulePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    BOOL gotTime = GetFileTime(fileHandle, NULL, NULL, &time);
    CloseHandle(fileHandle);
    if(gotTime)
    {
        ULARGE_INTEGER large;
        large.LowPart = time.dwLowDateTime;
        large.HighPart = time.dwHighDateTime;
        
        if (large.QuadPart != g_appModuleTimestamp.QuadPart)
        {
			g_appModuleTimestamp.QuadPart = large.QuadPart;
            Win32_LinkToApplication();
        }
    }
}

u8 Win32_LinkToAppStub()
{
    app = GetAppInterfaceStub(platInterface);
    return 1;
}

