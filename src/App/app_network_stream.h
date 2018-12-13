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


void Buf_WriteMessage(ByteBuffer* b, u32 msgId, u8* bytes, i32 numBytes)
{
	APP_ASSERT(b->ptrWrite, "Buf write message destination is null")
    APP_ASSERT(b->Space() > numBytes, "Buf write message has no space left")
    StreamMsgHeader h;
    h.id = msgId;
    h.size = numBytes;
    b->ptrWrite += COM_COPY(&h, b->ptrWrite, sizeof(StreamMsgHeader));
    b->ptrWrite += COM_COPY(bytes, b->ptrWrite, numBytes);
}

// Does NOT free I/O buffers
#if 1
internal void Stream_Clear(NetStream* stream)
{
    // careful not to change buffer pointers if they are set
    ByteBuffer input = stream->inputBuffer;
    ByteBuffer output = stream->outputBuffer;
    COM_SET_ZERO(stream, sizeof(NetStream));
    stream->inputBuffer = input;
    stream->outputBuffer = output;
}
#endif

i32 Stream_WriteStreamMsgHeader(u8* ptr, u32 msgId, i32 numBytes, f32 resendRateSeconds)
{
    u8* start = ptr;
    StreamMsgHeader* h = (StreamMsgHeader*)ptr;
    h->id = msgId;
    h->size = numBytes;
    // Write zero so msg is sent immediately when first discovered
    h->resendTime = 0;
    h->resendMax = resendRateSeconds;
    return sizeof(StreamMsgHeader);
}
/*
u32 Stream_ReadStreamMsgHeader(u8* ptr, StreamMsgHeader* result)
{
    u8* start = ptr;
    result->id = COM_ReadU32(&ptr);
    result->size = COM_ReadU32(&ptr);
    result->resendTime = COM_ReadF32(&ptr);
    result->resendMax = COM_ReadF32(&ptr);
    //&result->id, 
    //ptr += COM_WriteU32(msgId, ptr);
    //ptr += COM_WriteU32(numBytes, ptr);
    return (ptr - start);
}
*/
// void Buf_WriteStreamMsgHeader(ByteBuffer* b, u32 msgId, u32 numBytes)
// {
//     Assert(b->Space() > numBytes)
//     b->ptrWrite += Stream_WriteStreamMsgHeader(b->ptrWrite, msgId, numBytes);
// }

inline u16 Stream_WritePacketHeader(u8 sequenceOffset, u16 size)
{
    sequenceOffset = sequenceOffset & SIX_BIT_MASK;
    size = size & TEN_BIT_MASK;
    return ((sequenceOffset << 10) | size);
}

inline void Stream_ReadPacketHeader(u16 header, u8* sequenceOffset, u16* size)
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

void Stream_PrintPacketManifest(u8* bytes, u16 numBytes)
{
	printf("=== PACKET MANIFEST (%d bytes)===\n", numBytes);
    u8* reliableRead = bytes;
	u16 numReliableBytes = COM_ReadU16(&reliableRead);
    u8* end;
	printf("%d reliable Bytes\n", numReliableBytes);
	if (numReliableBytes != 0)
	{
        u32 reliableSequence = COM_ReadU32(&reliableRead);
        printf("Reliable sequence: %d\n", reliableSequence);
        
        end = reliableRead + numReliableBytes;
        while (reliableRead < end)
        {
            u16 header = COM_ReadU16(&reliableRead);
            u8 sequenceOffset = 0;
            u16 size = 0;
		    Stream_ReadPacketHeader(header, &sequenceOffset, &size);
            u8 type = *reliableRead;
            printf("Type %d, seq offset %d, size %d\n",
                type, sequenceOffset, size);
            if (size == 0)
            {
                printf("  Corrupted Manifest: cmd size is zero\n");
                break;
            }
			//APP_ASSERT(size > 0, "Packet Cmd size is 0!");
            reliableRead += size;
        }
	}
    // else
    // {
    //     unreliableRead = reliableRead;
    // }
    u8* unreliableRead;
    if (numReliableBytes == 0)
    {
        unreliableRead = bytes + sizeof(u16);
    }
    else
    {
        // u16 == reliable bytes, i32 == reliable sequence
        unreliableRead = bytes + (sizeof(u16) + sizeof(i32) + numReliableBytes);
    }

    u32 sync = COM_ReadI32(&unreliableRead);
    printf("Deserialise check: 0x%X vs 0x%X\n", sync, NET_DESERIALISE_CHECK);
    if (sync != NET_DESERIALISE_CHECK)
    {
        printf("  Check failed. Aborting manifest read\n");
        return;
    }
    u16 unreliableBytes = COM_ReadU16(&unreliableRead);
    printf("Unreliable bytes %d\n", unreliableBytes);
    if (unreliableBytes == 0) { return; }

    end = unreliableRead + unreliableBytes;
    while (unreliableRead < end)
    {
        u16 header = COM_ReadU16(&unreliableRead);
        u8 sequenceOffset = 0;
        u16 size = 0;
	    Stream_ReadPacketHeader(header, &sequenceOffset, &size);
        u8 type = *unreliableRead;
        printf("Type %d, seq offset %d, size %d\n",
            type, sequenceOffset, size);
		if (size == 0)
		{
			printf("  Corrupted Manifest: cmd size is zero\n");
			break;
		}
		//APP_ASSERT(size > 0, "Packet Cmd size is 0!");
        unreliableRead += size;
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
	i32 claimedWritten = b->Written();
    while (read < end)
    {
        StreamMsgHeader* msg = (StreamMsgHeader*)read;
		if (msg->id == 0) { printf("Read msg id 0, abort read\n"); break; }
        u8 type = *(read + sizeof(StreamMsgHeader));
        printf("Msg Id: %d, type: %d, size: %d\n", msg->id, type, msg->size);
        read += sizeof(StreamMsgHeader) + msg->size;
    }
	i32 bytesRead = read - b->ptrStart;
	printf("  Claimed bytes: %d Actual bytes: %d\n", claimedWritten, bytesRead);
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
    i32 bytesToCopy = bufferEnd - copySrc;
	APP_ASSERT(bytesToCopy > 0, "Collapse block size is <= 0");
    COM_COPY(copySrc, blockStart, bytesToCopy);
    return blockStart + bytesToCopy;
}

u32 Stream_ClearReceivedOutput(NetStream* stream, u32 packetSequence)
{

    TransmissionRecord* rec =
        Stream_FindTransmissionRecord(stream->transmissions, packetSequence);
    if (!rec) { return 0; }
    if (rec->numReliableMessages == 0) { return 0; }
    printf("STREAM Confirming delivery of packet %d\n", packetSequence);
    ByteBuffer* b = &stream->outputBuffer;
    Stream_PrintTransmissionRecord(rec);
    Stream_PrintBufferManifest(b);

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
        // Set ack if required
        if (stream->ackSequence < id)
        {
            stream->ackSequence = id;
        }
        // clear and collapse
        i32 blockSize = sizeof(StreamMsgHeader) + h->size;
		printf("  Delete %d from output. Blocksize %d cmd size %d Buffer space %d\n",
			id, blockSize, h->size, b->Space());
        end = Stream_CollapseBlock((u8*)h, blockSize, end);
        b->ptrWrite = end;
        removed += blockSize;
    }
    //printf("  Removed %d bytes. Reduced %d to %d\n",
    //    removed, currentSize, b->Written());
    return removed;
}

