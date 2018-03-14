#pragma once



#include "../Shared/Memory/Heap.h"

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
BlockRef Win32_Heap_LoadFile(Heap* heap, char* fileName)
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

    // pos = ftell(f); // assuming start at 0 anyway
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, 0, SEEK_SET);

    BlockRef ref = {};
    Heap_Allocate(heap, &ref, end, fileName);

    void* memory = Heap_GetBlockMemoryAddress(heap, &ref);
    // heap has made a space the size of the file, so buffer size and read size
    // are the same
    fread_s(memory, end, 1, end, f);

    return ref;
}
#endif
#if 0
//const char* FILE_PERMISSION_READ_BINARY = "rb";
#define FILE_PERMISSION_READ_BINARY "rb"

void Win32_ReadFileFull()
{
    char* path = "win32_fileIO.h";

    FILE* f;
    // "rb"
    fopen_s(&f, path, FILE_PERMISSION_READ_BINARY);

    i32 pos;
    i32 end;
    // Record file start, then seek to end and record file end
    // to get full file size
    // reset to start to read
    pos = ftell(f);
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, 0, SEEK_SET);

    const i32 bufferSize = 1024;
    char* fileData[bufferSize];
    fread_s(fileData, bufferSize, 1, end, f);

    fclose(f);
    
}

#endif
