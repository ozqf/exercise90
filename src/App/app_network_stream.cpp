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

#include "app_module.h"

i32 Stream_WriteStreamMsgHeader(
    u8* ptr, u32 msgId, i32 numBytes, f32 resendRateSeconds);

// Returns 0 on successful copy
i32 Stream_BufferMessage(ByteBuffer* b, u32 msgId, u8* bytes, i32 numBytes)
{
	COM_ASSERT(b->ptrWrite, "Buf write message destination is null")
    if (b->Space() < numBytes)
    {
        return COM_ERROR_NO_SPACE;
    }
    //StreamMsgHeader h;
    //h.id = msgId;
    //h.size = numBytes;
    //b->ptrWrite += COM_COPY(&h, b->ptrWrite, sizeof(StreamMsgHeader));

    b->ptrWrite += Stream_WriteStreamMsgHeader(
        b->ptrWrite, msgId, numBytes, NET_DEFAULT_RESEND_RATE);
    b->ptrWrite += COM_COPY(bytes, b->ptrWrite, numBytes);
    return COM_ERROR_NONE;
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
    // for debugging buffers:
	h->sentinel[0] = 'M';
	h->sentinel[1] = 'S';
	h->sentinel[2] = 'G';
	h->sentinel[3] = '\0';
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
            reliableRead += size;
        }
	}
    u8* unreliableRead;
    if (numReliableBytes == 0)
    {
        unreliableRead = bytes + sizeof(u16);
    }
    else
    {
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
    for (u32 i = 0; i < rec->numReliableMessages; ++i)
    {
        printf("%d, ", rec->reliableMessageIds[i]);
    }
    printf("\n");
}

// Linear search through buffer
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
	if (bytesToCopy == 0)
	{
		// nothing remains in this buffer. Just zero it out
		COM_ZeroMemory(blockStart, blockSpace);
	}
	else
	{
		COM_ASSERT(bytesToCopy > 0, "Collapse block - bytes < 0");
		// copy the memory remaining over the block
		COM_COPY(copySrc, blockStart, bytesToCopy);
		i32 diff = blockSpace - bytesToCopy;
		if (diff > 0)
		{
			// not enough bytes were copied back to fully write over the
			// remaining space so sanitise it
			COM_ZeroMemory((blockStart + bytesToCopy), diff);
		}
	}
    return blockStart + bytesToCopy;
}

u32 Stream_ClearReceivedOutput(NetStream* stream, u32 packetSequence)
{

    TransmissionRecord* rec =
        Stream_FindTransmissionRecord(stream->transmissions, packetSequence);
    if (!rec) { return 0; }
    if (rec->numReliableMessages == 0) { return 0; }
    ByteBuffer* b = &stream->outputBuffer;
    u8* read = b->ptrStart;
    //u8* end = b->ptrWrite;
    u32 removed = 0;
    for (u32 i = 0; i < rec->numReliableMessages; ++i)
    {
		// Drop out if Buffer empty
		if (b->Written() == 0) { break; }
        u32 id = rec->reliableMessageIds[i];
        StreamMsgHeader* h = Stream_FindMessageById(read, b->ptrWrite, id);
        if (h == NULL)
        {
            continue;
        }
		COM_ASSERT(h->size > 0, "Clear Buffer found Command size <= 0");
		// ACKING CANNOT BE DONE HERE!
		// This merely tells you what the highest sequence number received is... NOT
		// the highest executed. This allows for gaps!
		
        // Set ack if required
        if (stream->ackSequence < id)
        {
            stream->ackSequence = id;
        }
        // clear and collapse
        i32 blockSize = sizeof(StreamMsgHeader) + h->size;
		b->ptrWrite = Stream_CollapseBlock((u8*)h, blockSize, b->ptrWrite);
        //Stream_PrintBufferManifest(b);
        removed += blockSize;
    }
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
		//packetBuf->ptrWrite += COM_WriteU32(s->inputSequence, packetBuf->ptrWrite);
        packetBuf->ptrWrite += COM_WriteU16(0, packetBuf->ptrWrite);
        return;
    }

    // Open a packet
    u32 packetSequence = ZNet_GetNextSequenceNumber(connId);
    TransmissionRecord* rec = Stream_AssignTransmissionRecord(s->transmissions, packetSequence);

    // step over space for header (assuming it will be written)
	// > u32 input sequence (for moving remote sequence forward
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
        #if 0
        if (h->resendTime > 0)
        {
            h->resendTime -= deltaTime;
			read += h->size;
            continue;
        }
        #endif

        // Ready to send (or resend)
        h->resendTime = h->resendMax;
        i32 msgSize = h->size;
        i32 space = packetBuf->Space();
        if (space < 0)
        {
            Stream_PrintBufferManifest(&s->outputBuffer);
            COM_ASSERT(0, "ERROR Packet Space < 0");
        }
        if (msgSize < 0)
        {
            Stream_PrintBufferManifest(&s->outputBuffer);
            COM_ASSERT(0, "Msg size is < 0")
        }
        if (space < msgSize)
        {
            printf("-");
            // printf("Packet full! Space %d size required %d (Wrote %d so far)\n",
            //     packetBuf->Space(),
            //     h->size,
            //     rec->numReliableMessages);
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
#if 1
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
        u16 packedHeader = COM_ReadU16(&read);
        u8 offset;
        u16 size;
        Stream_ReadPacketHeader(packedHeader, &offset, &size);
        u32 messageId = reliableSequence + offset;
        
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
            u8* msg = read;
            i32 err = Stream_BufferMessage(&s->inputBuffer, messageId, msg, size);
            if (err == COM_ERROR_NO_SPACE)
            {
                COM_ASSERT(0, "Stream Input buffer is full");
                return read;
            }
            read += size;
        }
    }
	return read;
}
#endif
