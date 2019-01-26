#pragma once
///////////////////////////////////////////////////////////
// Header only - commands shared between client and server
///////////////////////////////////////////////////////////

#include "commands_base.h"

#define CMD_TYPE_IMPULSE 255
#define CMD_TYPE_S2C_HANDSHAKE 254
#define CMD_TYPE_S2C_SET_SCENE 253
#define CMD_TYPE_S2C_SPAWN_ENTITY 252

struct CmdPing
{
	Command header;
	i32 pingSequence;
    f32 sendTime;
};

struct S2C_SpawnEntity
{
    Command header;
    i32 networkId;
    u8 entType;
    Vec3 pos;
	Vec3 vel;
    f32 pitch;
    f32 yaw;
};

internal void Cmd_InitSpawnEntity(S2C_SpawnEntity* cmd, i32 tick, i32 sequence)
{
    *cmd = {};
    Cmd_Prepare(&cmd->header, tick, sequence);
    cmd->header.size = sizeof(S2C_SpawnEntity);
    cmd->header.type = CMD_TYPE_S2C_SPAWN_ENTITY;


}

// SV -> CL
struct S2C_Handshake
{
    Command header;
    i32 privateId;
};

internal void Cmd_InitHandshake(S2C_Handshake* cmd, i32 tick, i32 sequence, i32 privateId)
{
    *cmd = {};
    Cmd_Prepare(&cmd->header, tick, sequence);
    cmd->header.size = sizeof(S2C_Handshake);
    cmd->header.type = CMD_TYPE_S2C_HANDSHAKE;
    cmd->privateId = privateId;
}

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

internal void Cmd_InitSetScene(CmdSetScene* cmd, i32 tick, i32 sequence, i32 sceneId)
{
    *cmd = {};
    Cmd_Prepare((Command*)&cmd->header, tick, sequence);
    cmd->header.size = sizeof(CmdSetScene);
    cmd->header.type = CMD_TYPE_S2C_SET_SCENE;
    cmd->sceneId = sceneId;
}

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
