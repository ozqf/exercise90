#pragma once

#include "com_module.h"

struct TextBuffer
{
	char* memory;
	i32 position;
	i32 capacity;
};

// return 1 if command is handled
typedef u8 (*ZParseTextCommand)(char* text, char** tokens, i32 numTokens);

#define ZTEXT_COMMAND_HANDLER_NAME_LENGTH 32
struct ZTextCommandHandler
{
	u8 inUse;
	char name[ZTEXT_COMMAND_HANDLER_NAME_LENGTH];
	i32 minTokens;	// must be >= 1. name == first token
	i32 maxTokens;
	ZParseTextCommand func;
};

// returns 0 if no errors found
inline u8 COM_ValidateTextCommandHandler(char* name, i32 minTokens, i32 maxTokens, ZParseTextCommand func)
{
	i32 len = COM_StrLen(name);
    if (len > 32 || len <= 0)
    {
        return 1;
    }

    if (minTokens < 1)
    {
        return 2;
    }
    if (minTokens > maxTokens)
    {
        return 3;
    }

    if (func == NULL)
    {
        return 4;
    }

    return 0;
}

// pattern is either t for text, or n for numeric:
// eg spawn enemy 3 5 7 would be ttnnn\0
inline u8 COM_CheckTokenPattern(char** tokens, i32 numTokens, char* pattern)
{
	i32 patternLength = COM_StrLen(pattern);
	
	if (patternLength != numTokens)
	{
		return 0;
	}
	for (i32 i = 0; i < patternLength; ++i)
	{
		char c = pattern[i];
		printf("Checking token %s vs pattern symbol %c\n", tokens[i], c);
		if (c == 'n')
		{
			if (!COM_AsciIsDecimalInt32(tokens[i]))
			{
				return 0;
			}
		}
		else if (c == 't')
		{
			continue;
		}
		else
		{
			printf("Unknown pattern symbol %c\n", c);
			return 0;
		}
	}
	return 1;
}

/*
-- Example command strings --
EXIT\0
CMD 1\nCMD 2\0
CMD 1\nWAIT\nCMD 2\0
*/

// Returns new position
inline i32 COM_EnqueueTextCommand(char* command, char* buffer, i32 position, i32 capacity)
{
	i32 dataSize = sizeof(char);
	i32 len = COM_StrLen(command);
	char* ptrOrigin = buffer + (dataSize * position);
	char* ptrWrite = ptrOrigin;
	COM_CopyString(command, ptrWrite);
	ptrWrite += len * dataSize;
	*ptrWrite = '\n';
	ptrWrite += dataSize;
	i32 wrote = (ptrWrite - ptrOrigin);
	//printf("wrote %d chars\n", wrote);
	return position + wrote;
}

// returns command chars written
inline i32 COM_DequeueTextCommand(char* buffer, char* target, i32 position, i32 targetCapacity)
{
	char* ptrRead = buffer + position;
	char c = *ptrRead;
	i32 written = 0;
	while (c != '\n' && c != '\0')
	{
		*target++ = c;
		*ptrRead++ = '\0';
		written++;
		c = *ptrRead;
	}
	// terminate string
	*target = '\0';
	written++;
	return written;
}
