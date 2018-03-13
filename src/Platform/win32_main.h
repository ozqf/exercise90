#pragma once

#include "win32_system_include.h"

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
