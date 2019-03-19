#pragma once

#include "win32_module.cpp"

#include "../common/com_defines.h"
#include "../common/com_memory_utils.h"

#include "../../lib/zlib/zlib.h"

/**********************************************************************
 * PRIMITIVE FILE I/O
/*********************************************************************/
void Win32_FreeFileMemory(void *mem)
{
    VirtualFree(mem, 0, MEM_RELEASE);
}

#define CHUNK 16384

i32 Win32_UnzipTest()
{
    return NULL;
#if 0
    //OutputDebugStringA(zlibVersion);
    //int z_result = compress(NULL, NULL, NULL, 0);

    i32 ret, flush;
    // unsigned have;
    z_stream strm;
    // u8 in[CHUNK];
    // u8 out[CHUNK];

    // int (faster) -1 to 9 (better)
    i8 defaultCompressionLevel = -1;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, defaultCompressionLevel);
    if (ret != Z_OK)
    {
        return ret;
    }
    flush = 0;
    return NULL;
#endif
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

extern "C"
void Win32_LoadBMP(Heap* heap, BlockRef* destRef, MemoryBlock mem, char* filePath)
{
    /* From .dat file:
    > Retrieve handle to .dat file and relevant file entry
    > Calculate size of bitmap
    > Alloc space on heap
    > Read over file in heap block
    */
    u8* fileOrigin = (u8*)mem.ptrMemory;
	u8* reader = fileOrigin;

	// Read file header
	WINBMPFILEHEADER fileHeader;
	i32 fileHeaderSize = sizeof(fileHeader);
	reader += COM_CopyMemory(reader, (u8*)&fileHeader, sizeof(WINBMPFILEHEADER));
	//fread_s((void*)&fileHeader, fileHeaderSize, 1, fileHeaderSize, f);
	AssertAlways(fileHeader.FileType == BMP_FILE_TYPE);


	// Read bitmap header
	WINNTBITMAPHEADER bmpHeader;
	i32 bmpHeaderSize = sizeof(bmpHeader);

	//reader = fileOrigin + fileHeader.

	//fseek(f, fileHeaderSize, SEEK_SET);
	//fread_s((void*)&bmpHeader, bmpHeaderSize, 1, bmpHeaderSize, f);
	COM_CopyMemory(reader, (u8*)&bmpHeader, sizeof(WINNTBITMAPHEADER));
	 
	i32 w = bmpHeader.Width;
	i32 h = bmpHeader.Height;
	u32 numPixels = w * h;
	
	PLAT_LOG(1024, "PLATFORM Reading BMP at %s: Type: %d. bytes: %d Size: %d, %d\n",
		filePath, fileHeader.FileType, fileHeader.FileSize,
		w, h);

	u32 targetImageBytes = sizeof(u32) * numPixels;
	u32 targetSize = sizeof(Texture2DHeader) + targetImageBytes;

	u32* sourcePixels = (u32*)(fileOrigin + fileHeader.BitmapOffset);

	// Allocate space for results on Heap
	Heap_Allocate(heap, destRef, targetSize, filePath, 1);
	Texture2DHeader* tex = (Texture2DHeader*)destRef->ptrMemory;
	COM_CopyStringLimited(filePath, tex->name, 32);
	u32* pixels = (u32*)((u8*)destRef->ptrMemory + sizeof(Texture2DHeader));
	tex->ptrMemory = pixels;
	tex->width = w;
	tex->height = h;

	COMTex_BMP2Internal(sourcePixels, pixels, numPixels);
}

extern "C"
i32 Win32_SaveBMP(Texture2DHeader* tex)
{
	if (tex == NULL || tex->ptrMemory == NULL)
	{
		return COM_ERROR_BAD_ARGUMENT;
	}
	printf("PLAT: Save BMP\n");

	WINBMPFILEHEADER fileHeader = {};

	WINNTBITMAPHEADER bmpHeader = {};
	bmpHeader.BitsPerPixel = 32;
	bmpHeader.Width = tex->width;
	bmpHeader.Height = tex->height;

	return COM_ERROR_NONE;
}

extern "C"
void Win32_CopyFile(char* sourcePath, char* targetPath)
{
    /*
    BOOL WINAPI CopyFileEx(
      _In_     LPCTSTR            lpExistingFileName,
      _In_     LPCTSTR            lpNewFileName,
      _In_opt_ LPPROGRESS_ROUTINE lpProgressRoutine,
      _In_opt_ LPVOID             lpData,
      _In_opt_ LPBOOL             pbCancel,
      _In_     DWORD              dwCopyFlags
    );
    */
    BOOL cancel = false;
	Assert(sourcePath);
	Assert(targetPath);
    CopyFileEx(sourcePath, targetPath, NULL, NULL, &cancel, 0);
}

