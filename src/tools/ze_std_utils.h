#ifndef ZE_STD_UTILS_H
#define ZE_STD_UTILS_H

#include <stdio.h>

static void ZE_std_ReadEntireFileToHeap(
    char* path, unsigned char** mem, unsigned int* numBytes)
{
    FILE* f;
    fopen_s(&f, path, "rb");
    if (f == NULL)
    {
        printf("Could not open %s\n", path);
        *mem = NULL;
        *numBytes = 0;
        return;
    }
    fseek(f, 0, SEEK_END);
    unsigned int end = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buf = (unsigned char*)malloc(end);
    fread(buf, end, 1, f);

    // return
    *mem = buf;
    *numBytes = end;
}

#endif // ZE_STD_UTILS_H