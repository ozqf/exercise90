#pragma once
/*
Functions exported to platform DLL
Anything with platform_ is a function to export
*/

#include "win32_system_include.h"
#include "win32_fileIO.h"

#include "../interface/sound_interface.h"
//#include "win32_gl_loading.h"

/****************************************************************
ALLOC MAIN MEMORY
Returns true if alloc succeeded
****************************************************************/
u8 Platform_Alloc(MemoryBlock *mem, u32 bytesToAllocate)
{
    mem->size = bytesToAllocate;
    mem->ptrMemory = VirtualAlloc(0,mem->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
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

/**********************************************************************
 * PLATFORM INTERFACE FUNCTIONS
 *********************************************************************/
void Platform_R_DrawScene(RenderScene* scene)
{
    if (g_rendererLink.moduleState == 1)
    {
        g_renderer.R_RenderScene(scene, &g_time);
    }
}

void Platform_R_SubmitCommands(RenderCommand* commands, i32 numCommands)
{
    if (g_rendererLink.moduleState == 1)
    {
        g_renderer.R_Execute(commands, numCommands, &g_time);
    }
}

void Platform_LoadTexture(Heap* heap, BlockRef* result, char* path)
{
    //Win32_ReadBMPToHeap(heap, result, path);
	Platform_LoadFileIntoHeap(heap, result, path, true);
}

void Platform_BindTexture(void* rgbaPixels, u32 width, u32 height, u32 textureIndex)
{
    if (g_rendererLink.moduleState == 1)
    {
        g_renderer.R_BindTexture(rgbaPixels, width, height, textureIndex);
    }
}

void Platform_SetDebugInputTextureIndex(i32 i)
{
    Win32_SetDebugInputTextureIndex(i);
}

// Copy the base name into the given buffer
i32 Platform_GetBaseDirectoryName(char* buffer, i32 bufferSize)
{
    return COM_CopyStringLimited(g_baseDirectoryName, buffer, bufferSize);
}

void Platform_GetDateTime(DateTime* data)
{
    if (data == NULL) { return; }
    SYSTEMTIME time = {};
    GetSystemTime(&time);
    data->year = time.wYear;
    data->month = time.wMonth;
    data->dayOfTheMonth = time.wDay;
    data->dayOfTheWeek = time.wDayOfWeek;
    data->hour = time.wHour;
    data->minute = time.wMinute;
    data->second = time.wSecond;
}

void Win32_Log(char* msg)
{
    if (g_logFile)
    {
        fprintf(g_logFile, "%s", msg);
    }
}

void Win32_Print(char* msg)
{
    if (g_logFile)
    {
        fprintf(g_logFile, "%s", msg);
    }
    printf("%s", msg);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Sound
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void Platform_LoadSound(u8* data, i32 numBytes)
{
    if (g_soundLink.moduleState == 1)
    {
        g_sound.Snd_LoadSound("foo", data, numBytes);
    }
}

u8 Snd_Play(ZSoundEvent* ev)
{
    if (g_soundLink.moduleState == 1)
    {
        return g_sound.Snd_Play2(ev);
    }
    return 0;
}

/**********************************************************************
 * Attach to application DLL
 *********************************************************************/

void Win32_InitPlatformInterface()
{
    platInterface.Error = Win32_Error;
    platInterface.Log = Win32_Log;
    platInterface.Print = Win32_Print;

    platInterface.Malloc = Platform_Alloc;
    platInterface.Free = Platform_Free;
    platInterface.LoadFileIntoHeap = Platform_LoadFileIntoHeap;
    //platInterface.LoadDebugTextures = Platform_LoadDebugTextures;
    platInterface.LoadTexture = Platform_LoadTexture;
    platInterface.BindTexture = Platform_BindTexture;
    
    platInterface.SetMouseMode = Win32_SetMouseMode;

    platInterface.RenderScene = Platform_R_DrawScene;
    platInterface.SubmitRenderCommands = Platform_R_SubmitCommands;

    platInterface.SetDebugInputTextureIndex = Platform_SetDebugInputTextureIndex;

    platInterface.LoadSound = Platform_LoadSound;

    platInterface.OpenFileForWriting = Platform_OpenFileForWriting;
    platInterface.WriteToFile = Platform_WriteToFile;
    platInterface.CloseFileForWriting = Platform_CloseFileForWriting;
	platInterface.SeekInFileFromStart = Platform_SeekInFileFromStart;
    platInterface.GetBaseDirectoryName = Platform_GetBaseDirectoryName;
    platInterface.GetDateTime = Platform_GetDateTime;

    platInterface.WriteTextCommand = Win32_EnqueueTextCommand;

    // Network setup
    #if 1
    platInterface.Init = Net_Init;
    platInterface.OpenSocket = Net_OpenSocket;
    platInterface.Shutdown = Net_Shutdown;
    platInterface.CloseSocket = Net_CloseSocket;
    //platInterface.Net_RunLoopbackTest = Net_RunLoopbackTest;
    platInterface.SendTo = Net_SendTo;
    platInterface.Read = Net_Read;
    #endif

    // Renderer
    g_plat2Rend = {};
    g_plat2Rend.Log = Win32_Log;
    g_plat2Rend.Print = Win32_Log;
}

void Win32_CloseAppLink()
{
    g_app.AppShutdown();
	Win32_CloseDataFiles();
    FreeLibrary(g_appLink.moduleHandle);
    g_app.isValid = false;
}

///////////////////////////////////////////////////////////
// Link to App
///////////////////////////////////////////////////////////
u8 Win32_LinkToApplication()
{
    PLAT_LOG(64, "PLATFORM Link to App\n");
    if (g_app.isValid == 1)
    {
        Win32_CloseAppLink();
    }
    Win32_CopyFile(g_appLink.path, g_appLink.pathForCopy);
    g_appLink.moduleHandle = LoadLibraryA(g_appLink.pathForCopy);
    if (g_appLink.moduleHandle != NULL)
    {
        Func_LinkToApp *linkToApp = (Func_LinkToApp *)GetProcAddress(g_appLink.moduleHandle, "LinkToApp");
        if (linkToApp != NULL)
        {
            g_app = linkToApp(platInterface);
            // Attaching data files must happen BEFORE app init dummy
            Win32_LoadDataFiles();
            i32 errCode = g_app.AppInit();
            if (errCode != COM_ERROR_NONE)
            {
                PLAT_LOG(64, "INIT APP ERROR CODE %d - ABORTED\n", errCode);
                Win32_Error("Init App failed", "Error");
                return 1;
            }
			// Execute command line
			Win32_ExecuteCommandLine(g_numLaunchParams, g_launchParams);
            return 1;
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
}

void Win32_UpdateFileTimeStamp(char* path, ULARGE_INTEGER* timeStamp)
{
    FILETIME time;// = NULL;
    HANDLE fileHandle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    BOOL gotTime = GetFileTime(fileHandle, NULL, NULL, &time);
    CloseHandle(fileHandle);
    if(gotTime)
    {
        
        ULARGE_INTEGER large;
        large.LowPart = time.dwLowDateTime;
        large.HighPart = time.dwHighDateTime;
        
        timeStamp->QuadPart = large.QuadPart;
    }
    return;
}

u8 Win32_CheckFileModified(char* path, ULARGE_INTEGER* timeStamp)
{
    FILETIME time;// = NULL;
    HANDLE fileHandle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    BOOL gotTime = GetFileTime(fileHandle, NULL, NULL, &time);
    CloseHandle(fileHandle);
    if(gotTime)
    {
        ULARGE_INTEGER large;
        large.LowPart = time.dwLowDateTime;
        large.HighPart = time.dwHighDateTime;
        
        if (large.QuadPart != timeStamp->QuadPart)
        {
			timeStamp->QuadPart = large.QuadPart;
            return 1;
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////
// Link to Renderer
///////////////////////////////////////////////////////////

void Win32_CloseRendererLink()
{
    g_rendererLink.moduleState = 0;
    g_renderer.R_Shutdown();
    FreeLibrary(g_rendererLink.moduleHandle);
    g_renderer = {};
}

u8 Win32_LinkToRenderer()
{
    PLAT_LOG(64, "PLATFORM Link to Renderer\n");
    if (g_rendererLink.moduleState == 1)
    {
        Win32_CloseRendererLink();
    }
    Win32_CopyFile(g_rendererLink.path, g_rendererLink.pathForCopy);
    g_rendererLink.moduleHandle = LoadLibraryA(g_rendererLink.pathForCopy);
    if (g_rendererLink.moduleHandle != NULL)
    {
        Func_LinkToRenderer *link = (Func_LinkToRenderer *)GetProcAddress(g_rendererLink.moduleHandle, "LinkToRenderer");
        if (link != NULL)
        {
            g_renderer = link(g_plat2Rend);
            if (!g_renderer.R_Init(appWindow))
            {
                Win32_Error("Init Renderer failed", "Error");
                g_rendererLink.moduleState = 0;
                return 0;
            }
            g_rendererLink.moduleState = 1;
            return 1;
        }
        else
        {
            Win32_Error("Failed to find renderer dll link function", "Error");
            g_rendererLink.moduleState = 0;
            return 0;
        }
    }
    else
    {
        Win32_Error("Failed to find renderer dll", "Error");
        g_rendererLink.moduleState = 0;
        return 0;
    }
}

///////////////////////////////////////////////////////////
// Link to Sound
///////////////////////////////////////////////////////////

void Win32_CloseSoundLink()
{
	g_soundLink.moduleState = 0;
	g_sound.Snd_Shutdown();
	FreeLibrary(g_soundLink.moduleHandle);
	g_sound = {};
}

u8 Win32_LinkToSound()
{
    PLAT_LOG(64, "PLATFORM Link to Sound\n");
	if (g_soundLink.moduleState == 1)
	{
		Win32_CloseRendererLink();
	}
	Win32_CopyFile(g_soundLink.path, g_soundLink.pathForCopy);
	g_soundLink.moduleHandle = LoadLibraryA(g_soundLink.pathForCopy);
	if (g_soundLink.moduleHandle != NULL)
	{
		Func_LinkToSound *link = (Func_LinkToSound *)GetProcAddress(g_soundLink.moduleHandle, "LinkToSound");
		if (link != NULL)
		{
			g_sound = link();
			if (!g_sound.Snd_Init())
			{
				Win32_Error("Init Sound failed", "Error");
				g_soundLink.moduleState = 0;
				return 0;
			}
			g_soundLink.moduleState = 1;
			return 1;
		}
		else
		{
			Win32_Error("Failed to find sound dll link function", "Error");
			g_soundLink.moduleState = 0;
			return 0;
		}
	}
	else
	{
		Win32_Error("Failed to find sound dll", "Error");
		g_soundLink.moduleState = 0;
		return 0;
	}
}
