#pragma once

#include "win32_system_include.h"
#include <gl/gl.h>

#include "../common/com_module.h"

#include "../interface/platform_interface.h"
#include "../interface/app_interface.h"
#include "../interface/app_stub.h"

#include "win32_snd/win32_snd_interface.h"
#include "win32_gl/win32_gl_interface.h"

/****************************************************************
When something goes wrong
****************************************************************/
void Win32_Error(char *msg, char *title)
{
    MessageBox(0, msg, title, MB_OK | MB_ICONINFORMATION);
    AssertAlways(false);
}

/****************************************************************
STRUCTS
****************************************************************/

struct win32_offscreen_buffer
{
	BITMAPINFO info;
	void *ptrMemory;
	int memSize;
	int width;
	int height;
	int pitch;
	int bytesPerPixel;
};

struct win32_window_dimension
{
	int width;
	int height;
};

enum ExternalModuleType
{
	Null = 0,
	Game = 1,
	Renderer = 2,
	Sound = 3
};

struct win32_module_link
{
	ExternalModuleType type = Null;
	char* path = NULL;
	char* pathForCopy = NULL;

	HMODULE moduleHandle = NULL;
	u8 moduleState = 0;
	ULARGE_INTEGER timestamp = {};
	FILETIME* time = NULL;
	f32 checkTick = 1;
};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

global_variable GameTime g_gameTime;

// Interfaces
global_variable PlatformInterface platInterface;

global_variable AppInterface g_app;
global_variable RenderInterface g_renderer = {};
global_variable SoundInterface g_sound = {};

global_variable win32_module_link g_appLink = {};
global_variable win32_module_link g_rendererLink = {};
global_variable win32_module_link g_soundLink = {};

// DLLs
//global_variable char *appModulePath = "base/gamex86.dll";
//global_variable char *appModulePathCopy = "base/gamex86copy.dll";
//global_variable char *renderModulePath = "win32gl.dll";
//global_variable char *renderModulePathCopy = "win32glcopy.dll";
//global_variable char *soundModulePath = "win32sound.dll";
//global_variable char *soundModulePathCopy = "win32soundcopy.dll";

//global_variable HMODULE gameModule;
//global_variable HMODULE renderModule;
//global_variable u8 renderModuleState = 0;
//global_variable u8 appModuleState = 0;
//global_variable ULARGE_INTEGER g_renderModuleTimestamp = {};
//global_variable ULARGE_INTEGER g_appModuleTimestamp = {};
//global_variable FILETIME* renderModuleTime;
//global_variable FILETIME* appModuleTime;
//global_variable f32 g_checkRenderReloadTick = 1;
//global_variable f32 g_checkAppReloadTick = 1;

// system handles etc
global_variable HWND appWindow;
global_variable win32_offscreen_buffer globalBackBuffer;
global_variable u8 g_windowActive = 1;

// Control/Input
global_variable bool globalRunning = true;
global_variable POINT g_win32_mousePos = {};
global_variable POINT g_win32_mousePosLast = {};
global_variable POINT g_win32_mousePosMove = {};

#define COMMAND_BUFFER_SIZE 2048
global_variable u8 g_commandBufferA[COMMAND_BUFFER_SIZE];
global_variable u8 g_commandBufferB[COMMAND_BUFFER_SIZE];
// Init command buffer to point at and write from start of buffer A
global_variable ByteBuffer g_cmdBuf = 
{
	g_commandBufferA,
	g_commandBufferA,
	g_commandBufferA,
	COMMAND_BUFFER_SIZE 
};

// Timing
global_variable double global_timePassed = 0;
global_variable double global_secondsPerTick = 0;
global_variable __int64 global_timeCount = 0;
global_variable LARGE_INTEGER tick, tock;

// Command Line
// Nice array of pointers to the start of each token in the launch param string
#define MAX_LAUNCH_PARAMS 50
global_variable char *launchParams[MAX_LAUNCH_PARAMS];
global_variable i32 numLaunchParams = 0;
