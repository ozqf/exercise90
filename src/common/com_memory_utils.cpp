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
    i32 result = *(i32*)target;
    *target += sizeof(i32);
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

/**
 * if strings are equal, return 0
 * if a is first alphabetically, return -1
 * if b is first alphabetically, return 1
 */

static inline i32 COM_CompareStrings(const char *a, const char *b)
{
    while (*a == *b)
    {
        // End of string
        if (*a == NULL)
        {
            return 0;
        }
        ++a;
        ++b;
    }

    return ((*a < *b) ? -1 : 1);
}

static inline void COM_CopyStringA(const char *source, char *target)
{
    while (true)
    {
        *target = *source;
        if (*source == NULL)
        {
            return;
        }
        ++target;
        ++source;
    }
}

static inline void COM_CopyString(const char *source, char *target)
{
    while (*source)
    {
        *target++ = *source++;
    }
    *target = 0;
}

/**
 * Copy a string without exceeding the specified limit
 * Limit does NOT include NULL terminator
 */
static inline void COM_CopyStringLimited(const char *source, char *target, i32 limit)
{
    while (*source)
    {
        *target++ = *source++;
        --limit;
        if (limit == 0) { break; }
    }
    *target = 0;
}

/**
 * No checking whether source/target will clash
 * No checking that target has enough room!
 */
static inline void COM_CopyStringCount(const char *source, char *target, i32 count)
{
    if (count <= 0) { return; }
    while (*source && count)
    {
        *target++ = *source++;
        --count;
    }

    // Pad remaining space with zeros
    while (count)
    {
        *target++ = 0;
        --count;
    }

    *target = 0;
}

static inline i32 COM_StrLenA(const char* str)
{
    i32 result = 0;
    while (*str)
    {
        ++str;
        ++result;
    }
    return result;
}

static inline i32 COM_StrLen(const char* str)
{
    i32 count = 0;
    while (str[count]) { ++count; }
    return count;
}

// decimal or hexadecimal
// negative and positive
// "-54" "12" "0x432146fd" "-0X4AbdC"
static inline i32 COM_AsciToInt32(const char *str)
{
    i32 sign = 1;
    i32 val = 0;
    char c;
    if (*str == '-')
    {
        sign = -1;
        ++str;
    }

    // hexadecimal
    if (*str == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        // Move past first two characters
        str += 2;
        while (1)
        {
            c = *str;
            ++str;
            if (c >= '0' && c <= '9')
            {
                val = val * 16 + c - '0';
            }
            else if (c >= 'a' && c <= 'f')
            {
                val = val * 16 + c - 'a' + 10;
            }
            else if (c >= 'A' && c <= 'F')
            {
                val = val * 16 + c - 'A' + 10;
            }
            else
            {
                return val * sign;
            }
        }
    }

    // decimal
    while (true)
    {
        c = *str;
        ++str;
        if (c < '0' || c > '9')
        {
            // no numerical character
            return sign * val;
        }
        // '0' char = 48 in asci, so
        // c minus '0' = c minus 48
        // val * 10 moves to next digit
        // then add new value
        val = (val * 10) + (c - '0');
    }
    return val * sign;
}

static inline f32 COM_AsciToFloat32(const char *str)
{
    return 0.0f;
}

static inline i32 ZSTR_WriteChars(ZStringHeader* str, char* sourceChars, i32 numChars)
{
    i32 index = 0;
    while (numChars > 0 && str->length < str->maxLength)
    {
        str->chars[str->length++] = sourceChars[index++];
    }
    return index;
}
