#pragma once

#include "../../common/com_module.h"

#include "game_types.h"

//#include "game_physics.h"
#include "../Physics/ZPhysics_interface.h"

#define GAME_DEBUG_MODE_NONE 0
#define GAME_DEBUG_MODE_CAMERA 1
#define GAME_DEBUG_MODE_TRANSFORM 2
#define GAME_DEBUG_MODE_PHYSICS 3
#define GAME_DEBUG_MODE_ACTOR_INPUT 4
#define GAME_DEBUG_MODE_INPUT 5

// MODE      | ID        SERVER      CLIENT      CONNECTION
//--------------------------------------------------------------
// SP        |  0          1           1           0
// Client    |  1          0           1           1
// Listen    |  2          1           1           1
// Dedicated |  3          1           0           1
// Replay    |  4          0           1           0

#define NETMODE_NONE 0
#define NETMODE_SINGLE_PLAYER 1
#define NETMODE_CLIENT 2
#define NETMODE_LISTEN_SERVER 3
#define NETMODE_DEDICATED_SERVER 4
#define NETMODE_REPLAY 5

inline u8 IsRunningClient(u8 netMode)
{
    if (netMode == NETMODE_NONE) { return 0; }
    return (
           netMode == NETMODE_SINGLE_PLAYER
        || netMode == NETMODE_LISTEN_SERVER
        || netMode == NETMODE_CLIENT
        || netMode == NETMODE_REPLAY
    );
}

inline u8 IsRunningServer(u8 netMode)
{
    if (netMode == NETMODE_NONE) { return 0; }
    return (
           netMode == NETMODE_SINGLE_PLAYER
        || netMode == NETMODE_LISTEN_SERVER
        || netMode == NETMODE_DEDICATED_SERVER
    );
}

inline u8 IsConnectionOpen(u8 netMode)
{
    if (netMode == NETMODE_NONE) { return 0; }
    return (
        netMode == NETMODE_CLIENT
        || netMode == NETMODE_LISTEN_SERVER
        || netMode == NETMODE_DEDICATED_SERVER
    );
}


// Entity and entity component lists
#define GAME_MAX_ENTITIES 2048
//#define GAME_MAX_CLIENTS 8

#define UI_MAX_ENTITIES 512

#define GAME_DEBUG_BUFFER_LENGTH 1024

#define PLAYER_MOVE_SPEED 6.5f
#define PLAYER_ROTATION_SPEED 135.0f

#define TEST_PROJECTILE_SPEED 60.0f

// TODO: Remove this stuff sometime
global_variable u8 g_testAsciChar = '+';
global_variable char* g_testString = "Hello, World!...\nHey look, new line!";
global_variable u8 g_input_escapePressed = 0;
global_variable u8 cycleTexturePressed = 0;

global_variable ZStringHeader g_debugStr;
global_variable char g_debugStrBuffer[GAME_DEBUG_BUFFER_LENGTH];
global_variable RendObj g_debugStrRenderer;
global_variable u8 g_verbose = 0;

void Game_InitDebugStr()
{
    g_debugStr = {};
    g_debugStr.chars = g_debugStrBuffer;
    g_debugStr.maxLength = GAME_DEBUG_BUFFER_LENGTH;
}

void Game_SetDebugStringRender()
{
    RendObj_SetAsAsciCharArray(&g_debugStrRenderer, g_debugStr.chars, g_debugStr.length, 0.05f,  AppGetTextureIndexByName("textures\\charset.bmp"), 0, 1, 1);
}

/////////////////////////////////////////////////////////////
// Render Objects Memory
/////////////////////////////////////////////////////////////
// TODO: This stuff should probably be in the app, not the game area.
// Game
global_variable RenderScene g_worldScene;
global_variable RenderListItem g_scene_renderList[GAME_MAX_ENTITIES];

global_variable RendObj g_debugLine = {};

// Weapon Model
global_variable RenderScene g_weaponModelScene;
global_variable RenderListItem g_weaponModel_renderList[UI_MAX_ENTITIES];

// UI
global_variable RenderScene g_uiScene;
global_variable RenderListItem g_ui_renderList[UI_MAX_ENTITIES];

/////////////////////////////////////////////////////////////
// Entity Memory
/////////////////////////////////////////////////////////////
// Game
#define MAX_SCENE_NAME_CHARS 64
global_variable char g_currentSceneName[MAX_SCENE_NAME_CHARS];
global_variable GameState g_gameState;
global_variable Ent g_gameEntities[GAME_MAX_ENTITIES];

global_variable Player g_players[GAME_MAX_CLIENTS];

// UI
global_variable GameState g_uiState;
global_variable Ent g_uiEntities[UI_MAX_ENTITIES];

// Game Command I/O buffer handles

// pointers used for I/O
global_variable ByteBuffer* g_appReadBuffer = NULL;
global_variable ByteBuffer* g_appWriteBuffer = NULL;

// Double buffers, swapped at the end of each app frame.
global_variable ByteBuffer g_appBufferA;
global_variable BlockRef g_appBufferA_Ref;

global_variable ByteBuffer g_appBufferB;
global_variable BlockRef g_appBufferB_Ref;

// Physics engine buffer handles
global_variable BlockRef g_collisionCommandBuffer;
global_variable BlockRef g_collisionEventBuffer;

// Client input
global_variable ClientTick g_localClientTick;


/////////////////////////////////////////////////////////////
// Entity Components Memory
/////////////////////////////////////////////////////////////
// TODO: This stuff should be allocated on the heap so it can be scaled

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
#include "game_server.h"
#include "game.cpp"
