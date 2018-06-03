#pragma once

#include "../../common/com_module.h"

#define COLLISION_LAYER_WORLD (1 << 0)
#define COLLISION_LAYER_ACTOR (1 << 1)
#define COLLISION_LAYER_DEBRIS (1 << 2)

internal u16 COL_MASK_DEFAULT = COLLISION_LAYER_WORLD;
internal u16 COL_MASK_ACTOR = COLLISION_LAYER_WORLD | COLLISION_LAYER_ACTOR;
internal u16 COL_MASK_DEBRIS = COLLISION_LAYER_WORLD | COLLISION_LAYER_DEBRIS;

//////////////////////////////////////////////////
// Game Commands
//////////////////////////////////////////////////

#define CMD_TYPE_SPAWN 100
#define CMD_TYPE_IMPULSE 101
#define CMD_TYPE_PLAYER_INPUT 102
#define CMD_TYPE_CLIENT_UPDATE 103

//////////////////////////////////////////////////
// Spawning
#define ENTITY_TYPE_NULL 0
#define ENTITY_TYPE_WORLD_CUBE 1
#define ENTITY_TYPE_RIGIDBODY_CUBE 2
#define ENTITY_TYPE_ACTOR_GROUND 3
#define ENTITY_TYPES_COUNT 4

struct Cmd_Spawn
{
    i32 factoryType;
	EntId entityId;
    Vec3 pos;
    Vec3 rot;
    Vec3 size;
    u32 flags;
};

//////////////////////////////////////////////////
// Player management

#define PLAYER_STATE_FREE 0
#define PLAYER_STATE_ACTOR 1
#define PLAYER_STATE_DEAD 2
struct Cmd_PlayerInput
{
	i32 clientId;
    i32 playerId;
    i32 state;
    ActorInput input;
};

//struct Cmd_SpawnPlayer
//{
//	i32 playerId;
//	Cmd_Spawn spawnCmd;
//};

//////////////////////////////////////////////////
// Client management
#define CLIENT_STATE_FREE 0
#define CLIENT_STATE_OBSERVER 1
#define CLIENT_STATE_PLAYING 2
struct Cmd_ClientUpdate
{
    i32 clientId;
    i32 state;
    i32 playerId;
};

//////////////////////////////////////////////////
// Super simple one-off commands
// Always exec on the server!
#define IMPULSE_NULL 0
#define IMPULSE_JOIN_GAME 1
struct Cmd_ServerImpulse
{
    i32 clientId;
    i32 impulse;
};

Ent* Exec_Spawn(GameState* gs, Cmd_Spawn* cmd);
