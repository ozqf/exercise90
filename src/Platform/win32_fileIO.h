#pragma once

#include "win32_system_include.h"
#include "win32_asset_types.h"
#include "win32_gl.h"

#include "../Shared/shared_types.h"
#include "../Shared/Memory/Heap.h"
#include "../Shared/shared_render_types.h"

/**********************************************************************
 * PRIMITIVE FILE I/O
/*********************************************************************/
void Win32_FreeFileMemory(void *mem)
{
    VirtualFree(mem, 0, MEM_RELEASE);
}

void *Win32_ReadEntireFile(char *fileName)
{
    void *result = 0;
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize))
        {
            LPDWORD bytesRead = 0;
            u32 fileSize32 = SafeTruncateUInt64(fileSize.QuadPart);
            result = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (result)
            {
                if (ReadFile(fileHandle, result, fileSize32, bytesRead, 0) && fileSize32 == (u32)bytesRead)
                {
                    // File read successfully
                }
                else
                {
                    Win32_FreeFileMemory(result);
                    result = 0;
                }
            }
            else
            {
                // TODO: Logging
            }
        }

        CloseHandle(fileHandle);
    }
    return result;
}

bool32 Win32_WriteEntireFile(char *fileName, u32 memorySize, void *memory)
{
    bool32 result = false;

    HANDLE fileHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        if (WriteFile(fileHandle, memory, memorySize, &bytesWritten, 0))
        {
            // Make sure entire file was written
            result = (bytesWritten == memorySize);
        }
        else
        {
            // TODO logging
        }
    }
    else
    {
        // TODO logging
    }
    return result;
}

/**********************************************************************
 * PRIMITIVE FILE I/O B version
/*********************************************************************/

//#include <stdio.h>
//#include <stdarg.h>

//#include "win32_system_include.h"
void BlockRefTest()
{
    //BlockRef ref = {};
}
#if 1
BlockRef Platform_LoadFileIntoHeap(Heap* heap, char* fileName)
{
    /*
    > Open file, find size
    > Prepare block on heap
    > read file contents into heap block
    > close
    */
    
    FILE* f;
    fopen_s(&f, fileName, "rb");

    i32 end;

    // Scan for file size
    // pos = ftell(f); // assuming start at 0 anyway
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Alloc on Heap
    BlockRef ref = {};
    Heap_Allocate(heap, &ref, end, fileName);

    // Get position on heap
    void* memory = Heap_GetBlockMemoryAddress(heap, &ref);

    // Read!
    // heap has made a space the size of the file, so buffer size and read size
    // are the same
    fread_s(memory, end, 1, end, f);

    // Close file stream
    fclose(f);
    return ref;
}
#endif

