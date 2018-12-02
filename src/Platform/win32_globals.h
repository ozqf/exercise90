#pragma once

#include "win32_system_include.h"
#include <gl/gl.h>

#include "../common/com_module.h"

#include "../interface/platform_interface.h"
#include "../interface/app_interface.h"
#include "../interface/app_stub.h"

#include "../interface/sound_interface.h"
#include "win32_snd/win32_snd_interface.h"
#include "win32_gl/win32_gl_interface.h"

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
global_variable f32 g_fixedFrameAccumulator = 0;
global_variable f32 g_fixedFrameTime = 1.0f / 2.0f;//60.0f;

global_variable HWND consoleHandle;

// Interfaces
global_variable PlatformInterface platInterface;

global_variable AppInterface g_app;
global_variable RenderInterface g_renderer = {};
global_variable SoundInterface g_sound = {};

global_variable win32_module_link g_appLink = {};
global_variable win32_module_link g_rendererLink = {};
global_variable win32_module_link g_soundLink = {};

// Data files will be opened and kept open for program duration
#define PLATFORM_MAX_DATA_FILES 64
static DataFile g_dataFiles[PLATFORM_MAX_DATA_FILES];
static i32 g_nextDataFileIndex = 0;
static char* g_baseDirectoryName = "base";

// system handles etc
global_variable HWND appWindow;
global_variable win32_offscreen_buffer globalBackBuffer;
global_variable u8 g_windowActive = 1;

global_variable ScreenInfo g_screenInfo = {};

// Control/Input
global_variable bool globalRunning = true;
global_variable POINT g_win32_mousePos = {};
global_variable POINT g_win32_mousePosLast = {};
global_variable POINT g_win32_mousePosMove = {};

#define COMMAND_BUFFER_SIZE 2048
global_variable u8 g_commandBufferA[COMMAND_BUFFER_SIZE];
global_variable u8 g_commandBufferB[COMMAND_BUFFER_SIZE];
// Init command buffer to point at and write from start of buffer A
global_variable ByteBuffer g_input = 
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

global_variable u8 g_lowPowerMode = 0;
global_variable u8 g_singleFrameStepMode = 0;
global_variable u8 g_singleFrameRun = 0;

// Command Line
// Nice array of pointers to the start of each token in the launch param string
#define MAX_LAUNCH_PARAMS 50
global_variable char* g_launchParams[MAX_LAUNCH_PARAMS];
global_variable i32 g_numLaunchParams = 0;

#define TEXT_COMMAND_BUFFER_SIZE 2048
global_variable char g_textCommandBuffer[TEXT_COMMAND_BUFFER_SIZE];
global_variable i32 g_textCommandBufferPosition = 0;


inline void Win32_WritePlatformCommand(ByteBuffer* b, u8* source, u32 itemType, u32 itemSize)
{
    u32 used = b->ptrWrite - b->ptrStart;
    u32 space = b->capacity - used;
	if ((space - used) <= itemSize + sizeof(PlatformEventHeader))
	{
		printf("WIN32 No capacity left for platform command!\n");
		return;
	}
    //Assert((space - used) > itemSize + sizeof(PlatformEventHeader));

    PlatformEventHeader header = {};
    header.type = itemType;
    header.size = itemSize;

	u32 numHeaderBytes = sizeof(PlatformEventHeader);

    b->ptrWrite += COM_COPY(&header, b->ptrWrite, numHeaderBytes);
    b->ptrWrite += COM_COPY(source, b->ptrWrite, itemSize);

	// Trap to catch specific items being written
	//Assert(itemType != 2);
}

//void Win32_ParseTextCommand(char* str, i32 firstChar, i32 length);
void Win32_EnqueueTextCommand(char* command);

/****************************************************************
When something goes wrong
****************************************************************/
void Win32_Error(char *msg, char *title)
{
    MessageBox(0, msg, title, MB_OK | MB_ICONINFORMATION);
	ILLEGAL_CODE_PATH
}
