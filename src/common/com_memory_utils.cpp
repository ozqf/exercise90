#pragma once

#include "com_defines.h"

static inline u32 SafeTruncateUInt64(u64 value)
{
	// TODO: Defines for max value
	Assert(value <= 0xFFFFFFFF);
	u32 result = (u32)value;
	return result;
}

/**
 * Returns number of bytes written
 */
static inline u32 COM_WriteByte(u8 value, u8* target)
{
	Assert(target != NULL);
    *target = value;
    return 1;
}

static inline void COM_WriteByte(u8 value, u8** target)
{
	Assert(target != NULL);
    **target = value;
    *target++;
}

/**
 * Returns number of bytes written
 */
static inline u32 COM_WriteI32(i32 value, u8* target)
{
	Assert(target != NULL);
    *(i32*)target = value;
    return sizeof(i32);
}

/**
 * Returns number of bytes written
 */
static inline u32 COM_WriteU32(u32 value, u8* target)
{
	Assert(target != NULL);
    *(u32*)target = value;
    return sizeof(u32);
}

/**
 * Returns number of bytes written
 */
static inline u32 COM_WriteI16(i16 value, u8* target)
{
	Assert(target != NULL);
    *(i16*)target = value;
    return sizeof(u16);
}

/**
 * Returns number of bytes written
 */
static inline u32 COM_WriteU16(u16 value, u8* target)
{
	Assert(target != NULL);
    *(u16*)target = value;
    return sizeof(u16);
}

/**
 * Read an i32 at the target pointer position AND move the target pointer forward
 */
static inline u8 COM_ReadByte(u8** target)
{
	Assert(target != NULL);
    u8 result = *(u8*)*target;
    *target += sizeof(u8);
    return result;
}

/**
 * Read an i32 at the target position, keeping target in place
 */
static inline u8 COM_PeekByte(u8* target)
{
	Assert(target != NULL);
    u8 result = *(u8*)target;
    return sizeof(u8);
}

/**
 * Read an i32 at the target pointer position AND move the target pointer forward
 */
static inline i32 COM_ReadI32(u8** target)
{
	Assert(target != NULL);
    i32 result = *(i32*)*target;
    *target += sizeof(i32);
    return result;
}

static inline u16 COM_ReadU16(u8** target)
{
	Assert(target != NULL);
    u16 result = *(u16*)*target;
    *target += sizeof(u16);
    return result;
}

/**
 * Read an i32 at the target position, keeping target in place
 */
static inline i32 COM_PeekI32(u8* target)
{
	Assert(target != NULL);
    i32 result = *(i32*)target;
    return sizeof(i32);
}

/**
 * Returns number of bytes written
 */
static inline u32 COM_CopyMemory(u8* source, u8* target, u32 numBytes)
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
    return numBytes;
}

// returns 1 if two blocks of memory are identical. 0 if otherwise.
static inline i32 COM_CompareMemory(u8* ptrA, u8* ptrB, u32 numBytes)
{
	u8* end = ptrA + numBytes;
	do
	{
		if (*ptrA != *ptrB)
		{
			return 0;
		}
		++ptrA;
		++ptrB;
	} while (ptrA < end);
	return 1;
}

static inline void COM_ZeroMemory(u8 *ptr, u32 numBytes)
{
    u32 endPoint = (u32) ptr + numBytes;
    while ((u32)ptr < endPoint) { *ptr++ = 0; }
}

static inline void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val)
{
    u32 endPoint = (u32) ptr + numBytes;
    while ((u32)ptr < endPoint) { *ptr++ = val; }
}

/**
 * Repeatedly write the given pattern into ptr
 * Pattern size must fit exactly into numBytes!
 */
static inline void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize)
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

static inline void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes)
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
static inline u32 COM_AlignSize(u32 value, u32 alignment)
{
    u32 remainder = value % alignment;
    if (remainder == 0) { return value; }
    remainder = alignment - remainder;
    return value + remainder;
}

static inline i32 COM_SimpleHash(u8* ptr, i32 numBytes)
{
    #if 0
    printf("Hashing %d bytes: ", numBytes);
    for (i32 i = 0; i < numBytes; ++i)
    {
        printf("%d, ", ptr[i]);
    }
    printf("\n");
    #endif
	i32 hash = 0xDEADBEEF;
    //i32 hash = 49105278;
    u8* end = ptr + numBytes;
    while (ptr < end)
    {
        hash += *ptr + (hash << 6) + (hash << 16) - hash;
        ptr++;
    }
    return hash;
}
