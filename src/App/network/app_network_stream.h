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
#include "../app_module.cpp"


void Buf_WriteMessage(ByteBuffer* b, u32 msgId, u8* bytes, u32 numBytes)
{
    Assert(b->Space() > numBytes)
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

u32 Stream_ClearReceivedMessages(TransmissionRecord* rec, ByteBuffer* b, u32* ackSequence)
{
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

        // update ack if necessary
        if (h->id > *ackSequence)
        {
            *ackSequence = h->id;
        }
    }
    //printf("  Removed %d bytes. Reduced %d to %d\n",
    //    removed, currentSize, b->Written());
    return removed;
}

void Stream_OutputToPacket(i32 connId, NetStream* s, u8* packet, u16 numBytes)
{
    // Do we even need to send anything reliable?
    if (s->outputBuffer.Written() == 0) { return; }

    // Open a packet
    u32 packetSequence = ZNet_GetNextSequenceNumber(connId);
    TransmissionRecord* rec = Stream_AssignTransmissionRecord(s->transmissions, packetSequence);

    // sanitise packet
    COM_ZeroMemory(packet, numBytes);
    ByteBuffer b = Buf_FromBytes(packet, numBytes);
    // step over header
    // > u16 num reliable bytes
    // > u16 num unreliable bytes
    // > u32 first reliable sequence
    b.ptrWrite += sizeof(u16) + sizeof(u16) + sizeof(u32);
    u8* reliableStart = b.ptrWrite;

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
        if (b.Space() < h->size)
        {
            printf("Packet full! Space %d size required %d\n", b.Space(), h->size);
            Stream_PrintBufferManifest(&s->outputBuffer);
            break;
        }

        if (rec->numReliableMessages == 0) { firstReliableId = h->id; }
        
        u32 offset = h->id - firstReliableId;
        u16 msgHeader = Stream_PackMessageHeader((u8)offset, (u16)h->size);
        b.ptrWrite += COM_WriteU16(msgHeader, b.ptrWrite);
        b.ptrWrite += COM_COPY(read, b.ptrWrite, h->size);
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

    // Step back and write header
    u16 reliableBytes = (u16)(b.ptrWrite - reliableStart);
    u8* write = b.ptrStart;
    write += COM_WriteU16(reliableBytes, write);
    write += COM_WriteU16(0, write);
    write += COM_WriteU32(firstReliableId, write);

    // Send!
    ZNet_SendData(connId, b.ptrStart, (u16)b.Written(), 0);
}

void Stream_PacketToInput(NetStream* s, u8* ptr, u16 numBytes)
{
    //printf("Copy reliable packet (%d bytes) to input... ", numBytes);
    // iterate for messages
    // > if messageId <= stream input sequence ignore
    // > if messageId > input sequence, copy to input buffer
    u8* read = ptr;
    u8* end = read + numBytes;
    u32 reliableSequence = COM_ReadU32(&read);
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
    //printf("Done\n");
}
