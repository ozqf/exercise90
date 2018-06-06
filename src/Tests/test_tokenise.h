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

void Test_Tokenise()
{
	printf("Test Tokenise\n");
	
	//char* input = "  impulse 101";
	char* input = "  set sensitivity 5  ";
	//char* input = "  say Sup guys.";
	
	printf("Input string: \"%s\"\n", input);
	
	i32 results[24];
	
	ParseForTokens(input, results, 24);
}
