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

// is this necessary?
//struct BufferHeader
//{
//    u32 size;           // total size of buffer including headers!
//    u32 itemCount;      // total number of individual item buffers to be expected
//};

struct BufferItemHeader
{
    u32 type;
    u32 size;
};

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

/**
 * Write an object to the byte buffer preceeded by the specified header
 * Automatically: Asserts capacity, advances the buffers count, and ptrWrite pointer
 */
inline void Buf_WriteObject(ByteBuffer* b, u8* source, u32 itemType, u32 itemSize)
{
    u32 used = b->ptrWrite - b->ptrStart;
    u32 space = b->capacity - used;
    Assert((space - used) > itemSize + sizeof(BufferItemHeader));

    BufferItemHeader header = {};
    header.type = itemType;
    header.size = itemSize;

	u32 numHeaderBytes = sizeof(BufferItemHeader);

    b->ptrWrite += COM_COPY(&header, b->ptrWrite, numHeaderBytes);
    b->ptrWrite += COM_COPY(source, b->ptrWrite, itemSize);
    b->count++;

	// Trap to catch specific items being written
	Assert(itemType != 2);

}
