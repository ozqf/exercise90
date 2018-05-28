#pragma once

#include "com_defines.h"
#include "com_memory_utils.h"

struct ByteBuffer
{
    u8* ptrStart;
    u8* ptrEnd;
    u8* ptrWrite;
    i32 capacity;
    i32 count;
};

struct BufferHeader
{
    u32 size;           // total size of buffer including headers!
    u32 itemCount;      // total number of individual item buffers to be expected
};

struct BufferItemHeader
{
    u32 type;
    u32 size;
};

/**
 * Prepare a byte buffer for writing. HeaderSizeBytes assumes a header
 * Will be written to the front AFTER the rest of the buffer has been completed
 * so will step the write position forward to make room for the header. send 0
 * if not required
 */
static inline void Buf_Start(ByteBuffer* b, u32 headerSizeBytes, u8 zeroOut)
{
    if (zeroOut) { COM_ZeroMemory(b->ptrStart, b->capacity); }
    b->count = 0;
    b->ptrWrite = b->ptrStart + headerSizeBytes;
    b->ptrEnd = b->ptrWrite;
}

// inline u32 Buf_WriteObject()
// {

// }

// #define BUF_WRITE_ITEM(objType, objSize, ptrSource, ptrDestination)

