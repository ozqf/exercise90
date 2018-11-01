#pragma once

#include "test_network_win32.h"

/**
 * Select and overwrite a previous transmission record
 */
TransmissionRecord* Stream_AssignTransmissionRecord(TransmissionRecord* records, u32 sequence)
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
TransmissionRecord* Stream_FindTransmissionRecord(TransmissionRecord* records, u32 sequence)
{
	TransmissionRecord* result = &records[sequence % MAX_TRANSMISSION_RECORDS];
	if (result->sequence == sequence)
	{
		return result;
	}
	return NULL;
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

// dest* --- space |---- rest of buffer ----| bufferEnd*
// Returns new buffer end
u8* Stream_CollapseBlock(u8* blockStart, u32 blockSpace, u8* bufferEnd)
{
    u8* copySrc = blockStart + blockSpace;
    u32 bytesToCopy = bufferEnd - copySrc;
    COM_COPY(copySrc, blockStart, bytesToCopy);
    return blockStart + bytesToCopy;
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

void Stream_CopyReliablePacketToInput(ReliableStream* s, u8* ptr, u16 numBytes)
{
    //printf("Copy reliable packet (%d bytes) to input... ", numBytes);
    // iterate for messages
    // > if messageId <= streamput in sequence ignore
    // > if messageId > input sequence, copy to input buffer
    u8* read = ptr;
    u8* end = read + numBytes;
    while(read < end)
    {
        u32 messageId = COM_ReadU32(&read);

        u8 msgType = *read;
        if (msgType == 0)
        {
            printf("ABORTED input copy: read msgType 0!\n");
            break;
        }
        //u32 message = COM_ReadU32(&read);
        // Must get msg size regardless of whether command will be read
        // as it must be skipped over in either case
        u16 size = Msg_MeasureForReading(msgType, read);
        if (messageId <= s->inputSequence)
        {
            read += size;
            continue;
        }
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
            // step back to include type byte
            //printf("\n INPUT Id: %d type %d size: %d\n", messageId, msgType, size);
            u8* msg = read;
            Buf_WriteMessage(&s->inputBuffer, messageId, msg, size);
            read += size;
        }
    }
    //printf("Done\n");
}














/**
 * Iterate over pending input. Load into application update buffer
 * messages next in sequence
 * Returns bytes removed
 */
u32 Stream_ReadInput(u8* read, u8* end, u32 currentSequence, ByteBuffer* b)
{
    //printf("Reading %d bytes of input from sequence %d\n", (end - read), currentSequence);
    i32 reading = 1;
    i32 removed = 0;
    for (;;)
    {
        u32 nextSequence = currentSequence + 1;
        StreamMsgHeader* h = Stream_FindMessageById(read, end, nextSequence);
        if (h == NULL)
        {
            break;
        }
        currentSequence = nextSequence;
        printf("Exec msg %d\n", currentSequence);

        // Collapse buffer
        i32 blockSize = (sizeof(StreamMsgHeader) + h->size);
        end = Stream_CollapseBlock((u8*)h, blockSize, end);
        removed += blockSize;
    }
    return removed;
}

u32 Stream_ClearReceivedMessages(TransmissionRecord* rec, ByteBuffer* b)
{
    u32 currentSize = b->Written();
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
        printf("  Delete %d from output\n", id);
        i32 blockSize = sizeof(StreamMsgHeader) + h->size;
        end = Stream_CollapseBlock((u8*)h, blockSize, end);
        b->ptrWrite = end;
        removed += blockSize;
    }
    printf("  Removed %d bytes. Reduced %d to %d\n", removed, currentSize, b->Written());
    return removed;
}

