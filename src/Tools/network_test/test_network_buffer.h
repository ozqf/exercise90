#pragma once

#include "../../common/com_module.h"

/*
Required functions for storing messages
> Push a pending message into the buffer
> Scan buffer and copy out selected messages
> Pop from the buffer, collapsing the space down
> Errors if no space is left

> NULL where a header should be is the end of the buffer
*/

u8 g_buffer[2048];

struct MessageHeader
{
    u32 id;
    u32 size;
};

struct MessageBuffer
{
    u8* start;
    u32 capacity;
    u8* write;

    void Clear()
    {
        COM_ZeroMemory(this->start, this->capacity);
    }

    u32 Space()
    {
        u32 written = this->write - this->start;
        return capacity - written;
    }

    void Add(u32 id, u8* bytes, u32 size)
    {
        Assert(this->Space() > size + sizeof(MessageHeader));
        this->write += COM_WriteU32(id, this->write);
        this->write += COM_WriteU32(size, this->write);
        this->write += COM_COPY(bytes, this->write, size);
    }

    void Remove(u32 id)
    {
        u8* read = this->start;
        u8 reading = true;
        while (reading)
        {
            MessageHeader h = {};
            if (h.id == 0) { return; }
            read += COM_COPY_STRUCT(read, &h, MessageHeader);
            if (h.id == id)
            {
                // Delete and collapse!
            }
            // Jump to next message if there is one
            read += h.size;
        }
    }
};

void Test_BufferMessage(u32 messageId, u8* bytes, u16 numBytes)
{
    // Write into buffer
}

//void Test_CopyMessages(u8* buffer, )

void Test_PopMessage()
{

}
