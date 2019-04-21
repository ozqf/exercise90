#pragma once

#include "common.h"

i32 COM_ReadTokens(char* source, char* destination, char** tokens)
{
    i32 len = COM_StrLen(source);
    i32 tokensCount = 0;

    i32 readPos = 0;
    i32 writePos = 0;

    u8 reading = true;
    u8 readingToken = 0;
    while (reading)
    {
        char c = *(source + readPos);
        if (readingToken)
        {
            if (c == ' ')
            {
                *(destination + writePos) = NULL;
				readingToken = 0;
            }
            else if (c == NULL)
            {
                *(destination + writePos) = NULL;
				readingToken = 0;
                reading = false;
            }
            else
            {
                *(destination + writePos) = c;
            }
            readPos++;
            writePos++;
        }
        else
        {
            if (c == ' ' || c == '\t')
            {
                readPos++;
            }
            else if (c == NULL)
            {
                *(destination + writePos) = NULL;
                reading = false;
            }
            else
            {
                readingToken = 1;
                *(destination + writePos) = c;

                tokens[tokensCount++] = (destination + writePos);

                readPos++;
                writePos++;
            }
        }
    }

    return tokensCount;
}


char* COM_RunToNewLine(char* buffer)
{
    u8 reading = true;
    while (reading)
    {
        if (*buffer == '\n' || *buffer == EOF)
        {
            reading = false;
        }
        else
        {
            ++buffer;
        }
    }
    return buffer;
}

