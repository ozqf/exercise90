#pragma once

/**
 * Handles reliable input and output messages buffers
 * Send:
 * app -> output buffer -> packet -----> network
 * Receive
 * network -----> packet -> input buffer -> app
 * 
 * output buffer is continuously sent until successful ack
 * input is buffered and only executed in sequence
 */
#include "app_module.cpp"


void Buf_WriteMessage(ByteBuffer* b, u32 msgId, u8* bytes, u32 numBytes)
{
	APP_ASSERT(b->ptrWrite, "Buf write message destination is null")
    APP_ASSERT(b->Space() > numBytes, "Buf write message has no space left")
    StreamMsgHeader h;
    h.id = msgId;
    h.size = numBytes;
    b->ptrWrite += COM_COPY(&h, b->ptrWrite, sizeof(StreamMsgHeader));
    b->ptrWrite += COM_COPY(bytes, b->ptrWrite, numBytes);
}

u32 Stream_WriteStreamMsgHeader(u8* ptr, u32 msgId, u32 numBytes)
{
    u8* start = ptr;
    ptr += COM_WriteU32(msgId, ptr);
    ptr += COM_WriteU32(numBytes, ptr);
    return (ptr - start);
}

void Buf_WriteStreamMsgHeader(ByteBuffer* b, u32 msgId, u32 numBytes)
{
    Assert(b->Space() > numBytes)
    b->ptrWrite += Stream_WriteStreamMsgHeader(b->ptrWrite, msgId, numBytes);
}

inline u16 Stream_PackMessageHeader(u8 sequenceOffset, u16 size)
{
    sequenceOffset = sequenceOffset & SIX_BIT_MASK;
    size = size & TEN_BIT_MASK;
    return ((sequenceOffset << 10) | size);
}

inline void Stream_UnpackMessageHeader(u16 header, u8* sequenceOffset, u16* size)
{
    *sequenceOffset = (header >> 10) & SIX_BIT_MASK;
    *size = header & TEN_BIT_MASK;
}

/**
 * Select and overwrite a previous transmission record
 */
TransmissionRecord* Stream_AssignTransmissionRecord(
    TransmissionRecord* records, u32 sequence)
{
	TransmissionRecord* result = &records[sequence % MAX_TRANSMISSION_RECORDS];
    result->sequence = sequence;
    result->numReliableMessages = 0;
	return result;
}

/**
 * Search for a transmission record matching a specific sequence.
 * Returns null if one is not found (this may happen for very old packets
 * that have fallen out of the current ack window)
 */
TransmissionRecord* Stream_FindTransmissionRecord(
    TransmissionRecord* records, u32 sequence)
{
	TransmissionRecord* result = &records[sequence % MAX_TRANSMISSION_RECORDS];
	if (result->sequence == sequence)
	{
		return result;
	}
	return NULL;
}

void Stream_PrintPacketManifest(u8* read, u16 numBytes)
{
	printf("=== PACKET MANIFEST ===\n");
    u8* start = read;
	u16 reliableBytes = COM_ReadU16(&read);
    u8* end;
	printf("%d reliable Bytes\n", numBytes);
	if (reliableBytes != 0)
	{
        u32 reliableSequence = COM_ReadU32(&read);
        printf("Reliable sequence: %d\n", reliableSequence);
        end = read + reliableBytes;
        while (read < end)
        {
            u16 header = 0;
            u8 sequenceOffset = 0;
            u16 size = 0;
		    Stream_UnpackMessageHeader(header, &sequenceOffset, &size);
            u8 type = *read;
            printf("Type %d, seq offset %d, size %d\n",
                type, sequenceOffset, size);
            read += size;
        }
        
        
	}
    u32 sync = COM_ReadI32(&read);
    printf("Deserialise check: %d\n", sync);
    u16 unreliableBytes = COM_ReadU16(&read);
    printf("Unreliable bytes %d\n", unreliableBytes);
    end = read + unreliableBytes;
    while (read < end)
    {
        u16 header = 0;
        u8 sequenceOffset = 0;
        u16 size = 0;
	    Stream_UnpackMessageHeader(header, &sequenceOffset, &size);
        u8 type = *read;
        printf("Type %d, seq offset %d, size %d\n",
            type, sequenceOffset, size);
        read += size;
    }
}

