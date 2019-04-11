#pragma once

#include "com_defines.h"


/**
 * if strings are equal, return 0
 * if a is first alphabetically, return -1
 * if b is first alphabetically, return 1
 */

com_internal i32 COM_CompareStrings(const char *a, const char *b)
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
com_internal i32 COM_MatchStringStart(const char *str, const char *start)
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

com_internal void COM_CopyStringA(const char *source, char *target)
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

com_internal void COM_CopyString(const char *source, char *target)
{
    while (*source)
    {
        *target++ = *source++;
    }
    *target = 0;
}

/**
 * Copy a string without exceeding the specified limit
 * Will always patch a NULL terminator at the final position
 */
com_internal i32 COM_CopyStringLimited(const char *source, char *target, i32 limit)
{
    if (limit < 1) { return 0; }
    i32 written = 0;
    while (*source && limit > 0)
    {
        *target++ = *source++;
        --limit;
        ++written;
        //if (limit == 0) { break; }
    }
	target[limit - 1] = '\0';
    return ++written;
}

/**
 * No checking whether source/target will clash
 * No checking that target has enough room!
 */
com_internal void COM_CopyStringCount(const char *source, char *target, i32 count)
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

// com_internal i32 COM_StrLenA(const char* str)
// {
//     i32 result = 0;
//     while (*str)
//     {
//         ++str;
//         ++result;
//     }
//     return result;
// }

com_internal i32 COM_StrLen(const char* str)
{
    i32 count = 0;
    while (str[count]) { ++count; }
    return count;
}

com_internal i32 COM_StrReplace(char* str, char target, char replacement)
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

com_internal void COM_StrToUpperCase(char* str)
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

com_internal void COM_StrToLowerCase(char* str)
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

com_internal u8 COM_CharIsDecimalInt32(const char c)
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

com_internal u8 COM_AsciIsDecimalInt32(const char *str)
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
com_internal i32 COM_AsciToInt32(const char *str)
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

com_internal f32 COM_AsciToFloat32(const char *str)
{
    return 0.0f;
}

com_internal i32 COM_StripTrailingInteger(char* text, char separator, i32 failureValue)
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
	//printf("\n");
	return result;
}

/**
 * Tests that the end of the given filePath matches the given extension
 */
com_internal u8 COM_CheckForFileExtension(const char* filePath, const char* extension)
{
    COM_ASSERT(filePath != NULL, "Check extension sent null file path");
    COM_ASSERT(extension != NULL, "Check extension sent null extension");

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

com_internal i32 COM_ConcatonateTokens(
	char* buffer,
	i32 bufferSize,
	char** strings,
	i32 numStrings,
	i32 firstString)
{
	i32 totalWritten = 0;
	i32 pos = 0;
	i32 remaining = bufferSize;
	i32 written = 0;
	char* dest = buffer;
	//printf("Concatonating %d strings, buf size %d\n", numStrings, bufferSize);
	for (int i = firstString; i < numStrings; ++i)
	{
		//printf("Copying %s to %d, with %d remaining\n", strings[i], pos, remaining);
		dest = buffer + pos;
		written = COM_CopyStringLimited(strings[i], dest, remaining);
		*(dest + (written -1 )) = ' ';
		totalWritten += written;
		pos += written;
		remaining -= written;
		//printf("Wrote %d result %s. Remaining %d\n", written, buffer, remaining);
		if (remaining <= 0) { break; }
	}
	*(dest + (written -1 )) = '\0';
	return totalWritten;
}
