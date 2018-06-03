#pragma once

#include "../../common/com_module.h"

#include "game_types.h"

//#include "game_physics.h"
#include "../Physics/ZPhysics_interface.h"

// MODE      | ID        SERVER      CLIENT      CONNECTION
//--------------------------------------------------------------
// SP        |  0          1           1           0
// Client    |  1          0           1           1
// Listen    |  2          1           1           1
// Dedicated |  3          1           0           1
// Replay    |  4          0           1           0
#define NET_MODE_SINGLE_PLAYER 0
#define NET_MODE_CLIENT 1
#define NET_MODE_LISTEN_SERVER 2
#define NET_MODE_DEDICATED_SERVER 3
#define NET_MODE_REPLAY 4

inline u8 IsRunningClient(u8 netMode)
{
    return (
           netMode == NET_MODE_SINGLE_PLAYER
        || netMode == NET_MODE_LISTEN_SERVER
        || netMode == NET_MODE_CLIENT
        || netMode == NET_MODE_REPLAY
    );
}

inline u8 IsRunningServer(u8 netMode)
{
    return (
           netMode == NET_MODE_SINGLE_PLAYER
        || netMode == NET_MODE_LISTEN_SERVER
        || netMode == NET_MODE_DEDICATED_SERVER
    );
}

inline u8 IsConnectionOpen(u8 netMode)
{
    return (
        netMode == NET_MODE_CLIENT
        || netMode == NET_MODE_LISTEN_SERVER
        || netMode == NET_MODE_DEDICATED_SERVER
    );
}


// Entity and entity component lists
#define GAME_MAX_ENTITIES 2048
#define GAME_MAX_CLIENTS 8

#define UI_MAX_ENTITIES 512

#define GAME_DEBUG_BUFFER_LENGTH 1024

#define PLAYER_MOVE_SPEED 6.5f
#define PLAYER_ROTATION_SPEED 135.0f

#define TEST_PROJECTILE_SPEED 40.0f

// TODO: Remove this stuff sometime
global_variable u8 g_testAsciChar = '+';
global_variable char* g_testString = "Hello, World!...\nHey look, new line!";
global_variable u8 g_input_escapePressed = 0;
global_variable u8 cycleTexturePressed = 0;

global_variable ZStringHeader g_debugStr;
global_variable char g_debugStrBuffer[GAME_DEBUG_BUFFER_LENGTH];
global_variable RendObj g_debugStrRenderer;

void Game_InitDebugStr()
{
    g_debugStr = {};
    g_debugStr.chars = g_debugStrBuffer;
    g_debugStr.maxLength = GAME_DEBUG_BUFFER_LENGTH;
}

void Game_SetDebugStringRender()
{
    RendObj_SetAsAsciCharArray(&g_debugStrRenderer, g_debugStr.chars, g_debugStr.length, 0.05f, 0, 1, 1);
}

/////////////////////////////////////////////////////////////
// Render Objects Memory
/////////////////////////////////////////////////////////////
// Game
global_variable RenderScene g_worldScene;
global_variable RenderListItem g_scene_renderList[GAME_MAX_ENTITIES];

// UI
global_variable RenderScene g_uiScene;
global_variable RenderListItem g_ui_renderList[UI_MAX_ENTITIES];

/////////////////////////////////////////////////////////////
// Entity Memory
/////////////////////////////////////////////////////////////
// Game
global_variable GameState g_gameState;
global_variable Ent g_gameEntities[GAME_MAX_ENTITIES];

global_variable Player g_players[GAME_MAX_CLIENTS];

// UI
global_variable GameState g_uiState;
global_variable Ent g_uiEntities[UI_MAX_ENTITIES];

// Game Command I/O buffer handles
global_variable BlockRef g_gameInputBufferRef;
global_variable ByteBuffer g_gameInputByteBuffer;
global_variable BlockRef g_gameOutputBufferRef;
global_variable ByteBuffer g_gameOutputByteBuffer;

// Physics engine buffer handles
global_variable BlockRef g_collisionCommandBuffer;
global_variable BlockRef g_collisionEventBuffer;

// Client input
global_variable ClientTick g_localClientTick;


/////////////////////////////////////////////////////////////
// Entity Components Memory
/////////////////////////////////////////////////////////////
// Game
global_variable EC_Collider         g_colliders[GAME_MAX_ENTITIES];
global_variable EC_Renderer         g_renderers[GAME_MAX_ENTITIES];
global_variable EC_ActorMotor       g_actorMotors[GAME_MAX_ENTITIES];
global_variable EC_AIController     g_aiControllers[GAME_MAX_ENTITIES];
global_variable EC_Projectile       g_prjControllers[GAME_MAX_ENTITIES];
global_variable EC_Label            g_entLabels[GAME_MAX_ENTITIES];

// UI
global_variable EC_Renderer         g_ui_renderers[UI_MAX_ENTITIES];

/////////////////////////////////////////////////////////////
// Debug
/////////////////////////////////////////////////////////////
//M4x4 g_debugMatrix = {};
Transform g_debugTransform = {};
ClientTick g_debugInput = {};

#include "game_entities.h"
#include "comp_rendObj.h"
#include "game_commands.h"
#include "comp_aiController.h"

#include "game_entityFactory.cpp"
#include "game_ent_components.cpp"
#include "game_input.cpp"
#include "game_commands.cpp"
#include "game.cpp"
