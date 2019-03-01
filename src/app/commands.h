#pragma once
///////////////////////////////////////////////////////////
// Header only - commands shared between client and server
///////////////////////////////////////////////////////////

#include "commands_base.h"

#define CMD_TYPE_IMPULSE 255
#define CMD_TYPE_S2C_HANDSHAKE 254
#define CMD_TYPE_S2C_SET_SCENE 253
#define CMD_TYPE_S2C_SPAWN_ENTITY 252
#define CMD_TYPE_S2C_SPAWN_PROJECTILE 251
#define CMD_TYPE_S2C_SYNC 250
#define CMD_TYPE_C2S_INPUT 249
#define CMD_TYPE_S2C_SYNC_ENTITY 248
#define CMD_TYPE_PING 247
#define CMD_TYPE_S2C_INPUT_RESPONSE 246

struct CmdPing
{
	Command header;
	i32 pingSequence;
    f32 sendTime;
};

internal void Cmd_InitPing(CmdPing* cmd, i32 tick, i32 sequence, f32 time)
{
    Cmd_Prepare(&cmd->header, tick, sequence);
    cmd->header.type = CMD_TYPE_PING;
    cmd->header.size = sizeof(CmdPing);
    cmd->sendTime = time;
}

struct S2C_Sync
{
    Command header;
    i32 simTick;
    // ticks client should delay themselves by to avoid jitter
    i32 jitterTickCount;
    i32 avatarEntityId;
};

internal void Cmd_InitSync(
    S2C_Sync* cmd,
    i32 tick,
    i32 sequence,
    i32 simTick,
    i32 jitterTickCount,
    i32 avatarEntityId)
{
    *cmd = {};
    Cmd_Prepare(&cmd->header, tick, sequence);
    cmd->header.type = CMD_TYPE_S2C_SYNC;
    cmd->header.size = sizeof(S2C_Sync);
    cmd->simTick = simTick;
    cmd->jitterTickCount = jitterTickCount;
    cmd->avatarEntityId = avatarEntityId;
}

struct S2C_InputResponse
{
    Command header;
    i32 lastUserInputSequence;
    Vec3 latestAvatarPos;
};

internal void Cmd_InitInputResponse(
    S2C_InputResponse* cmd,
    i32 tick,
    i32 lastInputSequence,
    Vec3 avatarPos
)
{
    Cmd_Prepare(&cmd->header, tick, 0);
    cmd->header.type = CMD_TYPE_S2C_INPUT_RESPONSE;
    cmd->header.size = sizeof(S2C_InputResponse);
    cmd->lastUserInputSequence = lastInputSequence;
    cmd->latestAvatarPos = avatarPos;
}

struct C2S_Input
{
	Command header;
    i32 userInputSequence;
	SimActorInput input;
	Vec3 avatarPos;
};

internal void Cmd_InitClientInput(
	C2S_Input* cmd,
    i32 userInputSequence,
	SimActorInput* input,
	Vec3* avatarPos,
	i32 tick
	)
{
	*cmd = {};
	Cmd_Prepare(&cmd->header, tick, 0);
	cmd->header.type = CMD_TYPE_C2S_INPUT;
	cmd->header.size = sizeof(C2S_Input);
    cmd->userInputSequence = userInputSequence;
	if (input)
	{
		cmd->input = *input;
	}
	if (avatarPos)
	{
		cmd->avatarPos = *avatarPos;
	}
}

struct S2C_SpawnEntity
{
    Command header;
    // All data that must be replicated to spawn this entity
    // have to be here. The rest is client side
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

struct S2C_SpawnProjectile
{
    Command header;
    SimProjectileSpawnDef def;
};

internal void Cmd_InitSpawnEntity(S2C_SpawnProjectile* cmd, i32 tick, i32 sequence)
{
    *cmd = {};
    Cmd_Prepare(&cmd->header, tick, sequence);
    cmd->header.size = sizeof(S2C_SpawnProjectile);
    cmd->header.type = CMD_TYPE_S2C_SPAWN_PROJECTILE;
}

struct S2C_EntitySync
{
    // 20 + 4 + (3 x 4) + (3 x 4) + (3 x 4) bytes
	Command header;
	i32 networkId;
	Vec3 pos;
	Vec3 rot;
	Vec3 vel;
};

internal void Cmd_WriteEntitySync(S2C_EntitySync* cmd, i32 tick, i32 sequence, SimEntity* ent)
{
    Cmd_Prepare(&cmd->header, tick, sequence);
    cmd->header.type = CMD_TYPE_S2C_SYNC_ENTITY;
    cmd->header.size = sizeof(S2C_EntitySync);
    cmd->networkId = ent->id.serial;
	cmd->pos = ent->t.pos;
	cmd->vel = ent->velocity;
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
