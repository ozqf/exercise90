#pragma once

#include "../../common/com_module.h"

void Buf_WriteMessage(ByteBuffer* b, u32 msgId, u8* bytes, u32 numBytes)
{
    Assert(b->Space() > numBytes)
    MessageHeader h;
    h.id = msgId;
    h.size = numBytes;
    b->ptrWrite += COM_COPY(&h, b->ptrWrite, sizeof(MessageHeader));
    b->ptrWrite += COM_COPY(bytes, b->ptrWrite, numBytes);
}

void Buf_WriteMessageHeader(ByteBuffer* b, u32 msgId, u32 numBytes)
{
    Assert(b->Space() > numBytes)
    b->ptrWrite += COM_WriteU32(msgId, b->ptrWrite);
    b->ptrWrite += COM_WriteU32(numBytes, b->ptrWrite);
}

u8* Buf_SearchForMessageId(u32 messageId, u8* bytes, u16 numBytes)
{
    u8* read = bytes;
    u8* end = read + numBytes;
    while (read < end)
    {
        MessageHeader h;
		
        read += COM_COPY_STRUCT(read, &h, MessageHeader);
        if (h.id == messageId)
		{
			return read - sizeof(MessageHeader);
		}
		read += h.size;
    }
	return NULL;
}

i32 Buf_InspectionCallback2(u32 type, u8* bytes, u32 numBytes)
{
    if (type == 0)
    {
        return 0;
    }
    printf("Object type %d bytes %d at %d\n", type, numBytes, (u32)bytes);
    return 1;
}

i32 Buf_InspectionCallback(u32 type, u8* bytes, u32 numBytes)
{
    if (type == 0)
    {
        return 0;
    }
    printf("Object type %d bytes %d at %d\n", type, numBytes, (u32)bytes);
    return 1;
}

i32 Buf_CollapseCallback(u32 type, u8* bytes, u32 numBytes)
{
    if (type == 2)
    {
        printf("Collapse type %d (%d bytes\n", type, numBytes);
        return 2;
    }
    // else if (type == 0)
    // {
    //     return 0;
    // }
    else
    {
        printf("No collapse\n");
        return 1;
    }
}

// callback function must have the signature (i32)(*)(u32 type, u8* bytes, u32 numBytes);
#define PARSE_BUFFER(ptr2ptrStart, ptr2ptrEnd, Callback) \
{ \
    u8* read = *ptr2ptrStart; \
    u8* end = *ptr2ptrEnd; \
    while (read < end) \
    { \
        MessageHeader h; \
        read += COM_COPY_STRUCT(read, &h, MessageHeader); \
        i32 result = Callback##(h.id, read, h.size); \
        if (result == 0) \
        { \
            printf("  End of Buffer\n"); \
            break; \
        } \
        else if (result == 2) \
        { \
            u32 space = sizeof(MessageHeader) + h.size; \
            u8* copySrc = read + h.size; \
            u8* copyDest = read - sizeof(MessageHeader); \
            u32 bytesToCopy = (end - copySrc); \
            printf("Copy back %d bytes by %d\n", bytesToCopy, space); \
            COM_COPY(copySrc, copyDest, bytesToCopy); \
            read = copyDest; \
            end -= space; \
        } \
        else \
        { \
            read += h.size; \
        } \
    } \
    *ptr2ptrEnd = end; \
}

void Stream_WriteToOutput(ReliableStream* stream, u8* bytes, u32 numBytes)
{
    u32 messageId = stream->outputSequence;
    stream->outputSequence += 1;
    Buf_WriteMessage(&stream->outputBuffer, messageId, bytes, numBytes);
}

// Write each message: output = messageId + data (no size recorded here)
u16 Stream_WriteReliableOutput(ReliableStream* stream, u8* buffer, u32 capacity)
{
    u8* write = buffer;
    u8* read = stream->outputBuffer.ptrStart;
    u8* end = (u8*)(stream->outputBuffer.ptrWrite - stream->outputBuffer.ptrStart);
    while (read < end)
    {
        MessageHeader h;
        COM_COPY_STRUCT(read, &h, MessageHeader);
        u8* src = read + sizeof(MessageHeader);
        read += sizeof(MessageHeader) + h.size;
        printf("Writing output sequence %d\n", h.id);
        // header (messageId)
        write += COM_COPY(&h.id, write, sizeof(h.id));
        // data
        write += COM_COPY(src, write, h.size);
    }
    return (u16)(write - buffer);
}

MessageHeader Buf_FindMessage(u8* bytes, u32 numBytes) { return {}; }

global_variable u8 g_msgBuffer[1024];
global_variable u8 g_testPacket[1024];

void Test_CopyOutputToPacket()
{

}

void Test_BufferWriteAndCollapse()
{
    TestMsg1 t1 = {};
    TestMsg2 t2 = {};
    TestMsg3 t3 = {};

    // Fill buffer with random stuff
    ByteBuffer b = Buf_FromBytes(g_msgBuffer, 1024);
    Buf_WriteMessage(&b, 1, (u8*)&t1, sizeof(t1));
    Buf_WriteMessage(&b, 2, (u8*)&t2, sizeof(t2));
    Buf_WriteMessage(&b, 3, (u8*)&t2, sizeof(t3));
    Buf_WriteMessage(&b, 1, (u8*)&t1, sizeof(t1));
    Buf_WriteMessage(&b, 2, (u8*)&t2, sizeof(t2));
    Buf_WriteMessage(&b, 3, (u8*)&t2, sizeof(t3));
    Buf_WriteMessage(&b, 1, (u8*)&t1, sizeof(t1));
    printf("Wrote %d bytes\n", b.Written());
    //Buf_InspectCommands(b.ptrStart, b.Written());
    // scan, collapse all '2' messages, scan again
    printf("=== SCAN ===\n");
    PARSE_BUFFER(&b.ptrStart, &b.ptrWrite, Buf_InspectionCallback);
    printf("  Written %d, Space: %d\n", b.Written(), b.Space());
    printf("=== COLLAPSE ===\n");
    PARSE_BUFFER(&b.ptrStart, &b.ptrWrite, Buf_CollapseCallback);
    printf("=== SCAN AGAIN ===\n");
    PARSE_BUFFER(&b.ptrStart, &b.ptrWrite, Buf_InspectionCallback);
    printf("  Written %d, Space: %d\n", b.Written(), b.Space());
}