void Stream_PrintBufferManifest(ByteBuffer* b)
{
    printf("-- Buffer Manifest --\n");
    if (b->Written() == 0)
    {
        printf("Buffer is empty\n");
        return;
    }
    u8* read = b->ptrStart;
    u8* end = b->ptrWrite;
    while (read < end)
    {
        StreamMsgHeader* msg = (StreamMsgHeader*)read;
        if (msg->id == 0) { break; }
        u8 type = *(read + sizeof(StreamMsgHeader));
        printf("Msg Id: %d, type: %d, size: %d\n", msg->id, type, msg->size);
        read += sizeof(StreamMsgHeader) + msg->size;
    }
    printf("---------------------------------\n");
}

void Stream_PrintTransmissionRecord(TransmissionRecord* rec)
{
    //printf("RECORD: %d MSGS: ", rec->sequence);
    for (u32 i = 0; i < rec->numReliableMessages; ++i)
    {
        printf("%d, ", rec->reliableMessageIds[i]);
    }
    printf("\n");
}

StreamMsgHeader* Stream_FindMessageById(u8* read, u8* end, u32 id)
{
    while (read < end)
    {
        StreamMsgHeader* h = (StreamMsgHeader*)read;
        if (h->id == id)
        {
            return h;
        }
        read += (sizeof(StreamMsgHeader) + h->size);
    }
    return NULL;
}

/**
 * Before:
 * blockStart* (space ---- )(rest of buffer ---- )bufferEnd*
 * After:
 * blockStart* (rest of buffer ---- )bufferEnd*
 */
// 
// Returns new buffer end
u8* Stream_CollapseBlock(u8* blockStart, u32 blockSpace, u8* bufferEnd)
{
    u8* copySrc = blockStart + blockSpace;
    u32 bytesToCopy = bufferEnd - copySrc;
    COM_COPY(copySrc, blockStart, bytesToCopy);
    return blockStart + bytesToCopy;
}

u32 Stream_ClearReceivedOutput(NetStream* stream, u32 ackSequence)
{

    TransmissionRecord* rec =
        Stream_FindTransmissionRecord(stream->transmissions, ackSequence);
    if (!rec) { return 0; }
    ByteBuffer* b = &stream->outputBuffer;
    //u32 currentSize = b->Written();
    u8* read = b->ptrStart;
    u8* end = b->ptrWrite;
    u32 removed = 0;
    for (u32 i = 0; i < rec->numReliableMessages; ++i)
    {
        u32 id = rec->reliableMessageIds[i];
        StreamMsgHeader* h = Stream_FindMessageById(read, end, id);
        if (h == NULL)
        {
            continue;
        }
        // clear and collapse
        //printf("  Delete %d from output\n", id);
        i32 blockSize = sizeof(StreamMsgHeader) + h->size;
        end = Stream_CollapseBlock((u8*)h, blockSize, end);
        b->ptrWrite = end;
        removed += blockSize;
    }
    //printf("  Removed %d bytes. Reduced %d to %d\n",
    //    removed, currentSize, b->Written());
    return removed;
}

