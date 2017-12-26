/*******************************************
 * Utility functions for manipulating raw memory
 ******************************************/
#pragma once
#include "shared_types.h"

void Com_CopyMemory(u8* source, u8* target, u32 numBytes)
{
    u32 progress = 0;
    while (progress < numBytes)
    {
        *target = *source;
        source++;
        target++;
    };
}

void Com_ZeroMemory(u8 *ptr, u32 numBytes)
{
    u32 endPoint = (u32) ptr + numBytes;
    while ((u32)ptr < endPoint) { *ptr = 0; }
}

void Com_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes)
{
    u32 endPoint = (u32) ptr + numBytes;
    while ((u32)ptr < endPoint) { *ptr = value; }
}

/**
Returns <value> enlarged to match the alignment specified.
eg value = 52, align = 32 == 64, 
52 mod 32 = 20
32 - 20 = 12
52 + 12 = 64

but
64 mod 32 = 0
32 - 0 = 32
64 + 32 = 96

*/
u32 Com_AlignSize(u32 value, u32 alignment)
{
    u32 remainder = value % alignment;
    if (remainder == 0) { return value; }
    remainder = alignment - remainder;
    return value + remainder;
}
