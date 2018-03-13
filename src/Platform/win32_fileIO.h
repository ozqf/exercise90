#pragma once



#include "../Shared/Memory/Heap.h"

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
