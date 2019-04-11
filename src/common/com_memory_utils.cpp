#pragma once

#include "com_defines.h"

com_internal u32 SafeTruncateUInt64(u64 value)
{
	// TODO: Defines for max value
	COM_ASSERT(value <= 0xFFFFFFFF, "Truncate U64 overflow");
	u32 result = (u32)value;
	return result;
}

////////////////////////////////////////////////////////////
// TODO: None of this handles eddianness!
// Implement via compile time flag, #define BIG_ENDIAN
////////////////////////////////////////////////////////////

/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteByte(u8 value, u8* target)
{
	COM_ASSERT(target != NULL, "Target is null");
    *target = value;
    return sizeof(u8);
}

com_internal void COM_WriteByte(u8 value, u8** target)
{
	COM_ASSERT(target != NULL, "Target is null");
    **target = value;
    *target++;
}

/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteI16(i16 value, u8* target)
{
	COM_ASSERT(target != NULL, "Target is null");
    *(i16*)target = value;
    return sizeof(u16);
}

/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteU16(u16 value, u8* target)
{
	COM_ASSERT(target != NULL, "Target is null");
    *(u16*)target = value;
    return sizeof(u16);
}

/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteI32(i32 value, u8* target)
{
	COM_ASSERT(target != NULL, "Target is null");
    *(i32*)target = value;
    return sizeof(i32);
}

/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteU32(u32 value, u8* target)
{
	COM_ASSERT(target != NULL, "Target is null");
    *(u32*)target = value;
    return sizeof(u32);
}

/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteF32(f32 value, u8* target)
{
	COM_ASSERT(target != NULL, "Target is null");
    *(f32*)target = value;
    return sizeof(f32);
}

/**
 * Read an i32 at the target pointer position AND move the target pointer forward
 */
com_internal u8 COM_ReadByte(u8** target)
{
	COM_ASSERT(target != NULL, "Target is null");
    u8 result = *(u8*)*target;
    *target += sizeof(u8);
    return result;
}

com_internal u16 COM_ReadU16(u8** target)
{
	COM_ASSERT(target != NULL, "Target is null");
    u16 result = *(u16*)*target;
    *target += sizeof(u16);
    return result;
}

/**
 * Read an i32 at the target pointer position AND move the target pointer forward
 */
com_internal i32 COM_ReadI32(u8** target)
{
	COM_ASSERT(target != NULL, "Target is null");
    i32 result = *(i32*)*target;
    *target += sizeof(i32);
    return result;
}

com_internal u32 COM_ReadU32(u8** target)
{
	COM_ASSERT(target != NULL, "Target is null");
    u32 result = *(u32*)*target;
    *target += sizeof(u32);
    return result;
}

com_internal f32 COM_ReadF32(u8** target)
{
	COM_ASSERT(target != NULL, "Target is null");
    f32 result = *(f32*)*target;
    *target += sizeof(f32);
    return result;
}

/**
 * Read an i32 at the target position, keeping target in place
 */
com_internal i32 COM_PeekI32(u8* target)
{
	COM_ASSERT(target != NULL, "Target is null");
    i32 result = *(i32*)target;
    return sizeof(i32);
}

/**
 * Returns number of bytes written
 */
com_internal u32 COM_CopyMemory(u8* source, u8* target, u32 numBytes)
{
	COM_ASSERT(source != NULL, "Source is null");
	COM_ASSERT(target != NULL, "Target is null");
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
com_internal i32 COM_CompareMemory(u8* ptrA, u8* ptrB, u32 numBytes)
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

com_internal void COM_ZeroMemory(u8 *ptr, u32 numBytes)
{
    u32 endPoint = (u32) ptr + numBytes;
    while ((u32)ptr < endPoint) { *ptr++ = 0; }
}

// Check if every byte in this memory is zero
com_internal i32 COM_IsZeroed(u8 *ptr, u32 numBytes)
{
    u32 endPoint = (u32) ptr + numBytes;
    u32 total = 0;
    while ((u32)ptr < endPoint)
    {
        total += *ptr;
        *ptr++;
    }
    return !total;
}

com_internal void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val)
{
    u32 endPoint = (u32) ptr + numBytes;
    while ((u32)ptr < endPoint) { *ptr++ = val; }
}

com_internal u32 COM_GetI32Sentinel()
{
	u32_union u;
	u.bytes[0] = 'D';
	u.bytes[1] = 'E';
	u.bytes[2] = 'A';
	u.bytes[3] = 'D';
	return u.value;
}

/**
 * Repeatedly write the given pattern into ptr
 * Pattern size must fit exactly into numBytes!
 */
com_internal void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize)
{
    u32 numCopies = numBytes / patternSize;
    COM_ASSERT((numBytes % patternSize) == 0, "Pattern size invalid");
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

com_internal void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes)
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
com_internal u32 COM_AlignSize(u32 value, u32 alignment)
{
    u32 remainder = value % alignment;
    if (remainder == 0) { return value; }
    remainder = alignment - remainder;
    return value + remainder;
}

com_internal i32 COM_SimpleHash(u8* ptr, i32 numBytes)
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