extern "C"
u8  Platform_LoadFileIntoHeap(Heap* heap, BlockRef* destRef, char* fileName, u8 assertOnFailure)
{
	AssertAlways(destRef != NULL);
	/*
	> Find file in .dat, find size
	> Prepare block on heap
	> read file contents into heap block
	> close
	*/

	DataFileEntryReader r = {};
	if (Win32_FindDataFileEntry(fileName, &r) == 0)
	{
		// Read directly
		char buf[256];
		sprintf_s(buf, 256, "%s\\%s", g_baseDirectoryName, fileName);
		FILE* f;
		fopen_s(&f, buf, "rb");

		if (f == NULL)
		{
			//assertOnFailure = 1;
			if (assertOnFailure)
			{
				sprintf_s(buf, 256, "PLATFORM CRITICAL Failed to find file \"%s\\%s\"\n", g_baseDirectoryName, fileName);
				Win32_Error(buf, "File Not Found");
			}
			else
			{
				PLAT_PRINT(512, "PLATFORM Failed to find file \"%s\\%s\"\n", g_baseDirectoryName, fileName);
				return 0;
			}
			
		}

		fseek(f, 0, SEEK_END);
		u32 end = ftell(f);
		fseek(f, 0, SEEK_SET);
		Heap_Allocate(heap, destRef, end, fileName, 1);
		fread(destRef->ptrMemory, end, 1, f);
		fclose(f);
		return 1;
	}
	else
	{
		// Decide how to handle it based on file extension and flags
		//printf("Found file\n");
		if (COM_CheckForFileExtension((char*)r.entry.fileName, ".bmp"))
		{
			// Read file into staging area then load into heap
			MemoryBlock mem = {};
			mem.size = r.entry.size;
			void* ptr = malloc(mem.size);
			mem.ptrMemory = ptr;
			// mem.ptrMemory = malloc(mem.size);
			fseek(r.handle, r.entry.offset, SEEK_SET);
			fread(mem.ptrMemory, mem.size, 1, r.handle);

			Win32_LoadBMP(heap, destRef, mem, fileName);

			free(mem.ptrMemory);
		}
		else
		{
			// Just read directly to heap
			// Read file into staging area then load into heap
			MemoryBlock mem = {};
			mem.size = r.entry.size;
			void* ptr = malloc(mem.size);
			mem.ptrMemory = ptr;
			// mem.ptrMemory = malloc(mem.size);
			fseek(r.handle, r.entry.offset, SEEK_SET);
			fread(mem.ptrMemory, mem.size, 1, r.handle);
			PLAT_PRINT(1024, "PLATFORM Reading Generic File %s (%d bytes)\n", fileName, mem.size);

			Heap_Allocate(heap, destRef, mem.size, fileName, 1);
			COM_CopyMemory((u8*)mem.ptrMemory, (u8*)destRef->ptrMemory, destRef->objectSize);
			
			free(mem.ptrMemory);
		}
		return 1;
	}

}

/////////////////////////////////////////////////////////
// App interface to standard fopen/read/write etc
/////////////////////////////////////////////////////////
#define WIN32_MAX_OPEN_APP_FILES 4
FILE* g_appReadFiles[WIN32_MAX_OPEN_APP_FILES];

extern "C"
i32 Win32_GetFreeAppFileHandle()
{
	for (i32 i = 0; i < WIN32_MAX_OPEN_APP_FILES; ++i)
	{
		if (g_appReadFiles[i] == NULL)
		{
			return i;
		}
	}
	return -1;
}

i32 Platform_OpenFileForWriting(char* fileName)
{
	i32 index = Win32_GetFreeAppFileHandle();
	PLAT_LOG(512, "PLATFORM Opening file %s for writing\n", fileName);
	if (index == -1)
	{
		PLAT_PRINT(1024, "  PLATFORM no free file handle\n");
		ILLEGAL_CODE_PATH
	}
	
	fopen_s(&g_appReadFiles[index], fileName, "wb");

	if (g_appReadFiles[index] == NULL)
	{
		PLAT_PRINT(1024, "  PLATFORM failed to open %s for writing\n", fileName);
		return -1;
	}

	//fseek(g_appReadFiles[index], 0, SEEK_END);
	//i32 end = ftell(g_appReadFiles[index]);
	fseek(g_appReadFiles[index], 0, SEEK_SET);
	return index;
}

void Platform_SeekInFileFromStart(i32 fileId, u32 offset)
{
	fseek(g_appReadFiles[fileId], offset, SEEK_SET);
}

i32 Platform_WriteToFile(i32 fileId, u8* ptr, u32 numBytes)
{
	fwrite(ptr, numBytes, 1, g_appReadFiles[fileId]);
	i32 pos = ftell(g_appReadFiles[fileId]);
	// if (g_time.singleFrame)
	// {
	// 	printf("PLATFORM Wrote %d bytes to file %d. Total: %d\n", numBytes, fileId, pos);
	// }
	return 1;
}

i32 Platform_CloseFileForWriting(i32 fileId)
{
	Assert(g_appReadFiles[fileId] != NULL);
	fclose(g_appReadFiles[fileId]);
	g_appReadFiles[fileId] = NULL;
	return 1;
}

void Platform_SeekInFile(i32 fileId, i32 position)
{
	fseek(g_appReadFiles[fileId], position, SEEK_SET);
}

i32 Platform_GetPositionInFile(i32 fileId)
{
	return ftell(g_appReadFiles[fileId]);
}
