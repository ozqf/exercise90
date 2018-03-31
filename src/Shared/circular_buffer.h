#pragma once

// TODO: Port this over!

#include <stdio.h> /* printf, scanf, NULL */
#include <stdlib.h> /* malloc, free, rand */

#include <stdint.h>

#define f32 float
#define f64 double
#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

char* testStr1 = "abcdefghijklmnopqrstuvwxyz";

#define Assert(expression) if(!(expression)) { printf("Assert fail at %d in %s Build Date %s\n", __LINE__, __FILE__, __DATE__); *(int*)0 = 0; }

#define OUTPUT_BUFFER_SIZE 128
char outputBuffer[128];

struct CircularBuffer
{
    u8* ptrMemory;
    i32 capacity;
    i32 position;
    u8 terminator;
};

static CircularBuffer tBuf = {};

#define INPUT_BUFFER_SIZE 16
static CircularBuffer inputBuf = {};

void COM_SetMemory(u8* ptr, u32 numBytes, u8 value)
{
    while (numBytes--)
    {
        *ptr++ = value;
    }
}

void CBuf_Alloc(CircularBuffer* buffer, i32 capacity, u8 blockTerminator, u8 clearValue)
{
    // +1 for guard value
    buffer->ptrMemory = (u8*)malloc(capacity + 1);
    buffer->capacity = capacity;
    buffer->position = 0;
    buffer->terminator = blockTerminator;
    COM_SetMemory(buffer->ptrMemory, capacity, clearValue);
    // Place guard value NULL to safely allow the buffer to be dumped
    u8* guardValue = buffer->ptrMemory + capacity;
    *guardValue = 0;
}

inline i32 CBuf_StepIndexForward(CircularBuffer* buffer, i32 currentIndex)
{
    ++currentIndex;
    if (currentIndex >= buffer->capacity) { currentIndex = 0; }
    return currentIndex;
}

inline i32 CBuf_StepIndexBackward(CircularBuffer* buffer, i32 currentIndex)
{
    --currentIndex;
    if (currentIndex < 0) { currentIndex = buffer->capacity - 1; }
    return currentIndex;
}

/**
 * Doesn't really work eh?
 */
inline i32 CBuf_Space(CircularBuffer* buffer)
{
    i32 pos = buffer->position;
    i32 count = 0;
    while (buffer->ptrMemory[pos] != buffer->terminator)
    {
        pos = CBuf_StepIndexForward(buffer, pos);
        count++;
    }
    return count;
}

inline void CBuf_EndBlock(CircularBuffer* buffer)
{
    buffer->ptrMemory[buffer->position] = buffer->terminator;
    buffer->position = CBuf_StepIndexForward(buffer, buffer->position);
}

inline void CBuf_WriteValue(CircularBuffer* buffer, u8 value)
{
    buffer->ptrMemory[buffer->position] = value;
    buffer->position = CBuf_StepIndexForward(buffer, buffer->position);
}

inline void CBuf_WriteArray(CircularBuffer* buffer, u8* array, i32 length, u8 endBlock)
{
    for (int i = 0; i < length; ++i)
    {
        CBuf_WriteValue(buffer, array[i]);
    }
    if (endBlock)
    {
        CBuf_EndBlock(buffer);
    }
}

void CBuf_DebugPrint(CircularBuffer* buffer)
{
    printf("Buffer contents: '%s'\n", buffer->ptrMemory);
}

inline void CBuf_WriteSection(CircularBuffer* buffer, i32 firstPosition, i32 lastPosition, char* target)
{
    while (firstPosition != lastPosition)
    {
        *target = buffer->ptrMemory[firstPosition];
        target++;
        firstPosition = CBuf_StepIndexForward(buffer, firstPosition);
    }
}

/**
 * Return number of lines in buffer, but stops counting at the given max value
 */
inline i32 CBuf_CountLines(CircularBuffer* buffer, i32 max)
{
    i32 end =  CBuf_StepIndexBackward(buffer, buffer->position);
    end = CBuf_StepIndexBackward(buffer, end);
    i32 start = CBuf_StepIndexBackward(buffer, end);
    i32 count = 0;
    while (start != end && count < max)
    {
        if (buffer->ptrMemory[start] == buffer->terminator)
        {
            count++;
        }
        start = CBuf_StepIndexBackward(buffer, start);
        if (start == buffer->position) { break; }
    }
    return count;
}

/**
 * Returns the start position of the line
 */
