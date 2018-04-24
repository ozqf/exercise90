#pragma once

#include "shared_types.h"


void COM_CopyMemory(u8* source, u8* target, u32 numBytes);

void COM_ZeroMemory(u8 *ptr, u32 numBytes);

void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val);

void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize);

void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes);

u32 COM_AlignSize(u32 value, u32 alignment);

///////////////////////////////////////////////////////////////////////
// Macros
///////////////////////////////////////////////////////////////////////
#ifndef COM_COPY
#define COM_COPY(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##))
#endif

#ifndef COM_COPY_STEP
#define COM_COPY_STEP(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##)); \
ptrDestination = (u8*)((u32)ptrDestination + (u32)sizeof(##structType##))
#endif