#if 1
void Win32_ReadBMPToHeap(Heap* heap, char* filePath)
{
    FILE* f;
    fopen_s(&f, filePath, "rb");

    i32 end;

	i32 w, h;

    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, 0, SEEK_SET);

	// WINBMPFILEHEADER* fileHeader = (WINBMPFILEHEADER*)mem;
	// AssertAlways(fileHeader->FileType == BMP_FILE_TYPE);
	
	// Read file header
    WINBMPFILEHEADER fileHeader;
    i32 fileHeaderSize = sizeof(fileHeader);
    fread_s((void*)&fileHeader, fileHeaderSize, 1, fileHeaderSize, f);
	AssertAlways(fileHeader.FileType == BMP_FILE_TYPE);

	// Read bitmap header
	WINNTBITMAPHEADER bmpHeader;
	i32 bmpHeaderSize = sizeof(bmpHeader);
	fseek(f, fileHeaderSize, SEEK_SET);
	fread_s((void*)&bmpHeader, bmpHeaderSize, 1, bmpHeaderSize, f);

	w = bmpHeader.Width;
	h = bmpHeader.Height;
	u32 numPixels = w * h;

    char buf[128];
    sprintf_s(buf, 128,
        "Read BMP at %s: Type: %d. bytes: %d Size: %d, %d\n",
        filePath, fileHeader.FileType, fileHeader.FileSize,
		w, h);
    OutputDebugString(buf);

	u32 targetImageBytes = sizeof(u32) * numPixels;
	u32 targetSize = sizeof(Texture2DHeader) + targetImageBytes;
	
	// Allocate space for results on Heap
	BlockRef destRef = {};
	Heap_Allocate(heap, &destRef, targetSize, filePath);
	Texture2DHeader* tex = (Texture2DHeader*)destRef.ptrMemory;
	u32* pixels = (u32*)((u8*)destRef.ptrMemory + sizeof(Texture2DHeader));
	COM_SetMemory((u8*)pixels, targetImageBytes, 0xAB);

	// Allocate temporary space on the heap for converting the file
	BlockRef sourceRef = {};
	Heap_Allocate(heap, &sourceRef, end, filePath);
	fseek(f, 0, SEEK_SET);
	fread_s(sourceRef.ptrMemory, end, 1, end, f);
	u32* sourcePixels = (u32*)((u8*)sourceRef.ptrMemory + fileHeader.BitmapOffset);

	u32* destPixel = pixels;
	u32* sourcePixel = sourcePixels;

	// Read bitmap and convert pixel format

	// Byte order in file is AA BB GG RR
	// Little endian 0xRRGGBBAA
	// convert to 0xAARRGGBB for GL_RGBA

	for (u32 i = 0; i < numPixels; ++i)
	{

#if 1
		u8 alpha = (u8)(*sourcePixel);
		u8 blue = (u8)(*sourcePixel >> 8);
		u8 green = (u8)(*sourcePixel >> 16);
		u8 red = (u8)(*sourcePixel >> 24);
#endif
#if 0
		// purple: r 255, b 255
		// yellow: r 255, g 255
		// cyan: g 255, b 255
		// bgra?
		u8 alpha = 0;
		u8 blue = 255;
		u8 green = 255;
		u8 red = 0;
#endif

		u32_union u32Bytes;
		//u32Bytes.bytes[0] = 0; // r
		//u32Bytes.bytes[1] = 255; // g
		//u32Bytes.bytes[2] = 255; // b
		//u32Bytes.bytes[3] = 0; // a
		u32Bytes.bytes[0] = red;
		u32Bytes.bytes[1] = green;
		u32Bytes.bytes[2] = blue;
		u32Bytes.bytes[3] = alpha;
		*destPixel = u32Bytes.value;

		// Doesn't work... use union trick?
		/**destPixel = (u32)alpha << 0;
		*destPixel |= (u32)red << 16;
		*destPixel |= (u32)green << 8;
		*destPixel |= (u32)blue << 24;*/

		++destPixel;
		++sourcePixel;
	}

	u32 id = Win32_R_RegisterTexture(pixels, w, h);
	//Heap_Free(heap, sourceRef.id);

    //DebugBreak();

    fclose(f);
}
#endif

/**************************************************
 * Specific file loading
 * 
 */
#if 0
u8 Win32_IO_ReadBMPTest(BlockRef ref)
{
    void* mem = Heap_GetBlockMemoryAddress(&g_heap, &ref);
	
	WINBMPFILEHEADER* fileHeader = (WINBMPFILEHEADER*)mem;
	AssertAlways(fileHeader->FileType == BMP_FILE_TYPE);
	
	const i32 fileHeaderSize = sizeof(WINBMPFILEHEADER);

	WINNTBITMAPHEADER* bmpHeader = (WINNTBITMAPHEADER*)((u8*)mem + fileHeaderSize);

	// only supporting 32 bit bitmaps right now!
	AssertAlways(bmpHeader->BitsPerPixel == 32);
	AssertAlways(bmpHeader->Compression == 3);
	
	u32* pixels = (u32*)((u8*)mem + fileHeaderSize + bmpHeader->Size);
	u32 firstPixel = *pixels;

	// Byte order is AA BB GG RR
	// Little endian 0xRRGGBBAA

	u8 alpha = (u8)(firstPixel);
	u8 blue = (u8)(firstPixel >> 8);
	u8 green = (u8)(firstPixel >> 16);
	u8 red = (u8)(firstPixel >> 24);

	//  GL_RGBA

	// Rearrange to rgba
	
    DebugBreak();
	return 1;
}
#endif