void Stream_RunTests()
{
    printf("\n=== RELIABLE STREAM TESTS ===\n");
    /*
	1> Adding to stream buffer (input or output)
	2> stream buffer to packet (output)
		> Copying from buffer to packet
		> Creating Transmission Record of messages in packet
	3> packet to stream buffer (input)
		> Checking that message is not out of date or already in the buffer
		> Copying from packet to buffer
	4> Parsing Input
		> Collapse buffer after a message is executed.
	5> On packet acked
		> Iterate Output, removing acked messages and collapsing the buffer

    Buffer layout:
    StreamMsgHeader
        (u32) Sequence
        (u32) Size (omitted in packet)
    Message
        (u8) Type
        (...) data
    
	*/

    // Prep for tests
	static const i32 BUFFER_SIZE = 1024;
    
	u8 inputBytes[BUFFER_SIZE];
    u8 outputBytes[BUFFER_SIZE];
    u8 packetBytes[BUFFER_SIZE];
    u8 gameBytes[BUFFER_SIZE];

    ByteBuffer input = Buf_FromBytes(inputBytes, BUFFER_SIZE);
    ByteBuffer output = Buf_FromBytes(outputBytes, BUFFER_SIZE);
    ByteBuffer packet = Buf_FromBytes(packetBytes, BUFFER_SIZE);
    ByteBuffer gameBuffer = Buf_FromBytes(gameBytes, BUFFER_SIZE);

    u32 outgoingSequence = 200;

    //////////////////////////////////////////////////////////////////////////
    printf("\n------ 1: Copy to pending output ------\n");
    //////////////////////////////////////////////////////////////////////////
    TestMsg1 m1 = {};
	m1.member1 = 256;
	m1.member2 = 512;
	m1.member3 = 768;
    TestMsg2 m2 = {};
	m2.member1 = 256;
	m2.member2 = 512;
	m2.pos[0] = 768;
	m2.pos[0] = 1024;
	m2.pos[0] = 2048;
    //TestMsg1 m3 = {};
    printf("Measure msg sizes:\n");
    printf("  Type1: %d bytes\n", m1.MeasureForWriting());
    printf("  Type2: %d bytes\n", m2.MeasureForWriting());
    
    // Messages know how many bytes they will use.
    // Number of bytes used is recorded in input/output buffers to allow

    Buf_WriteStreamMsgHeader(&output, 201, m1.MeasureForWriting());
    output.ptrWrite += m1.Write(output.ptrWrite);
    Buf_WriteStreamMsgHeader(&output, 200, m2.MeasureForWriting());
    output.ptrWrite += m2.Write(output.ptrWrite);
    Buf_WriteStreamMsgHeader(&output, 202, m2.MeasureForWriting());
    output.ptrWrite += m2.Write(output.ptrWrite);
    Buf_WriteStreamMsgHeader(&output, 204, m1.MeasureForWriting());
    output.ptrWrite += m1.Write(output.ptrWrite);

    printf("  output done. Wrote %d bytes\n", output.Written());
    PARSE_MSG_BUFFER(&output.ptrStart, &output.ptrWrite, Buf_InspectionCallback);
    
    // 8 + 8 + 13 + 21

    //////////////////////////////////////////////////////////////////////////
    printf("\n------ 2: Copy to packet ------\n");
    //////////////////////////////////////////////////////////////////////////
    // step over packet header
    packet.ptrWrite += sizeof(u16) * 2;
    u8* reliableStart = packet.ptrWrite;

    TransmissionRecord rec = {};
    rec.sequence = 127;

    u8* read = output.ptrStart;
    u8* end = output.ptrWrite;
    while (read < end)
    {
        StreamMsgHeader h;
        read += h.Read(read);
        printf("  writing %d (%d bytes)\n", h.id, h.size);
        packet.ptrWrite += COM_WriteU32(h.id, packet.ptrWrite);
        // This is a raw bytes copy. It should really be the point where
        // the message is Packed!
        packet.ptrWrite += COM_COPY(read, packet.ptrWrite, h.size);
        rec.reliableMessageIds[rec.numReliableMessages] = h.id;
        rec.numReliableMessages++;
        read += h.size;
    }

    u16 reliableBytes = (u16)(packet.ptrWrite - reliableStart);
    printf("Wrote %d reliable bytes\n", reliableBytes);
    // Patch in packet header
    read = packet.ptrStart;
    read += COM_WriteU16(reliableBytes, read);
    COM_WriteU16(0, read);

    // Inspect transmission record:
    printf("Transmission, sequence %d, %d messages: ", rec.sequence, rec.numReliableMessages);
    for (u32 i = 0; i < rec.numReliableMessages; ++i)
    {
        printf("%d, ", rec.reliableMessageIds[i]);
    }
    printf("\n");

    // parse packet
    printf("Parse Packet\n");
    read = packet.ptrStart;
    reliableBytes = COM_ReadU16(&read);
    u16 unreliableBytes = COM_ReadU16(&read);
    printf("Read %d reliable bytes and %d unreliable bytes\n", reliableBytes, unreliableBytes);
    end = read + reliableBytes;
    while (read < end)
    {
        u32 sequence = COM_ReadU32(&read);
		printf("Reading msg sequence %d\n", sequence);
        u8 type = *read;
        printf("  Peeked type %d\n", type);
        switch (type)
        {
            case TEST_MSG_TYPE_1:
            {
                TestMsg1 msg;
                u8* msgStart = read;
                read += msg.Read(read);
                printf("Read type %d, %d bytes\n",
                    type, (read - msgStart));
            } break;

            case TEST_MSG_TYPE_2:
            {
                TestMsg2 msg;
                u8* msgStart = read;
                read += msg.Read(read);
                printf("Read type %d, %d bytes\n",
                    type, (read - msgStart));
            } break;

            default:
            {
                printf("UNKNOWN MSG TYPE %d\n", type);
                return;
            } break;
        }
    }
    printf("  Done\n");

    //////////////////////////////////////////////////////////////////////////
    printf("\n------ 3: Packet to Input Buffer ------\n");
    //////////////////////////////////////////////////////////////////////////
    read = packet.ptrStart;
    reliableBytes = COM_ReadU16(&read);
    unreliableBytes = COM_ReadU16(&read);
    end = read + reliableBytes;
    while (read < end)
    {
        u32 sequence = COM_ReadU32(&read);
        u8 type = *read;
        u16 bytesRead;
        printf("Read sequence %d type %d\n", sequence, type);
        switch (type)
        {
            case TEST_MSG_TYPE_1:
            {
                TestMsg1 msg;
                // Reading... ideally would just measure here
                bytesRead = msg.Read(read);
                input.ptrWrite += COM_WriteU32(sequence, input.ptrWrite);
                input.ptrWrite += COM_WriteU32(bytesRead, input.ptrWrite);
                input.ptrWrite += COM_COPY(read, input.ptrWrite, bytesRead);
                read += bytesRead;
            } break;

            case TEST_MSG_TYPE_2:
            {
                TestMsg2 msg;
                bytesRead = msg.Read(read);
                input.ptrWrite += COM_WriteU32(sequence, input.ptrWrite);
                input.ptrWrite += COM_WriteU32(bytesRead, input.ptrWrite);
                input.ptrWrite += COM_COPY(read, input.ptrWrite, bytesRead);
                read += bytesRead;
            } break;

            default:
            {
                printf("UNKNOWN MSG TYPE %d\n", type);
                return;
            } break;
        }
    }
    printf("Reading Input buffer:\n");
    PARSE_MSG_BUFFER(&input.ptrStart, &input.ptrWrite, Buf_InspectionCallback);

    //////////////////////////////////////////////////////////////////////////
    printf("\n------ 4: Parse Input ------\n");
    //////////////////////////////////////////////////////////////////////////
    read = input.ptrStart;
    end = input.ptrWrite;
    input.ptrWrite -= Stream_ReadInput(read, end, 199, &gameBuffer);
    printf("Contents of input:\n");
    PARSE_MSG_BUFFER(&input.ptrStart, &input.ptrWrite, Buf_InspectionCallback);

    //////////////////////////////////////////////////////////////////////////
    printf("\n------ 5: Clear Output ------\n");
    //////////////////////////////////////////////////////////////////////////
    output.ptrWrite -= Stream_ClearReceivedMessages(&rec, &output);
    PARSE_MSG_BUFFER(&output.ptrStart, &output.ptrWrite, Buf_InspectionCallback);
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

void TNet_TestMsgHeaderPacking(u8 offset, u16 size)
{
    // sequence offset 6 bits, so 0 to 64
    //u8 offset = 29;
    // size, 10 bits, 0 to 1024
    //u16 size = 576;

    offset = offset & SIX_BIT_MASK;
    size = size & TEN_BIT_MASK;
    u16 packed = (offset << 10) | size;
    printf("Packing offset %d and size %d to: %d\n", offset, size, packed);

    u8 offset2 = (packed >> 10) & SIX_BIT_MASK;
    u16 size2 = packed & TEN_BIT_MASK;
    printf("Unpacked: offset %d and size %d\n", offset2, size2);
}

void TNet_TestReliability()
{
	//Test_BufferWriteAndCollapse();
    //Stream_RunTests();
    TNet_TestMsgHeaderPacking(29, 576);
    TNet_TestMsgHeaderPacking(56, 184);
    TNet_TestMsgHeaderPacking(127, 1023);
}
