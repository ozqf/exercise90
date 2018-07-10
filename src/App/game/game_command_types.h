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

#define CMD_TYPE_STATIC_STATE 100
#define CMD_TYPE_IMPULSE 101
#define CMD_TYPE_PLAYER_INPUT 102
#define CMD_TYPE_CLIENT_UPDATE 103
#define CMD_TYPE_TEXT 104
#define CMD_TYPE_ENTITY_STATE 105
#define CMD_TYPE_REMOVE_ENT 106
#define CMD_TYPE_ENTITY_DELTA 107
#define CMD_TYPE_PLAYER_STATE 108

//////////////////////////////////////////////////
// Spawning
#define ENTITY_TYPE_NULL 0
#define ENTITY_TYPE_WORLD_CUBE 1
#define ENTITY_TYPE_RIGIDBODY_CUBE 2
#define ENTITY_TYPE_ACTOR_GROUND 3
#define ENTITY_TYPE_PROJECTILE 4
#define ENTITY_TYPES_COUNT 5

// 100
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
// 101
// Super simple one-off commands
// Always exec on the server!
#define IMPULSE_NULL 0
#define IMPULSE_JOIN_GAME 1
struct Cmd_ServerImpulse
{
    i32 clientId;
    i32 impulse;
    
    inline u16 WriteRaw(u8* ptr)
    {
        return 0;
    }
    
};

//////////////////////////////////////////////////
// Player management
#if 1
// #define PLAYER_STATE_FREE 0
// #define PLAYER_STATE_ACTOR 1
// #define PLAYER_STATE_DEAD 2
// 102
struct Cmd_PlayerInput
{
	i32 clientId;
    i32 playerId;
    i32 state;
    ActorInput input;
};
#endif
//////////////////////////////////////////////////
// 103
// Client management
#define CLIENT_STATE_FREE 0
#define CLIENT_STATE_OBSERVER 1
#define CLIENT_STATE_PLAYING 2
struct Cmd_ClientUpdate
{
    i32 clientId;
    i32 state;
    EntId entId;
    ActorInput input;
    //i32 playerId;
};

// 104
// Bytes is all bytes given to string including any padding
// or null terminator
struct Cmd_Text
{
    char* text;
    i32 bytes;
};

//struct Cmd_SpawnPlayer
//{
//	i32 playerId;
//	Cmd_Spawn spawnCmd;
//};

// 105
// struct Cmd_SpawnProjectile
// {
//     // What to spawn
//     Cmd_Spawn spawn;
//     f32 speed;
// };

// State save/load
// 105 - Complete entity restore
struct Cmd_EntityState
{
    // Identification
    i32 factoryType;
    EntId entityId;
    i32 tag;
    // total 8b
    
    // Linking
    EntId target;            // 4b
    EntId source;            // 4b
    // total 16b
    
    // Physical state
    Vec3 pos;                // 12b
    Vec3 rot;                // 12b
    Vec3 vel;                // 12b
    Vec3 size;               // 12b
    // total 48b
    
    // Settings
    u32 flags;
    f32 moveSpeed;
    f32 tick;                
    f32 tock;
    // total 12b
    // -- 84b so far --
    // packet of 1000b = 11 updates per packet

    inline u32 WriteRaw(u8* ptr)
    {
        return 0;
    }
    
    inline u32 ReadRaw(u8* ptr)
    {
        return 0;
    }
};

// 106
struct Cmd_RemoveEntity
{
    EntId entId;
    i32 gfxNormal;
};

// 107 - Entity delta contains a list of fields to update
// each bit in 'fields' relates to a field on the state to set.
// total size of this struct + the data after it should be encoded
// in the command header
struct Cmd_EntityDelta
{
    EntId entId;
    u32 size;
    u32 fields;
};

// 108 Player State
struct Cmd_PlayerState
{
    i32 playerId;
    i32 state;
    EntId avatarId;
};

Ent* Exec_Spawn(GameState* gs, Cmd_Spawn* cmd);
