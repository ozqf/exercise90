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
#define CMD_TYPE_ENTITY_SYNC 107
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

#define IMPULSE_NULL 0
#define IMPULSE_JOIN_GAME 1
#define IMPULSE_NEXT_WEAPON 2
struct Cmd_ServerImpulse;
struct Cmd_PlayerInput;
struct Cmd_ClientUpdate;
struct Cmd_Text;
struct Cmd_RemoveEntity;
struct Cmd_ActorSync;
struct Cmd_EntityStateHeader;
struct Cmd_SpawnProj;
struct Cmd_GameSessionState;
#define CMD_SYNC_FILE_NAME_LENGTH 255
struct Cmd_S2C_Sync;
struct Cmd_C2S_Sync;
struct Cmd_Packet;
struct Cmd_SpawnViaTemplate;
struct Cmd_SpawnHudItem;
struct Cmd_Test;