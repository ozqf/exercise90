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
static inline u32 COM_WriteU16(u16 value, u8* target)
{
	Assert(target != NULL);
    *(i32*)target = value;
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

// returns 1 if true, 0 if not
static inline i32 COM_MatchStringStart(const char *str, const char *start)
{
    i32 strLen = COM_StrLen(str);
	i32 startLen = COM_StrLen(start);
	if (strLen < startLen) { return 0; }
	if (strLen == 0) { return 0; }
	if (startLen == 0) { return 0; }
	i32 i = 0;
	for(;;)
	{
		char a = str[i];
		char b = start[i];
		if (b == '\0') { return 1; }
		i++;
		if (a != b) { return 0; }
	}
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
static inline i32 COM_CopyStringLimited(const char *source, char *target, i32 limit)
{
    i32 written = 0;
    while (*source)
    {
        *target++ = *source++;
        --limit;
        ++written;
        if (limit == 0) { break; }
    }
    *target = 0;
    return ++written;
}

/**
 * Copy a string without exceeding the specified limit
 * Limit will patch in a null terminator
 */
static inline void COM_CopyStringLimited_Safe(const char *source, char *target, i32 limit)
{
    Assert(limit > 1);
    while (*source)
    {
        // Null terminate
        if (limit == 1)
        {
            *target = NULL;
            break;
        }
        *target++ = *source++;
        --limit;
        //if (limit == 0) { break; }
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

static inline i32 COM_StrReplace(char* str, char target, char replacement)
{
    i32 count = 0;
    while (*str)
    {
        if (*str == target)
        {
            *str = replacement;
            ++count;
        }
        str++;
    }
    return count;
}

static inline void COM_StrToUpperCase(char* str)
{
    // A to B == 65 to 90
    // a to b == 97 to 122
    while (*str)
    {
        if (*str >= (char)97 && *str <= (char)122)
        {
            *str -= 32;
        }
        str++;
    }
}

static inline void COM_StrToLowerCase(char* str)
{
    // A to B == 65 to 90
    // a to b == 97 to 122
    while (*str)
    {
        if (*str >= (char)65 && *str <= (char)90)
        {
            *str += 32;
        }
        str++;
    }
}

static inline u8 COM_CharIsDecimalInt32(const char c)
{
	if (c < '0' || c > '9')
	{
		if (c != '-')
		{
			return 0;
		}
    }
	return 1;
}

static inline u8 COM_AsciIsDecimalInt32(const char *str)
{
    i32 read = 0;
	while (str && *str)
	{
        char c = *str;
		if (c < '0' || c > '9')
        {
            if (c != '-')
            {
                return 0;
            }
            
        }
        ++read;
		++str;
	}
    return (read > 0);
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

static inline i32 COM_StripTrailingInteger(char* text, char separator, i32 failureValue)
{
	i32 pos = COM_StrLen(text);
	if (pos <= 0) { return failureValue; }
	pos -= 1;	// step back to last char, over null terminator
	i32 result = failureValue;
	for (;;)
	{
		char c = text[pos];
		//printf("%c, ", c);
		if (c == separator)
		{
			result = COM_AsciToInt32(&text[++pos]);
			break;
		}
		else if (!COM_CharIsDecimalInt32(c))
		{
			break;
		}
		else if (pos == 0)
		{
			result = COM_AsciToInt32(&text[pos]);
			break;
		}
		pos--;
	}
	printf("\n");
	return result;
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

/**
 * Tests that the end of the given filePath matches the given extension
 */
static inline u8 COM_CheckForFileExtension(const char* filePath, const char* extension)
{
    Assert(filePath != NULL);
    Assert(extension != NULL);

    // Calc length of filename so it can be read backwards
    i32 fileNameLength = 0;
    while (filePath[fileNameLength] != 0) { fileNameLength++; }

    i32 extensionLength = 0;
    while (extension[extensionLength] != 0) { extensionLength++; }

    if (fileNameLength <= extensionLength)
    {
        return 0;
    }
    
    // step both back to last char
    i32 fNamePos = fileNameLength - 1;
    i32 exPos = extensionLength - 1;

    while (exPos >= 0)
    {
        if (filePath[fNamePos] != extension[exPos])
        {
            return 0;
        }
        --fNamePos;
        --exPos;
    }
    return 1;
}
