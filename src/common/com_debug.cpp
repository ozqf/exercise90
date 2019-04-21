#pragma once

#include "common.h"

void COM_PrintBits(i32 val, u8 printNewLine)
{
    for (i32 i = 31; i >= 0; --i)
    {
        i32 result = 1 & (val >> i);
        printf("%d", result);
    }
    if (printNewLine)
    {
        printf("\n");
    }
}

void COM_PrintBytes(u8* bytes, i32 numBytes, i32 bytesPerRow)
{
    u8* read = bytes;
    if (bytesPerRow <= 0)
    {
        bytesPerRow = 16;
    }
    u8* end = read + numBytes;
    i32 count = 0;
    while (read < end)
    {
        printf("%03d, ", *read);
        read++;
        if (++count >= bytesPerRow)
        {
            count = 0;
        }
    }
    printf("\n");
}

void COM_PrintBytesHex(u8* bytes, i32 numBytes, i32 bytesPerRow)
{
    u8* read = bytes;
    if (bytesPerRow <= 0)
    {
        bytesPerRow = 16;
    }
    u8* end = read + numBytes;
    i32 count = 0;
    while (read < end)
    {
        printf("%02X, ", *read);
        read++;
        if (++count >= bytesPerRow)
        {
            count = 0;
            printf("\n");
        }
    }
    printf("\n");
}
