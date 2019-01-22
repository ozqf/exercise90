#pragma once
/*
Header only - base for command structs.
*/
#include "../common/com_module.h"
#include "app.h"
#include "../sim/sim.h"

// a null command is invalid and will cause command validation to fail!
#define CMD_TYPE_NULL 0

// 48879?
#define CMD_SENTINEL 0xBEEF
#define CMD_INVALID_SIZE 0

// BASE FOR ALL COMMANDS
// All commands MUST have a Command struct as their first member, for
// pointer casting
struct Command
{
    // for alignment checking
    i32 sentinel;
    // Type and Size must be set by the implementing command
    i32 type;
    i32 size;

    // Controls execution time and order
    u32 tick;
    i32 sequence;
};

internal inline i32 Cmd_Validate(Command* cmd)
{
    if (cmd == NULL)  { return COM_ERROR_BAD_ARGUMENT; }
    if (cmd->sentinel != SIM_CMD_SENTINEL) { return COM_ERROR_DESERIALISE_FAILED; }
    if (cmd->type == 0) { return COM_ERROR_UNKNOWN_COMMAND; }
    if (cmd->size <= 0) { return COM_ERROR_BAD_SIZE; }
    return COM_ERROR_NONE;
}

internal inline void Cmd_WriteToByteBuffer(ByteBuffer* b, Command* cmd)
{
    Assert(!Cmd_Validate(cmd))
    Assert(b->Space() >= cmd->size)
    b->ptrWrite += COM_COPY(cmd, b->ptrWrite, cmd->size);
}

internal void Cmd_Prepare(Command* cmd, i32 tick, i32 sequence)
{
    cmd->sentinel = CMD_SENTINEL;
    cmd->tick = tick;
    cmd->sequence = sequence;
    cmd->type = CMD_TYPE_NULL;
    cmd->size = CMD_INVALID_SIZE;
}