i32 CBuf_DebugPrintLineFrom(CircularBuffer* buffer, i32 position)
{
    COM_SetMemory((u8*)outputBuffer, OUTPUT_BUFFER_SIZE, '_');
    // Make sure output is null terminated
    u8* guardPos = (u8*)outputBuffer + (OUTPUT_BUFFER_SIZE - 1);
    COM_SetMemory(guardPos, 1, 0);
    
    char* ptr = outputBuffer;

    // step from start of next line onto the terminator of the last line
    i32 end =  CBuf_StepIndexBackward(buffer, position);
    // step back to just before the terminator
    end = CBuf_StepIndexBackward(buffer, end);
    // step backward again to start sampling the rest of the line
    i32 start = CBuf_StepIndexBackward(buffer, end);

    char c = buffer->ptrMemory[start];
    while (c != 0 && c != buffer->terminator)
    {
        start = CBuf_StepIndexBackward(buffer, start);
        c = buffer->ptrMemory[start];
    }
    // Step forward off the terminator of the line before this one
    start = CBuf_StepIndexForward(buffer, start);
    
    printf("Buffer has %d lines. Last line is indices %d to %d: %c to %c\n",
        CBuf_CountLines(buffer, 99999),
        start,
        end,
        buffer->ptrMemory[start],
        buffer->ptrMemory[end]
    );
    return start;
}

i32 CBuf_DebugPrintLines(CircularBuffer* buffer, i32 numLines)
{
    i32 pos = buffer->position;
    while(numLines--)
    {
        pos = CBuf_DebugPrintLineFrom(buffer, pos);
    }
    return pos;
}

void GetInput(CircularBuffer* buf)
{
    buf->position = 0;
    char ch;
    for (;;)
    {
        ch = (u8)fgetc(stdin);
        if (ch == EOF || ch == '\n')
        {
            break;
        }
        
        // < capacity - 1 because we need to leave a space to add a NULL terminator at the end!
        if (buf->position < buf->capacity - 1)
        {
            buf->ptrMemory[buf->position] = ch;
            ++buf->position;
        }
    }
    // add NULL terminator
    buf->ptrMemory[buf->position + 1] = 0;
    buf->position = 0;
}

int main()
{
    printf("=== CIRCULAR BUFFER TEST ===\n");
    CBuf_Alloc(&tBuf, 32, '_', 0);
    CBuf_DebugPrint(&tBuf);
    
    CBuf_DebugPrintLineFrom(&tBuf, tBuf.position);
    Assert(true);

    printf("Write some characters:\n");
    CBuf_WriteArray(&tBuf, (u8*)"ABCDEFGHIJKLMNOPQRSTUVWYXZ", 26, 1);
    CBuf_DebugPrint(&tBuf);
    CBuf_DebugPrintLineFrom(&tBuf, tBuf.position);
    printf("Space left: %d. Position: %d\n", CBuf_Space(&tBuf), tBuf.position);
    
    printf("Write some characters again:\n");
    CBuf_WriteArray(&tBuf, (u8*)"x123456789", 10, 1);
    CBuf_DebugPrint(&tBuf);
    CBuf_DebugPrintLineFrom(&tBuf, tBuf.position);
    printf("Space left: %d. Position: %d\n", CBuf_Space(&tBuf), tBuf.position);
    
    printf("Write a terminator:\n");
    char terminatorArr[1];
    terminatorArr[0] = tBuf.terminator;
    CBuf_WriteArray(&tBuf, (u8*)terminatorArr, 1, 1);
    CBuf_DebugPrint(&tBuf);
    CBuf_DebugPrintLineFrom(&tBuf, tBuf.position);
    printf("Space left: %d. Position: %d\n", CBuf_Space(&tBuf), tBuf.position);
    
    printf("Write some characters yet again:\n");
    CBuf_WriteArray(&tBuf, (u8*)"abcdefghijklmnopqrstuvwxyz", 26, 1);
    CBuf_DebugPrint(&tBuf);
    CBuf_DebugPrintLineFrom(&tBuf, tBuf.position);
    printf("Space left: %d. Position: %d\n", CBuf_Space(&tBuf), tBuf.position);

    printf("%s\n", outputBuffer);

    CBuf_DebugPrintLines(&tBuf, CBuf_CountLines(&tBuf, 99999));

    printf("\n=== Input test ===\n");
    CBuf_Alloc(&inputBuf, INPUT_BUFFER_SIZE, 0, 0);
    GetInput(&inputBuf);
    printf("Input: '%s'\n", inputBuf.ptrMemory);
}