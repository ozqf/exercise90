#pragma once

#include "../../common/com_module.h"
#include "../../app/commands.h"
#include "../../app/stream.h"

#define CMD_TYPE_TEST 1

struct TestCommand1
{
    Command header;
    i32 data1;
    i32 data2;
};

struct TestCommand2
{
    Command header;
    i32 data1;
    i32 data2;
};

NetStream g_stream;

void PrintCommand(Command* header)
{
    printf("Read Cmd type 1. sequence %d, tick %d\n",
		header->sequence, header->tick);
    switch (header->type)
    {
        case CMD_TYPE_TEST:
        {
            TestCommand1* cmd = (TestCommand1*)header;
            printf("Cmd data1: %d, data2: %d\n", cmd->data1, cmd->data2);
        } break;
    }
}

void PrintCommandBuffer(u8* ptr, i32 numBytes)
{
    u8* read = ptr;
    u8* end = ptr + numBytes;

    while(read < end)
    {
        Command* header = (Command*)read;
        Assert(Cmd_Validate(header) == COM_ERROR_NONE);
        read += header->size;
        PrintCommand(header);
    }
}

void TestCommandStream()
{
    printf("=== Test command streams ===\n");

    // init buffers
    i32 size = KiloBytes(64);
    g_stream = {};
    g_stream.inputBuffer = Buf_FromMalloc(malloc(size), size);
    Buf_Clear(&g_stream.inputBuffer);
    g_stream.outputBuffer = Buf_FromMalloc(malloc(size), size);
    Buf_Clear(&g_stream.outputBuffer);

    // Enqueue command
    TestCommand1 cmd = {};
    Cmd_Prepare(&cmd.header, 1, 0);
    cmd.header.type = CMD_TYPE_TEST;
    cmd.header.size = sizeof(TestCommand1);
    cmd.data1 = 1234;
    cmd.data2 = 5678;
    Stream_EnqueueReliableOutput(&g_stream, &cmd.header);
    
    // Enqueue command
    TestCommand2 cmd2 = {};

    // Enqueue command
    cmd.data1 = 4321;
    cmd.data2 = 8765;
    Stream_EnqueueReliableOutput(&g_stream, &cmd.header);

    // Enqueue command
    cmd.data1 = 1357;
    cmd.data2 = 2468;
    Stream_EnqueueReliableOutput(&g_stream, &cmd.header);

    PrintCommandBuffer(
        g_stream.outputBuffer.ptrStart, g_stream.outputBuffer.Written());
	
	i32 sequence = 1;
	
	printf("Find output command seq %d\n", sequence);
	Command* command = Stream_FindMessageBySequence(
		g_stream.outputBuffer.ptrStart, g_stream.outputBuffer.Written(), sequence);
	if (command)
	{
		PrintCommand(command);
	}
	else
	{
		printf("Couldn't find output cmd %d\n", sequence);
	}
	Stream_DeleteCommand(&g_stream.outputBuffer, command);
	printf("Deleted command %d\n", sequence);
	PrintCommandBuffer(g_stream.outputBuffer.ptrStart, g_stream.outputBuffer.Written());
	
	printf("Deleted command %d\n", 0);
	Stream_DeleteCommandBySequence(&g_stream.outputBuffer, 0);
	PrintCommandBuffer(g_stream.outputBuffer.ptrStart, g_stream.outputBuffer.Written());
}
