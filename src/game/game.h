#pragma once

#include "../Shared/shared.h"
#include "../Shared/Collision/shared_collision.h"

#include "game_types.h"

// Entity and entity component lists
#define WORLD_MAX_ENTITIES 2048
#define R_MAX_RENDER_OBJECTS 2048

// Game state
global_variable RenderScene g_scene;

global_variable Ent g_entities[WORLD_MAX_ENTITIES];
global_variable i32 g_num_entities;

global_variable EntComp_TestBrain g_game_brains[WORLD_MAX_ENTITIES];
global_variable i32 g_num_brains;

global_variable ZCollider g_game_colliders[WORLD_MAX_ENTITIES];
global_variable i32 g_num_colliders;

global_variable RendObj g_game_rendObjects[R_MAX_RENDER_OBJECTS];
global_variable u32 g_numSceneRendComponents;
global_variable RendObj g_ui_rendObjects[R_MAX_RENDER_OBJECTS];
global_variable u32 g_numUIRendComponents;

global_variable RenderListItem g_scene_renderList[R_MAX_RENDER_OBJECTS];
global_variable RenderListItem g_ui_renderList[R_MAX_RENDER_OBJECTS];

#include "game.cpp"
#include "game_ent_components.cpp"