void Stream_OutputToPacket(i32 connId, NetStream* s, ByteBuffer* packetBuf)
{
    // Do we even need to send anything reliable?
    if (s->outputBuffer.Written() == 0)
    {
        packetBuf->ptrWrite += COM_WriteU16(0, packetBuf->ptrWrite);
        return;
    }

    // Open a packet
    u32 packetSequence = ZNet_GetNextSequenceNumber(connId);
    TransmissionRecord* rec = Stream_AssignTransmissionRecord(s->transmissions, packetSequence);

    // step over space for header
    // > u16 num reliable bytes written
    // > u32 first reliable sequence
    packetBuf->ptrWrite += NET_SIZE_OF_RELIABLE_HEADER;
    u8* reliableStart = packetBuf->ptrWrite;

    // iterate output buffer
    u8* read = s->outputBuffer.ptrStart;
    u8* end = s->outputBuffer.ptrWrite;

    // set to zero to avoid compiler warning. should be set to the sequence of the 
    // first reliable message, but we've not read it yet
    u32 firstReliableId = 0;

    while (read < end)
    {
        StreamMsgHeader* h = (StreamMsgHeader*)read;
        read += sizeof(StreamMsgHeader);
        //u32 messageId = COM_ReadU32(&read);
        if (packetBuf->Space() < h->size)
        {
            printf("Packet full! Space %d size required %d\n", packetBuf->Space(), h->size);
            Stream_PrintBufferManifest(&s->outputBuffer);
            break;
        }

        if (rec->numReliableMessages == 0) { firstReliableId = h->id; }
        
        u32 offset = h->id - firstReliableId;
        u16 msgHeader = Stream_PackMessageHeader((u8)offset, (u16)h->size);
        packetBuf->ptrWrite += COM_WriteU16(msgHeader, packetBuf->ptrWrite);
        packetBuf->ptrWrite += COM_COPY(read, packetBuf->ptrWrite, h->size);
        rec->reliableMessageIds[rec->numReliableMessages] = h->id;
        rec->numReliableMessages++;
        if (rec->numReliableMessages == MAX_PACKET_TRANSMISSION_MESSAGES)
        {
            printf("Max messages in packet!\n");
            break;
        }
        read += h->size;
    }

    //printf("Sending packet %d. Contents: ", packetSequence);
    //Stream_PrintTransmissionRecord(rec);

    // Step back and write header reliable section header
    u16 reliableBytes = (u16)(packetBuf->ptrWrite - reliableStart);
    u8* write = packetBuf->ptrStart;
    write += COM_WriteU16(reliableBytes, write);
    write += COM_WriteU32(firstReliableId, write);

    // Send!
    //ZNet_SendData(connId, packetBuf->ptrStart, (u16)packetBuf->Written(), 0);

    // return position for next part of packet payload
    //return packetBuf->ptrWrite;
}

// Returns read position after section
u8* Stream_PacketToInput(NetStream* s, u8* ptr)
{
    //printf("Copy reliable packet (%d bytes) to input... ", numBytes);
    // iterate for messages
    // > if messageId <= stream input sequence ignore
    // > if messageId > input sequence, copy to input buffer (if it isn't already there)
    u8* read = ptr;

    // Reliable header
    u16 numReliableBytes = COM_ReadU16(&read);
    if (numReliableBytes == 0)
    {
        return read;
    }
    u32 reliableSequence = COM_ReadU32(&read);
    // Nothing to read
    if (numReliableBytes == 0) { return read; }

    u8* end = read + numReliableBytes;
    while(read < end)
    {
        //u32 messageId = COM_ReadU32(&read);
        u16 packedHeader = COM_ReadU16(&read);
        u8 offset;
        u16 size;
        Stream_UnpackMessageHeader(packedHeader, &offset, &size);
        u32 messageId = reliableSequence + offset;
        //printf("First id %d offset %d size %d\n", reliableSequence, offset, size);

        u8 msgType = *read;
        if (msgType == 0)
        {
            printf("ABORTED input copy: read msgType 0!\n");
            break;
        }
        // message might be out of date...
        if (messageId <= s->inputSequence)
        {
            read += size;
            //printf("  MessageId %d is out of date\n", messageId);
            continue;
        }
        // message might have already been received
        else if (Stream_FindMessageById(
            s->inputBuffer.ptrStart,
            s->inputBuffer.ptrWrite,
            messageId))
        {
            read += size;
            continue;
        }
        else
        {
            //printf("\n INPUT Id: %d type %d size: %d\n", messageId, msgType, size);
            u8* msg = read;
            Buf_WriteMessage(&s->inputBuffer, messageId, msg, size);
            read += size;
        }
    }
	return read;
    //printf("Done\n");
}
