#pragma once

#include "test_network_win32.h"

#define MAX_PACKET_TRANSMISSION_MESSAGES 32
struct PacketTransmissionRecord
{
	u32 sequence;
	u32 numReliableMessages;
	u32 reliableMessageIds[MAX_PACKET_TRANSMISSION_MESSAGES];
};

#define MAX_TRANSMISSION_RECORDS 33 
PacketTransmissionRecord g_transmissions[MAX_TRANSMISSION_RECORDS];

PacketTransmissionRecord* Stream_FindTransmissionRecord(PacketTransmissionRecord* records, u32 sequence)
{
	PacketTransmissionRecord* result = &records[sequence % MAX_TRANSMISSION_RECORDS];
	if (result->sequence == sequence)
	{
		result->sequence = 0;
		return result;
	}
	return NULL;
}

void Stream_CopyReliablePacketToInput(ReliableStream* s, u8* ptr, u16 numBytes)
{
    // iterate for messages
    // > if messageId <= stream input sequence ignore
    // > if messageId > input sequence, copy to input buffer
    u8* read = ptr;
    u8* end = read + numBytes;
    while(read < end)
    {
        u32 messageId = COM_ReadU32(&read);
        u8 msgType = COM_ReadByte(&read);
        //u32 message = COM_ReadU32(&read);
        // Must get msg size regardless of whether command will be read
        // as it must be skipped over in either case
        u16 size = Net_MeasureMessageBytes(msgType, read);
        printf(" Id: %d type %d size: %d\n", messageId, msgType, size);
        if (messageId <= s->inputSequence)
        {
            read += size;
            continue;
        }
        else
        {
            // step back to include type byte
            u8* msg = read - 1;
            Buf_WriteMessage(&s->inputBuffer, messageId, msg, size);
            read += size;
        }
    }
}

void Stream_RunTests()
{
    printf("=== RELIABLE STREAM TESTS ===\n");
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
    MessageHeader
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

    ByteBuffer input = Buf_FromBytes(inputBytes, BUFFER_SIZE);
    ByteBuffer output = Buf_FromBytes(outputBytes, BUFFER_SIZE);
    ByteBuffer packet = Buf_FromBytes(packetBytes, BUFFER_SIZE);

    u32 outgoingSequence = 200;

    // 1
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
    printf("  Type1: %d bytes\n", m1.Measure());
    printf("  Type2: %d bytes\n", m2.Measure());
    
    // Messages know how many bytes they will use.
    // Number of bytes used is recorded in input/output buffers to allow

    Buf_WriteMessageHeader(&output, outgoingSequence++, m1.Measure());
    output.ptrWrite += m1.Write(output.ptrWrite);
    Buf_WriteMessageHeader(&output, outgoingSequence++, m2.Measure());
    output.ptrWrite += m2.Write(output.ptrWrite);
    printf("  output done. Wrote %d bytes\n", output.Written());
    PARSE_BUFFER(&output.ptrStart, &output.ptrWrite, Buf_InspectionCallback);
    
    // 8 + 8 + 13 + 21

    // 2 load into packet
    printf("2: Copy to packet\n");
    // step over packet header
    packet.ptrWrite += sizeof(u16) * 2;
    u8* reliableStart = packet.ptrWrite;

    u8* read = output.ptrStart;
    u8* end = output.ptrWrite;
    while (read < end)
    {
        MessageHeader h;
        read += h.Read(read);
        printf("  writing %d (%d bytes)\n", h.id, h.size);
        packet.ptrWrite += COM_WriteU32(h.id, packet.ptrWrite);
        packet.ptrWrite += COM_COPY(read, packet.ptrWrite, h.size);
        read += h.size;
    }
    u16 reliableBytes = (u16)(packet.ptrWrite - reliableStart);
    printf("Wrote %d reliable bytes\n", reliableBytes);
    read = packet.ptrStart;
    read += COM_WriteU16(reliableBytes, read);
    COM_WriteU16(0, read);

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
            } break;
        }
    }
}

void TNet_TestReliability()
{
	Test_BufferWriteAndCollapse();
    Stream_RunTests();
}