/////////////////////////////////////////////////////////////////
// Stream reliable output buffer --> packet
// Returns read position after section
/////////////////////////////////////////////////////////////////
void Stream_OutputToPacket(i32 connId, NetStream* s, ByteBuffer* packetBuf, f32 deltaTime)
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

    // step over space for header (assuming it will be written)
    // > u16 num reliable bytes written
    // > u32 first reliable sequence
	u8* reliableBlockStart = packetBuf->ptrWrite;
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
        if (h->resendTime > 0)
        {
            h->resendTime -= deltaTime;
			read += h->size;
            continue;
        }

        // Ready to send (or resend)
        h->resendTime = h->resendMax;
        i32 msgSize = h->size;
        i32 space = packetBuf->Space();
        if (space < 0)
        {
            Stream_PrintBufferManifest(&s->outputBuffer);
            APP_ASSERT(0, "ERROR Packet Space < 0");
        }
        if (msgSize < 0)
        {
            Stream_PrintBufferManifest(&s->outputBuffer);
            APP_ASSERT(0, "Msg size is < 0")
        }
        if (space < msgSize)
        {
            printf("Packet full! Space %d size required %d (Wrote %d so far)\n",
                packetBuf->Space(),
                h->size,
                rec->numReliableMessages);
            break;
        }

        if (rec->numReliableMessages == 0) { firstReliableId = h->id; }
        
        u32 offset = h->id - firstReliableId;
        u16 msgHeader = Stream_WritePacketHeader((u8)offset, (u16)h->size);
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
	// Due to resend delays, we may have a buffer full of messages but
	// none were actually written
	if (rec->numReliableMessages == 0)
	{
		packetBuf->ptrWrite = reliableBlockStart;
		packetBuf->ptrWrite += COM_WriteU16(0, packetBuf->ptrWrite);
		//write += COM_WriteU16(reliableBytes, write);
		return;
	}

    // Step back and write header reliable section header
    u16 reliableBytes = (u16)(packetBuf->ptrWrite - reliableStart);
    u8* write = packetBuf->ptrStart;
	write += COM_WriteU16(reliableBytes, write);
	write += COM_WriteU32(firstReliableId, write);
}

/////////////////////////////////////////////////////////////////
// Packet --> Stream reliable buffer
// Returns read position after section
/////////////////////////////////////////////////////////////////
u8* Stream_PacketToInput(NetStream* s, u8* ptr)
{
    // iterate for messages
    // > if messageId <= stream input sequence ignore
    // > if messageId > input sequence, copy to input buffer (if it isn't already there)
    u8* read = ptr;

    // Reliable header
    u16 numReliableBytes = COM_ReadU16(&read);
    if (numReliableBytes == 0) { return read; }
    u32 reliableSequence = COM_ReadU32(&read);
    if (numReliableBytes == 0) { return read; }

    u8* end = read + numReliableBytes;
    while(read < end)
    {
        //u32 messageId = COM_ReadU32(&read);
        u16 packedHeader = COM_ReadU16(&read);
        u8 offset;
        u16 size;
        Stream_ReadPacketHeader(packedHeader, &offset, &size);
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
            //printf("  MessageId %d is out of date (vs %d)\n", messageId, s->inputSequence);
            continue;
        }
        // message might have already been received
        else if (Stream_FindMessageById(
            s->inputBuffer.ptrStart,
            s->inputBuffer.ptrWrite,
            messageId))
        {
            //printf("  MessageId %d already received\n", messageId);
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
}
