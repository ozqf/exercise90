#pragma once
/*
Header only - base for command structs.
*/
#include "../common/common.h"
#include "app.h"
#include "../sim/sim.h"

// a null command is invalid and will cause command validation to fail!
#define CMD_TYPE_NULL 0

// 48879?
#define CMD_SENTINEL 0xDEADBEEF
#define CMD_INVALID_SIZE 0

// BASE FOR ALL COMMANDS
// All commands MUST have a Command struct as their first member, for
// pointer casting
struct Command
{
    // for alignment checking
    i32 sentinel;
    // Type and Size must be set by the implementing command
    u8 type;
    i32 size;

    // Controls execution time and order
    i32 tick;
    i32 sequence;
};

internal inline i32 Cmd_Validate(Command* cmd)
{
    if (cmd == NULL)  { return COM_ERROR_BAD_ARGUMENT; }
    if (cmd->sentinel != CMD_SENTINEL) { return COM_ERROR_DESERIALISE_FAILED; }
    if (cmd->type == 0) { return COM_ERROR_UNKNOWN_COMMAND; }
    if (cmd->size <= 0) { return COM_ERROR_BAD_SIZE; }
    return COM_ERROR_NONE;
}

internal inline void Cmd_WriteToByteBuffer(ByteBuffer* b, Command* cmd)
{
    ErrorCode err = Cmd_Validate(cmd);
    COM_ASSERT(!err, "Command failed validation")
    COM_ASSERT(b->Space() >= cmd->size, "No space for command")
    b->ptrWrite += COM_COPY(cmd, b->ptrWrite, cmd->size);
}


// TODO: Passing in sequence 0 often here as it is set by the
// stream when enqueued anyway. Is manually setting it ever required?
internal void Cmd_Prepare(Command* cmd, i32 tick, i32 sequence)
{
    cmd->sentinel = CMD_SENTINEL;
    cmd->tick = tick;
    cmd->sequence = sequence;
    cmd->type = CMD_TYPE_NULL;
    cmd->size = CMD_INVALID_SIZE;
}

////////////////////////////////////////////////////////////////////////////
// Debugging
////////////////////////////////////////////////////////////////////////////
#if 0
internal void Cmd_PrintHeader(Command* header)
{
    printf("Read Cmd type %d. sequence %d, tick %d\n",
		header->type, header->sequence, header->tick);
}

internal void Cmd_PrintBuffer(u8* ptr, i32 numBytes)
{
	printf("\n=== CMD BUFFER (%d bytes) ===\n", numBytes);
    u8* read = ptr;
    u8* end = ptr + numBytes;

    while(read < end)
    {
        Command* header = (Command*)read;
        Assert(Cmd_Validate(header) == COM_ERROR_NONE);
        read += header->size;
        Cmd_PrintHeader(header);
    }
	printf("  Ptr diff check: %d\n", (read - end));
}
#endif
