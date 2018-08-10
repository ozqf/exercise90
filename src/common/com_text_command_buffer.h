#pragma once

#include "com_module.h"

struct TextBuffer
{
	char* memory;
	i32 position;
	i32 capacity;
};

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
