#pragma once

#include "common.h"

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
u8 COM_ValidateTextCommandHandler(
	char* name, i32 minTokens, i32 maxTokens, ZParseTextCommand func);
// pattern is either t for text, or n for numeric:
// eg spawn enemy 3 5 7 would be ttnnn\0
u8 COM_CheckTokenPattern(char** tokens, i32 numTokens, char* pattern);
/*
-- Example command strings --
EXIT\0
CMD 1\nCMD 2\0
CMD 1\nWAIT\nCMD 2\0
*/

// Returns new position
i32 COM_EnqueueTextCommand(
	char* command, char* buffer, i32 position, i32 capacity);
// returns command chars written
i32 COM_DequeueTextCommand(
	char* buffer, char* target, i32 position, i32 targetCapacity);