#pragma once

#include "com_defines.h"
#include "com_memory_utils.h"

struct ByteBuffer
{
    u8* ptrStart;
    u8* ptrEnd;
    u8* ptrWrite;
    i32 capacity;

    inline i32 Written()
    {
        return this->ptrWrite - this->ptrStart;
    }
    inline i32 Space()
    {
        return capacity - (ptrWrite - ptrStart);
    }
};

static inline i32 Buf_BytesWritten(ByteBuffer* b)
{
    return (b->ptrWrite - b->ptrStart);
}

static inline ByteBuffer Buf_FromBytes(u8* ptr, i32 numBytes)
{
    ByteBuffer b = {};
    b.ptrStart = ptr;
    b.ptrWrite = ptr;
    b.ptrEnd = ptr;
    b.capacity = numBytes;
    return b;
}

static inline ByteBuffer Buf_FromMalloc(void* ptr, i32 size)
{
    ByteBuffer b = {};
    b.ptrStart = (u8*)ptr;
    b.ptrWrite = b.ptrStart;
    b.ptrEnd = b.ptrStart;
    b.capacity = size;
    return b;
}

static inline ByteBuffer Buf_FromMemoryBlock(MemoryBlock mem)
{
    ByteBuffer b = {};
    b.ptrStart = (u8*)mem.ptrMemory;
    b.ptrWrite = b.ptrStart;
    b.ptrEnd = b.ptrStart;
    b.capacity = mem.size;
    return b;
}

static inline void Buf_Clear(ByteBuffer* b)
{
    if (b->ptrStart != NULL)
    {
        COM_ZeroMemory(b->ptrStart, b->capacity);
    }
	b->ptrEnd = b->ptrStart;
	b->ptrWrite = b->ptrStart;
}
