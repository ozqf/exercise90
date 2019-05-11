#pragma once

#include "win32_system_include.h"
#include "win32_fileIO.h"

#include "../interface/sound_interface.h"


/**********************************************************************
 * Prepare export interfaces for DLLs
 *********************************************************************/

void Win32_InitPlatformInterface()
{
    //////////////////////////////////////////////////////////////
    // App
    platInterface.Error = Win32_Error;
    platInterface.Log = Win32_Log;
    platInterface.Print = Win32_Print;

    platInterface.Malloc = Win32_Alloc;
    platInterface.Free = Win32_Free;
    platInterface.MeasureFile = Win32_MeasureFile;
    platInterface.LoadFileIntoHeap = Platform_LoadFileIntoHeap;
    platInterface.LoadTextureB = Win32_LoadTextureB;
    platInterface.BindTexture = Platform_BindTexture;
    
    platInterface.SetMouseMode = Win32_SetMouseMode;
    
    platInterface.SampleClock = Win32_QueryPerformanceCounter;

    platInterface.RenderScene = Platform_R_DrawScene;
    platInterface.SubmitRenderCommands = Platform_R_SubmitCommands;

    platInterface.LoadSound = Platform_LoadSound;

    platInterface.OpenFileForWriting = Platform_OpenFileForWriting;
    platInterface.WriteToFile = Platform_WriteToFile;
    platInterface.CloseFileForWriting = Platform_CloseFileForWriting;
	platInterface.SeekInFileFromStart = Platform_SeekInFileFromStart;
    platInterface.GetBaseDirectoryName = Platform_GetBaseDirectoryName;
    platInterface.GetDateTime = Platform_GetDateTime;
    platInterface.WriteAllTextToFile = Win32_WriteAllTextToFile;

    platInterface.SaveBMP = Win32_SaveBMP;

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

    //////////////////////////////////////////////////////////////
    // Renderer
    g_plat2Rend = {};
    g_plat2Rend.Log = Win32_Log;
    g_plat2Rend.Print = Win32_Log;
    g_plat2Rend.Error = Win32_Error;
    g_plat2Rend.Malloc = Win32_Alloc;
    g_plat2Rend.Free = Win32_Free;
    g_plat2Rend.SampleClock = Win32_QueryPerformanceCounter;
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
u8 Win32_LinkToApplication(win32_module_link* link, char* appName)
{
    PLAT_LOG(128, "PLATFORM Link to App %s\n", appName);
    if (g_app.isValid == 1)
    {
        Win32_CloseAppLink();
    }
    // Set paths
    sprintf_s(g_gameName, MAX_GAME_NAME_LENGTH, "%s", appName);
    sprintf_s(
        g_gamePath,
        MAX_GAME_PATH_LENGTH,
        "%s/%s",
        g_gameName,
        DEFAULT_GAME_DLL_NAME);
    sprintf_s(
        g_gamePathCopy,
        MAX_GAME_PATH_LENGTH,
        "%s/%s",
        g_gameName,
        DEFAULT_GAME_DLL_COPY_NAME);
    g_appLink.path = g_gamePath;
    g_appLink.pathForCopy = g_gamePathCopy;

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

