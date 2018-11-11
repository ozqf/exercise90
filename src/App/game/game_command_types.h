#pragma once

#include "../../common/com_module.h"

#define COLLISION_LAYER_WORLD (1 << 0)
#define COLLISION_LAYER_ACTOR (1 << 1)
#define COLLISION_LAYER_PROJECTILE (1 << 2)
#define COLLISION_LAYER_DEBRIS (1 << 3)
#define COLLISION_LAYER_FENCE (1 << 4)
#define COLLISION_LAYER_VOLUME (1 << 5)

internal u16 COL_MASK_DEFAULT = 
    COLLISION_LAYER_WORLD |
    COLLISION_LAYER_ACTOR |
    COLLISION_LAYER_DEBRIS;

internal u16 COL_MASK_ACTOR =
    COLLISION_LAYER_WORLD |
    COLLISION_LAYER_ACTOR |
    COLLISION_LAYER_FENCE |
	COLLISION_LAYER_VOLUME |
	COLLISION_LAYER_PROJECTILE;

internal u16 COL_MASK_DEBRIS =
    COLLISION_LAYER_WORLD |
    COLLISION_LAYER_DEBRIS;

internal u16 COL_MASK_FENCE =
    COLLISION_LAYER_ACTOR;
	
internal u16 COL_MASK_VOLUME =
	COLLISION_LAYER_ACTOR;

internal u16 COL_MASK_PROJECTILE =
    COLLISION_LAYER_WORLD |
    COLLISION_LAYER_ACTOR |
    COLLISION_LAYER_DEBRIS;

//////////////////////////////////////////////////
// Game Commands
// First byte of any command or message should
// be the type. Read should check this byte is
// correct
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
#define CMD_TYPE_ENTITY_STATE_2 109
#define CMD_TYPE_GAME_SESSION_STATE 110
#define CMD_TYPE_GAME_CAMPAIGN_STATE 111
#define CMD_TYPE_SPAWN_VIA_TEMPLATE 112
#define CMD_TYPE_SPAWN_GFX 113
#define CMD_TYPE_SPAWN_HUD_ITEM 114

// Define placeholder raw read/write functions for commands.

#ifndef GAME_CMD_DEFAULT_INTERFACE
//#define GAME_CMD_DEFAULT_INTERFACE(structType)
//#endif

#define GAME_CMD_DEFAULT_INTERFACE(structType, u8_cmdType)\
\
inline u16 MeasureForReading(u8* ptr) \
{ \
    u16 size = sizeof(structType) + sizeof(u8); \
    return size; \
} \
\
inline u16 MeasureForWriting() \
{ \
    u16 size = sizeof(structType) + sizeof(u8); \
    return size; \
} \
\
inline u16 Write(u8* ptr) \
{\
    u8* start = ptr; \
    ptr += COM_WriteByte(u8_cmdType, ptr); \
    u32 structSize = sizeof(structType); \
    ptr += COM_COPY(this, ptr, structSize); \
    return (u16)(ptr - start); \
} \
\
inline u16 Read(u8* ptr) \
{ \
    u8* start = ptr; \
    u8 commandType = COM_ReadByte(&ptr); \
    APP_ASSERT((commandType == u8_cmdType), "CMD read: type mismatched\n"); \
    ptr += COM_COPY(ptr, this, sizeof(structType)); \
    return (u16)(ptr - start); \
}
#endif

//////////////////////////////////////////////////
// 101
// Super simple one-off commands
// Always exec on the server!
#define IMPULSE_NULL 0
#define IMPULSE_JOIN_GAME 1
#define IMPULSE_NEXT_WEAPON 2
struct Cmd_ServerImpulse
{
    i32 clientId;
    i32 impulse;
    
    GAME_CMD_DEFAULT_INTERFACE(Cmd_ServerImpulse, CMD_TYPE_IMPULSE)
    // inline u16 WriteRaw(u8* ptr)
    // {
    //     printf("Copying Impulse cmd\n");
    //     return (u16)COM_COPY()
    // }
    
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

    GAME_CMD_DEFAULT_INTERFACE(Cmd_PlayerInput, CMD_TYPE_PLAYER_INPUT)
};
#endif
//////////////////////////////////////////////////
// 103
// Client management
struct Cmd_ClientUpdate
{
    // If a client receives an update and the connId matches theres, it identifies
    // their client for them.
    // connId must be 0 for all other clients or it isn't private!
    i32 connId;
	i32 clientId;
	i32 connectionId;
    i32 state;
	u32 flags;
    EntId entId;
    ActorInput input;
    //i32 playerId;

    void Set(Client* cl)
    {
        clientId = cl->clientId;
        state = cl->state;
        connectionId = cl->connectionId;
        flags = cl->flags;
        entId = cl->entId;
        input = cl->input;
    }

    GAME_CMD_DEFAULT_INTERFACE(Cmd_ClientUpdate, CMD_TYPE_CLIENT_UPDATE)
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
#if 0
// TODO: REMOVE THIS WHEN SECOND VERISON OF ECS IS WORKING
/*
TODO: This state update object is cramming every component's possible
state data into one struct, some of it overlapping between entity types.
Very ugly. Refactor somehow, or dump ECS and just have monolithic entities!
*/
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
    Ticker ticker;

    GAME_CMD_DEFAULT_INTERFACE(Cmd_EntityState)
};
#endif
// 106
struct Cmd_RemoveEntity
{
    EntId entId;
    i32 gfxNormal;

    GAME_CMD_DEFAULT_INTERFACE(Cmd_RemoveEntity, CMD_TYPE_REMOVE_ENT)
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
	
	GAME_CMD_DEFAULT_INTERFACE(Cmd_PlayerState, CMD_TYPE_PLAYER_STATE)
};

// 109 CMD_TYPE_ENTITY_STATE_2
struct Cmd_EntityStateHeader
{
    EntId entId;
    u32 componentBits;
};

// 109 CMD_TYPE_ENTITY_STATE_2
struct Cmd_EntitySpawn2
{
    EntId entId;
    i32 factoryType;
    EntitySpawnOptions options;
};

//Ent* Exec_Spawn(GameState* gs, Cmd_Spawn* cmd);

struct Cmd_GameSessionState
{
	i32 state;
	
	GAME_CMD_DEFAULT_INTERFACE(Cmd_GameSessionState, CMD_TYPE_GAME_SESSION_STATE)
};

struct Cmd_GameCampaignState
{
	i32 state;
	
	GAME_CMD_DEFAULT_INTERFACE(Cmd_GameCampaignState, CMD_TYPE_GAME_CAMPAIGN_STATE)
};

struct Cmd_SpawnViaTemplate
{
	i32 factoryType;
	EntitySpawnOptions options;
	
	GAME_CMD_DEFAULT_INTERFACE(Cmd_SpawnViaTemplate, CMD_TYPE_SPAWN_VIA_TEMPLATE)
};

// CMD_TYPE_SPAWN_HUD_ITEM

struct Cmd_SpawnHudItem
{
    i32 type;
    Vec3 pos;
    EntId source;
    EntId victim;

    GAME_CMD_DEFAULT_INTERFACE(Cmd_SpawnHudItem, CMD_TYPE_SPAWN_HUD_ITEM)
};
