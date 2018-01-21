#pragma once

#include <windows.h>

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
