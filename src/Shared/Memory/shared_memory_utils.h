#pragma once
///////////////////////////////////////////////////////////////////////
// Utility functions for manipulating raw memory
///////////////////////////////////////////////////////////////////////
#include "../shared_types.h"

void COM_CopyMemory(u8* source, u8* target, u32 numBytes)
{
	Assert(source != NULL);
	Assert(target != NULL);
    u32 progress = 0;
    while (progress < numBytes)
    {
        *target = *source;
        source++;
        target++;
		progress++;
    };
}

void COM_ZeroMemory(u8 *ptr, u32 numBytes)
{
    u32 endPoint = (u32) ptr + numBytes;
    while ((u32)ptr < endPoint) { *ptr++ = 0; }
}

void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val)
{
    u32 endPoint = (u32) ptr + numBytes;
    while ((u32)ptr < endPoint) { *ptr++ = val; }
}

/**
 * Repeatedly write the given pattern into ptr
 * Pattern size must fit exactly into numBytes!
 */
void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize)
{
    u32 numCopies = numBytes / patternSize;
    AssertAlways((numBytes % patternSize) == 0);
    u32 endPoint = (u32) ptr + numBytes;
    for (u32 i = 0; i < numCopies; ++i)
    {
        for (u32 j = 0; j < patternSize; ++j)
        {
            *ptr = pattern[j];
            ptr++;
        }
    }
}

void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes)
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
u32 COM_AlignSize(u32 value, u32 alignment)
{
    u32 remainder = value % alignment;
    if (remainder == 0) { return value; }
    remainder = alignment - remainder;
    return value + remainder;
}

///////////////////////////////////////////////////////////////////////
// Macros
///////////////////////////////////////////////////////////////////////
#ifndef COM_COPY
#define COM_COPY(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##))
#endif

#ifndef COM_COPY_STEP
#define COM_COPY_STEP(ptrSource, ptrDestination, structType) \
ptrDestination = (u8*)((u32)ptrDestination + (u32)sizeof(##structType##))
#endif


#ifndef COM_HANDLE_EVENT_CASE
#define COM_HANDLE_EVENT_CASE(eventTypeInteger, structType, ptrBytes, ptrBufferStart, bufferSize, functionName) \
case eventTypeInteger##: \
{ \
	ASSERT(COM_CheckBufferHasSpace((u8*)##ptrBytes##, (u8*)ptrBufferStart##, bufferSize##, sizeof(##structType##))); \
    structType ev = {}; \
    COM_COPY(##ptrBytes##, &##ev##, structType##); \
    ptrBytes = ((u8*)##ptrBytes + sizeof(##structType##)); \
} break;
#endif

void COM_Mem_Test()
{
    Vec3 v = {};
    u8 buffer[64];
    u8* b2 = 0;
    // copy
    COM_COPY(&v, buffer, Vec3);
    // copy and move
    COM_COPY_STEP(&v, b2, Vec3);
}
