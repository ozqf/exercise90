#pragma once

#include "win32_system_include.h"

/****************************************************************
RENDERING
****************************************************************/

internal void Win32ResizeDIBSection(win32_offscreen_buffer *buffer, int width, int height)
{
	// TODO: Bulletproof this.
	// Maybe don't free first, free after, then free first if that succeeds

	if (buffer->ptrMemory)
	{
		VirtualFree(buffer->ptrMemory, 0, MEM_RELEASE);
		buffer->ptrMemory = NULL;
	}

	buffer->width = width;
	buffer->height = height;

	buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
	buffer->info.bmiHeader.biWidth = buffer->width;
	buffer->info.bmiHeader.biHeight = -buffer->height; // negative so that 0,0 x/y is top left not bottom left
	buffer->info.bmiHeader.biPlanes = 1;
	buffer->info.bmiHeader.biBitCount = 32;
	buffer->info.bmiHeader.biCompression = BI_RGB;
	
	// 32 bit pixel = 8 bits * 4 channels (rgb and 8bit padding to make pixels 32 bit aligned for speed)
	buffer->bytesPerPixel = 4;
	buffer->memSize = (width * height) * buffer->bytesPerPixel;
	buffer->ptrMemory = VirtualAlloc(0, buffer->memSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	
	// TODO: Clear buffer to black
	buffer->pitch = buffer->width * buffer->bytesPerPixel;
}

internal void Win32CopyBufferToWindow(HDC deviceContext, int windowWidth, int windowHeight, win32_offscreen_buffer *buffer, int x, int y, int width, int height)
{
	/*
	int pixIndex = x + (y * bitmapWidth);
	if (pixIndex > bitmapMemorySize
	*/
	
	// copy pixels with scaling
	StretchDIBits(deviceContext,
		/*x, y, width, height,
		x, y, width, height,*/
		0, 0, windowWidth, windowHeight,
		0, 0, buffer->width, buffer->height,
		buffer->ptrMemory,
		&buffer->info,
		DIB_RGB_COLORS, SRCCOPY);
}
