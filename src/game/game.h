#pragma once

#include "../Shared/shared.h"
#include "../Shared/Collision/shared_collision.h"

#include "game_types.h"

// Entity and entity component lists
#define GAME_MAX_ENTITIES 2048

#define UI_MAX_ENTITIES 512

/////////////////////////////////////////////////////////////
// Render Objects Memory
/////////////////////////////////////////////////////////////
global_variable RenderScene g_worldScene;
global_variable RenderScene g_uiScene;

global_variable RenderListItem g_scene_renderList[GAME_MAX_ENTITIES];
global_variable RenderListItem g_ui_renderList[GAME_MAX_ENTITIES];

/////////////////////////////////////////////////////////////
// Entity Memory
/////////////////////////////////////////////////////////////
// Game State handles
global_variable GameState g_gameState;
global_variable GameState g_uiState;
// Game Entities
global_variable Ent g_gameEntities[GAME_MAX_ENTITIES];
//global_variable i32 g_num_entities;

// UI Entities
global_variable RendObj g_ui_rendObjects[GAME_MAX_ENTITIES];
global_variable u32 g_numUIRendComponents;

/////////////////////////////////////////////////////////////
// Entity Components Memory
/////////////////////////////////////////////////////////////
global_variable EC_Collider         g_colliders[GAME_MAX_ENTITIES];
global_variable EC_Renderer         g_renderers[GAME_MAX_ENTITIES];
global_variable EC_ActorMotor       g_actorMotors[GAME_MAX_ENTITIES];
global_variable EC_AIController     g_aiControllers[GAME_MAX_ENTITIES];


#include "game_entities.h"
#include "comp_rendObj.h"
#include "comp_aiController.h"

#include "game.cpp"
#include "game_ent_components.cpp"
