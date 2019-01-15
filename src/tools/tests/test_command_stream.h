#pragma once

#include "../../common/com_module.h"
#include "../../app/commands.h"
#include "../../app/stream.h"
#include "../../app/packet.h"

#define CMD_TYPE_TEST 1
#define CMD_TYPE_TEST_2 2

struct TestCommand1
{
    Command header;
    i32 data1;
    i32 data2;
};

struct TestCommand2
{
    Command header;
    i32 id;
    u16 pos[3];
    u16 pitch;
    u16 yaw;
};

void PrintCommand(Command* header)
{
    printf("Read Cmd type %d. sequence %d, tick %d\n",
		header->type, header->sequence, header->tick);
    switch (header->type)
    {
        case CMD_TYPE_TEST:
        {
            TestCommand1* cmd = (TestCommand1*)header;
            printf(" Cmd data1: %d, data2: %d\n", cmd->data1, cmd->data2);
        } break;
    }
}

void PrintCommandBuffer(u8* ptr, i32 numBytes)
{
	printf("\n=== CMD BUFFER (%d bytes) ===\n", numBytes);
    u8* read = ptr;
    u8* end = ptr + numBytes;

    while(read < end)
    {
        Command* header = (Command*)read;
        Assert(Cmd_Validate(header) == COM_ERROR_NONE);
        read += header->size;
        PrintCommand(header);
    }
	printf("  Ptr diff check: %d\n", (read - end));
}

void TestStreamToPacket(NetStream* s, u8* packet, i32 packetBytes)
{
    i32 packetSequence = 1; // IRL: acquired from znet!;
    TransmissionRecord* rec = Stream_AssignTransmissionRecord(s->transmissions, packetSequence);
}

void AllocStream(NetStream* stream, i32 size)
{
	*stream = {};
	
    stream->inputBuffer = Buf_FromMalloc(malloc(size), size);
    Buf_Clear(&stream->inputBuffer);
    stream->outputBuffer = Buf_FromMalloc(malloc(size), size);
    Buf_Clear(&stream->outputBuffer);
}

void TestCommandStream()
{
    printf("=== Test command streams ===\n");

    // init buffers
	i32 size = KiloBytes(64);
	
	NetStream reliableStream;
	AllocStream(&reliableStream, size);
	
	NetStream unreliableStream;
	AllocStream(&unreliableStream, size);
	
    //reliableStream = {};
    //reliableStream.inputBuffer = Buf_FromMalloc(malloc(size), size);
    //Buf_Clear(&reliableStream.inputBuffer);
    //reliableStream.outputBuffer = Buf_FromMalloc(malloc(size), size);
    //Buf_Clear(&reliableStream.outputBuffer);

    // Enqueue command
    TestCommand1 cmd = {};
    Cmd_Prepare(&cmd.header, 1, 0);
    cmd.header.type = CMD_TYPE_TEST;
    cmd.header.size = sizeof(TestCommand1);
    cmd.data1 = 1234;
    cmd.data2 = 5678;
    Stream_EnqueueReliableOutput(&reliableStream, &cmd.header);
    
    // Enqueue command
    TestCommand2 cmd2 = {};
    Cmd_Prepare(&cmd2.header, 3, 0);
    cmd2.header.type = CMD_TYPE_TEST_2;
    cmd2.header.size = sizeof(TestCommand2);
    cmd2.pos[0] = 8642;
    cmd2.pos[2] = 9753;
    Stream_EnqueueReliableOutput(&reliableStream, &cmd2.header);

    // Enqueue command
    cmd.data1 = 4321;
    cmd.data2 = 8765;
    Stream_EnqueueReliableOutput(&reliableStream, &cmd.header);

    // Enqueue command
    cmd.data1 = 1357;
    cmd.data2 = 2468;
    Stream_EnqueueReliableOutput(&reliableStream, &cmd.header);

    PrintCommandBuffer(
        reliableStream.outputBuffer.ptrStart, reliableStream.outputBuffer.Written());
	
	i32 sequence = 1;
	#if 0
	printf("Find output command seq %d\n", sequence);
	Command* command = Stream_FindMessageBySequence(
		reliableStream.outputBuffer.ptrStart, reliableStream.outputBuffer.Written(), sequence);
	if (command)
	{
		PrintCommand(command);
	}
	else
	{
		printf("Couldn't find output cmd %d\n", sequence);
	}
    #endif
	//Stream_DeleteCommand(&reliableStream.outputBuffer, command);
	//printf("Deleted command %d\n", sequence);
	//PrintCommandBuffer(reliableStream.outputBuffer.ptrStart, reliableStream.outputBuffer.Written());
	
	//printf("Deleted command %d\n", 0);
	//Stream_DeleteCommandBySequence(&reliableStream.outputBuffer, 0);
	//PrintCommandBuffer(reliableStream.outputBuffer.ptrStart, reliableStream.outputBuffer.Written());
	
	
	//Stream_DeleteCommandBySequence(&reliableStream.outputBuffer, 2);
	//PrintCommandBuffer(reliableStream.outputBuffer.ptrStart, reliableStream.outputBuffer.Written());
    printf("\n");
    u8 buf[1400];
    i32 written = Packet_WriteFromStream(
		&reliableStream, &unreliableStream, buf, 1400, 0, 3, 1);
    
    PacketDescriptor p;
    
    i32 result = Packet_InitDescriptor(&p, buf, written);
    printf("Packet descriptor init result: %d\n", result);
    if (result == COM_ERROR_NONE)
    {
        PrintCommandBuffer((p.ptr + p.reliableOffset), p.numReliableBytes);
    }
    printf("Done!\n");

    // Hey lets explore endianess
    printf("0X%X\n", 0xDEADBEEF);
    u8 bytes[4];
    bytes[0] = 0xDE;
    bytes[1] = 0xAD;
    bytes[2] = 0xBE;
    bytes[3] = 0xEF;
    printf("0X%X%X%X%X\n", bytes[3], bytes[2], bytes[1], bytes[0]);

    *(i32*)bytes = 0xDEADBEEF;
    printf("0X%X%X%X%X\n", bytes[3], bytes[2], bytes[1], bytes[0]);
}
