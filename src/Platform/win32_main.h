#pragma once

#include "win32_system_include.h"

#include "../Shared/shared.h"
#include "../interface/platform_interface.h"
#include "../interface/app_interface.h"
#include "../interface/app_stub.h"
#include "../Shared/r_scene.h"

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

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// App/platform interface
global_variable PlatformInterface platInterface;
global_variable AppInterface app;
global_variable char *appModulePath = "base/gamex86.dll";

// system handles etc
global_variable HWND appWindow;
global_variable HMODULE gameModule;
global_variable win32_offscreen_buffer globalBackBuffer;

// Control/Input
global_variable bool globalRunning = true;
global_variable InputTick inputTick;
global_variable POINT g_win32_mousePos = {};
global_variable POINT g_win32_mousePosLast = {};
global_variable POINT g_win32_mousePosMove = {};

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
