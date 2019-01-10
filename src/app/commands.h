#pragma once
///////////////////////////////////////////////////////////
// Header only - commands shared between client and server
///////////////////////////////////////////////////////////
#include "../common/com_module.h"
#include "app.h"
#include "../sim/sim.h"

#define CMD_TYPE_NULL 0
#define CMD_TYPE_IMPULSE 1
#define CMD_TYPE_SET_SCENE 2

// 48879?
#define CMD_SENTINEL 0xBEEF
#define CMD_INVALID_SIZE 0

// BASE FOR ALL COMMANDS
// All commands MUST have a SimCmd as their first member, for
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

struct CmdImpulse
{
	Command header;
	u8 impulse;
};

struct CmdSetScene
{
    Command header;
    i32 sceneId;
};

struct CmdSetPrivateUser
{
    Command header;
    i32 connectionId;
    i32 userId;
};

struct CmdUserState
{
    Command header;
    i32 userId;
    i32 state;
};

internal inline i32 Cmd_Validate(Command* cmd)
{
    if (cmd == NULL)  { return COM_ERROR_BAD_ARGUMENT; }
    if (cmd->sentinel != SIM_CMD_SENTINEL) { return COM_ERROR_DESERIALISE_FAILED; }
    if (cmd->type == 0) { return COM_ERROR_UNKNOWN_COMMAND; }
    if (cmd->size <= 0) { return COM_ERROR_BAD_SIZE; }
    return COM_ERROR_NONE;
}

internal void Cmd_Prepare(Command* cmd, i32 tick, i32 sequence)
{
    cmd->sentinel = CMD_SENTINEL;
    cmd->tick = tick;
    cmd->sequence = sequence;
    cmd->type = CMD_TYPE_NULL;
    cmd->size = CMD_INVALID_SIZE;
}


internal void Cmd_InitSetScene(CmdSetScene* cmd, i32 tick, i32 sequence, i32 sceneId)
{
    *cmd = {};
    Cmd_Prepare((Command*)&cmd->header, tick, sequence);
    cmd->header.size = sizeof(CmdSetScene);
    cmd->header.type = CMD_TYPE_SET_SCENE;
    cmd->sceneId = sceneId;
}
