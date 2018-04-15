#pragma once

#include "../../Shared/shared.h"
#include "../../Shared/Collision/shared_collision.h"

#include "game_types.h"

#include "game_physics.h"

// Entity and entity component lists
#define GAME_MAX_ENTITIES 2048

#define UI_MAX_ENTITIES 512

#define GAME_DEBUG_BUFFER_LENGTH 1024

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

// UI
global_variable GameState g_uiState;
global_variable Ent g_uiEntities[UI_MAX_ENTITIES];

/////////////////////////////////////////////////////////////
// Entity Components Memory
/////////////////////////////////////////////////////////////
// Game
global_variable EC_Collider         g_colliders[GAME_MAX_ENTITIES];
global_variable EC_Renderer         g_renderers[GAME_MAX_ENTITIES];
global_variable EC_ActorMotor       g_actorMotors[GAME_MAX_ENTITIES];
global_variable EC_AIController     g_aiControllers[GAME_MAX_ENTITIES];
global_variable EC_Projectile        g_prjControllers[GAME_MAX_ENTITIES];

// UI
global_variable EC_Renderer         g_ui_renderers[UI_MAX_ENTITIES];


#include "game_entities.h"
#include "comp_rendObj.h"
#include "comp_aiController.h"

#include "game_ent_components.cpp"
#include "game.cpp"