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
#define CMD_TYPE_QUICK 108
#define CMD_TYPE_ENTITY_STATE_2 109
#define CMD_TYPE_GAME_SESSION_STATE 110
#define CMD_TYPE_GAME_CAMPAIGN_STATE 111
#define CMD_TYPE_SPAWN_VIA_TEMPLATE 112
#define CMD_TYPE_SPAWN_GFX 113
#define CMD_TYPE_SPAWN_HUD_ITEM 114
#define CMD_TYPE_S2C_SYNC 115
#define CMD_TYPE_C2S_SYNC 116
#define CMD_TYPE_PACKET 117

#define CMD_TYPE_TEST 255

//////////////////////////////////////////////////
// DEFAULT COMMAND INTERFACE FUNCTIONS
// All commands should follow this interface to be
// usable in read/write macros
//////////////////////////////////////////////////

/**
 * Should be safe for any command
 */
#ifndef GAME_CMD_DEFAULT_GetType
#define GAME_CMD_DEFAULT_GetType(u8_cmdType) \
inline u8 GetType() { return u8_cmdType##; }
#endif

/**
 * Raw read/write functions for commands
 * 
 * TODO: This is a placeholder and the raw struct copies MUST be
 * replaced with proper encoding functions!
 */
#ifndef GAME_CMD_DEFAULT_Read_Write
#define GAME_CMD_DEFAULT_Read_Write(structType, u8_cmdType) \
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


#ifndef GAME_CMD_DEFAULT_INTERFACE
#define GAME_CMD_DEFAULT_INTERFACE(structType, u8_cmdType) \
GAME_CMD_DEFAULT_GetType(##u8_cmdType##) \
GAME_CMD_DEFAULT_Read_Write(##structType##, u8_cmdType##)
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
    // Use private id so player input must come from the correct client?
	// i32 connectionId;
    // This struct is also sent from client to client for prediction:
    i32 clientId;
    ActorInput input;
    Vec3 avatarPosition;    // Have to sanity check this on server

    GAME_CMD_DEFAULT_INTERFACE(Cmd_PlayerInput, CMD_TYPE_PLAYER_INPUT)

    void Set(Client* cl)
    {
        clientId = cl->clientId;
        input = cl->input;
    }
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

    void Set(Client* cl, i32 setPrivateId)
    {
        clientId = cl->clientId;
        state = cl->state;
        flags = cl->flags;
        entId = cl->entId;
        input = cl->input;
		if (setPrivateId)
		{
			connectionId = cl->connectionId;
		}
		else
		{
			connectionId = 0;
		}
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

#define CMD_QUICK_TYPE_CONFIRM_CLIENT_ID 1
#define CMD_QUICK_TYPE_PACKET_DELIVERED 2
// 108 For quick data transfer
struct Cmd_Quick
{
    u8 quickType;
    struct
    {
        i32 clientId;

    };
    struct
    {
        i32 connectionId;
        u32 packetNumber;
    };

    void SetAsConfimClientId(i32 newClientId)
    {
        quickType = CMD_QUICK_TYPE_CONFIRM_CLIENT_ID;
        clientId = newClientId;
    }

    void SetAsPacketDelivered(i32 connId, u32 newPacketNumber)
    {
        quickType = CMD_QUICK_TYPE_PACKET_DELIVERED;
        connectionId = connId;
        packetNumber = newPacketNumber;
    }

	GAME_CMD_DEFAULT_INTERFACE(Cmd_Quick, CMD_TYPE_QUICK)
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

//Ent* Exec_Spawn(GameScene* gs, Cmd_Spawn* cmd);

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

#define CMD_SYNC_FILE_NAME_LENGTH 255
struct Cmd_S2C_Sync
{
    // length excludes \0 terminator
    u8 length;
	char fileName[CMD_SYNC_FILE_NAME_LENGTH];
	
	GAME_CMD_DEFAULT_GetType(CMD_TYPE_S2C_SYNC)

    inline u16 MeasureForReading(u8* ptr)
    {
        return sizeof(u8) + length;// + sizeof('\0');
    }
    inline u16 MeasureForWriting()
    {
        return sizeof(u8) + length;// + sizeof('\0');
    }
    inline u16 Write(u8* ptr)
    {
        u8* start = ptr;
        ptr += COM_WriteByte(CMD_TYPE_S2C_SYNC, ptr);
        if (length >= CMD_SYNC_FILE_NAME_LENGTH)
        {
            // force space for null terminator
            length = (CMD_SYNC_FILE_NAME_LENGTH - 1);
        }
        ptr += COM_WriteByte(length, ptr);
        ptr += COM_COPY(fileName, ptr, length);
        return (u16)(ptr - start);
    }
    inline u16 Read(u8* ptr)
    {
        u8* start = ptr;
        u8 commandType = COM_ReadByte(&ptr);
        APP_ASSERT((commandType == CMD_TYPE_S2C_SYNC), "Cmd read type mismatch");
        length = COM_ReadByte(&ptr);
        ptr += COM_COPY(ptr, fileName, length);
        *ptr = '\0';
        return (u16)(ptr - start);
    }
};

struct Cmd_C2S_Sync
{
	i32 state;
	
	GAME_CMD_DEFAULT_INTERFACE(Cmd_GameCampaignState, CMD_TYPE_C2S_SYNC)
};

// This struct is a header, packet data should IMMEDIATELY follow
// Due to containing data outside this struct, this command
// cannot use the default interface and must be streamed into the
// app buffer.
struct Cmd_Packet
{
    i32 connectionId;
    i32 numBytes;

    inline u16 Measure()
    {
        return (u16)(sizeof(u8) + (sizeof(i32) * 2) + numBytes);
    }

    GAME_CMD_DEFAULT_GetType(CMD_TYPE_PACKET);
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

struct Cmd_Test
{
    i32 data;

    GAME_CMD_DEFAULT_INTERFACE(Cmd_Test, CMD_TYPE_TEST)
};
