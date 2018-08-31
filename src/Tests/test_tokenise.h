#pragma once

#include "../common/com_module.h"



void ParseForTokens(char* input, i32* results, i32 maxResults)
{
	i32 len = COM_StrLen(input);

	char copy[32];
	COM_CopyString(input, copy);

	input = copy;

	printf("Tokenise %s (%d chars)\n", input, len);
	
	char* reader = input;
	char* terminator = input + len;
	
	u8 readingToken = 0;
	i32 nextTokenIndex = 0;
	i32 tokensRead = 0;
	
	i32 loopCount = 0;
	
	for (;;)
	{
		//printf("%d > ", loopCount++);
		char c = *reader;
		char* current = reader;
		// Make sure reader is stepped forward
		reader++;
		//printf("Char: %c Byte: %d\n", *current, (u8)*current);
		if (readingToken)
		{
			if (c == ' ' || c == NULL)
			{
				// Close string
				
				if (current !=0)
				{
	#if 0
					printf("Finishing token %d. Current PTR (%d) %c (%d) set to NULL\n",
						(nextTokenIndex - 1),
						(u32)current,
						*current,
						(u8)*current);
	#endif
					// TODO: Why don't I work? Address appears legal...
					// because it might be a compile time constant string, dumbass
					*current = 0;
				}
				else
				{
					//printf("Current == NULL\n");
				}
				// Finish token
				readingToken = 0;
				tokensRead++;
			}
			if (c == NULL)
			{
				break;
			}
		}
		else
		{
			if (c == ' ')
			{
				// eat whitespace
				continue;
			}
			else if (c == NULL)
			{
				break;
			}
			else
			{
				//tokenIndex = (i32)(current - input);
				results[nextTokenIndex] = (i32)(current - input);
				//printf("Starting token '%d' from char '%c'\n", nextTokenIndex, c);
				nextTokenIndex++;
				readingToken = 1;
			}
		}
	}
	
	printf("Tokens read: (%d)\n", tokensRead);
	for (i32 i = 0; i < tokensRead; ++i)
	{
		printf("%d: %d\n", i, results[i]);
	}
	printf("Tokens\n\n");
	for (i32 i = 0; i < tokensRead; ++i)
	{
		char* str = (char*)(input + results[i]);
		printf("%s\n", str);
	}
	
	printf("\n");
}

void Test_ExecTextBuffer(char* buffer)
{
	char executeBuffer[256];
	i32 written = 0;
	i32 position = 0;
	for (;;)
	{
		written = COM_DequeueTextCommand(buffer, executeBuffer, position, 256);
		position += written;
		if (written <= 1)
		{
			break;
		}

		char tokensBuffer[256];
		char *tokens[32];

		i32 numTokens = COM_ReadTokens(executeBuffer, tokensBuffer, tokens);
		printf("Read %d tokens\n", numTokens);
		for (i32 i = 0; i < numTokens; ++i)
		{
			printf("\"%s\"\n", tokens[i]);
		}
	}
}

void Test_MultipleCommands()
{
	// Create buffer
	char bufferMem[1024];
	COM_ZeroMemory((u8*)bufferMem, 1024);
	i32 position = 0;
	printf("=== Command batch 1 ===\n");
	// load commands
	position = COM_EnqueueTextCommand("START TESTBOX.LVL", bufferMem, position, 1024);
	printf("Position: %d\n", position);
	position = COM_EnqueueTextCommand("Say foo say bar say what", bufferMem, position, 1024);
	printf("Position: %d\n", position);
	
	printf("Result:\n--------------------------\n%s\n------------------------\n", bufferMem);

	// execute
	Test_ExecTextBuffer(bufferMem);

	printf("=== Command batch 2 ===\n");
	
	position = 0;
	position = COM_EnqueueTextCommand("Flibble biscuit", bufferMem, position, 1024);
	printf("Position: %d\n", position);
	position = COM_EnqueueTextCommand("Isn't that right Bindy Badgy?", bufferMem, position, 1024);
	printf("Position: %d\n", position);
	
	printf("Result:\n--------------------------\n%s\n------------------------\n", bufferMem);
	
	// execute
	Test_ExecTextBuffer(bufferMem);

	
	printf("\nDone\n");
}

void Test_StringIsDecimal(char* str)
{
	u8 result = COM_AsciIsDecimalInt32(str);
	printf("%s is decimal i32: %d\n", str, result);
}

void Test_TokenisePattern()
{
	char tokensBuffer[256];
	char *tokens[32];

	i32 numTokens = COM_ReadTokens("SPAWN ENEMY 1 2 3", tokensBuffer, tokens);
	printf("Read %d tokens\n", numTokens);
	u8 patternMatch = COM_CheckTokenPattern(tokens, numTokens, "ttnnn");
	printf("Pattern matched: %d\n", patternMatch);
}

void Test_Tokenise()
{
	printf("*** Test Tokenise ***\n");

	//Test_MultipleCommands();

	Test_StringIsDecimal("foo");
	Test_StringIsDecimal("-1");
	Test_StringIsDecimal("37");
	Test_StringIsDecimal("");

	Test_TokenisePattern();

	//char* input = "  set sensitivity 5  ";
	//printf("Input string: \"%s\"\n", input);
	
	#if 0 // Test internal function
	i32 results[24];
	ParseForTokens(input, results, 24);
	#endif

	#if 0 // Common lib function
	char commandCopy[256];
    char tokensBuffer[256];
    char *tokens[32];

	COM_CopyString(input, commandCopy);
	i32 numTokens = COM_ReadTokens(commandCopy, tokensBuffer, tokens);
	printf("Read %d tokens\n", numTokens);
	for (i32 i = 0; i < numTokens; ++i)
	{
		printf("\"%s\"\n", tokens[i]);
	}
	#endif
}
