#pragma once
#if 0
#include "../../common/com_module.h"

#define TEST_BUFFER_WIDTH 32
#define TEST_BUFFER_HEIGHT 32
#define TEST_BUFFER_PIXEL_COUNT (32 * 32)
#define TEST_BUFFER_MEM_SIZE (TEST_BUFFER_PIXEL_COUNT * 4)

Texture2DHeader testBuffer;
u32 testBufferPixels[TEST_BUFFER_PIXEL_COUNT];

Texture2DHeader testBuffer2;
u32 testBufferPixels2[TEST_BUFFER_PIXEL_COUNT];

Texture2DHeader testBuffer3;
u32 testBufferPixels3[TEST_BUFFER_PIXEL_COUNT];

void AppInitTestTextures()
{
    // TODO: Remove texture test
	testBuffer = {};
	COM_CopyStringLimited("Test Texture 1", testBuffer.name, 32);
	testBuffer.ptrMemory = testBufferPixels;
	//testBuffer.memSize = sizeof(testBufferPixels);
	testBuffer.width = TEST_BUFFER_WIDTH;
	testBuffer.height = TEST_BUFFER_HEIGHT;
	//testBuffer.bytesPerPixel = 4;
	//testBuffer.pitch = TEST_BUFFER_WIDTH;

    testBuffer2 = {};
	COM_CopyStringLimited("Test Texture 2", testBuffer2.name, 32);
	testBuffer2.ptrMemory = testBufferPixels2;
	//testBuffer2.memSize = sizeof(testBufferPixels2);
	testBuffer2.width = TEST_BUFFER_WIDTH;
	testBuffer2.height = TEST_BUFFER_HEIGHT;
	//testBuffer2.bytesPerPixel = 4;
	//testBuffer2.pitch = TEST_BUFFER_WIDTH;

    testBuffer3 = {};
	COM_CopyStringLimited("Test Texture 3", testBuffer3.name, 32);
	testBuffer3.ptrMemory = testBufferPixels3;
	//testBuffer3.memSize = sizeof(testBufferPixels3);
	testBuffer3.width = TEST_BUFFER_WIDTH;
	testBuffer3.height = TEST_BUFFER_HEIGHT;
	//testBuffer3.bytesPerPixel = 4;
	//testBuffer3.pitch = TEST_BUFFER_WIDTH;

	//Com_ZeroMemory((u8*)testBufferPixels, TEST_BUFFER_PIXEL_COUNT * sizeof(u32));
	u8 colour[4];
	// rgba
	colour[0] = 255;
	colour[1] = 0;
	colour[2] = 0;
	colour[3] = 128;
	COM_SetMemoryPattern((u8*)testBufferPixels, TEST_BUFFER_MEM_SIZE, colour, 4);

	colour[0] = 0;
	colour[1] = 255;
	colour[2] = 0;
	colour[3] = 255;
    COM_SetMemoryPattern((u8*)testBufferPixels2, TEST_BUFFER_MEM_SIZE, colour, 4);
	
	colour[0] = 0;
	colour[1] = 0;
	colour[2] = 255;
	colour[3] = 255;
    COM_SetMemoryPattern((u8*)testBufferPixels3, TEST_BUFFER_MEM_SIZE, colour, 4);
}
#endif
