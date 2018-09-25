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

static inline ByteBuffer Buf_FromBytes(u8* ptr, i32 numBytes)
{
    ByteBuffer b = {};
    b.ptrStart = ptr;
    b.ptrWrite = ptr;
    b.ptrEnd = ptr;
    b.capacity = numBytes;
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
	COM_ZeroMemory(b->ptrStart, b->capacity);
	b->ptrEnd = b->ptrStart;
	b->ptrWrite = b->ptrStart;
	b->count = 0;
}
