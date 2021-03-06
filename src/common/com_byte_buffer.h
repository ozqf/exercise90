#pragma once

#include "com_defines.h"
#include "com_memory_utils.h"

#define BUF_COPY(ptrToByteBuffer, ptrToByteArray, numOfBytesInArray) \
{##ptrToByteBuffer##->ptrWrite += \
    COM_CopyMemory((u8*)##ptrToByteArray##, ##ptrToByteBuffer##->ptrWrite##, numOfBytesInArray##);}

struct ByteBuffer
{
    u8* ptrStart;
    u8* ptrEnd;
    u8* ptrWrite;
    i32 capacity;

    i32 Written()
    {
        return this->ptrWrite - this->ptrStart;
    }
    i32 Space()
    {
        return capacity - (ptrWrite - ptrStart);
    }
};

struct DoubleByteBuffer
{
    i32 swapped;
    ByteBuffer a;
    ByteBuffer b;

    ByteBuffer* GetRead()
    {
        return this->swapped ? &this->b : &this->a;
    }

    ByteBuffer* GetWrite()
    {
        return this->swapped ? &this->a : &this->b;
    }

    void Swap()
    {
        swapped = !swapped;
    }
};

extern "C" com_internal i32 Buf_BytesWritten(ByteBuffer* b);
com_internal ByteBuffer Buf_FromBytes(u8* ptr, i32 numBytes);
com_internal ByteBuffer Buf_FromMalloc(void* ptr, i32 size);
com_internal ByteBuffer Buf_FromMemoryBlock(MemoryBlock mem);
com_internal void Buf_Clear(ByteBuffer* b);
