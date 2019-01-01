#pragma once

#include "game.h"

// TODO: Remove this stuff sometime
global_variable u8 g_testAsciChar = '+';
global_variable char* g_testString = "Hello, World!...\nHey look, new line!";
global_variable u8 g_input_escapePressed = 0;
global_variable u8 cycleTexturePressed = 0;

/////////////////////////////////////////////////////////////
// Game State Memory
/////////////////////////////////////////////////////////////
// Game
global_variable char g_currentSceneName[MAX_SCENE_NAME_CHARS];
global_variable GameSession g_session;
global_variable GameScene g_gameScene;

/////////////////////////////////////////////////////////////
// Entity + Entity Components Memory
/////////////////////////////////////////////////////////////
// TODO: This stuff should be allocated on the heap so it can be scaled,
// and not globally accessible
// Bah, to heck with it. Statics for everyone!!!

global_variable Client g_clients[GAME_MAX_CLIENTS];

// client N linksStart = (maxEnts * N) and end at (linksStart + (maxEnts - 1))
global_variable EntityLink g_clientEntityLinks[GAME_MAX_ENTITIES * GAME_MAX_CLIENTS];
// eg 2048 ents, 8 clients == 16384
// link struct is 22 Bytes.
// total array: 16384 * 22 == 360,448 (352 KB, 0.34375 MB)

// Entity Components
// unlike everywhere else involving this ECS, this bit ISN'T order dependent (harhar)
global_variable Ent                 g_gameEntities[GAME_MAX_ENTITIES];
global_variable EC_Transform        g_transforms[GAME_MAX_ENTITIES];
global_variable EC_SingleRendObj    g_renderers[GAME_MAX_ENTITIES];
global_variable EC_Collider         g_colliders[GAME_MAX_ENTITIES];
global_variable EC_ActorMotor       g_actorMotors[GAME_MAX_ENTITIES];
global_variable EC_AIController     g_aiControllers[GAME_MAX_ENTITIES];
global_variable EC_Projectile       g_prjControllers[GAME_MAX_ENTITIES];
global_variable EC_Label            g_entLabels[GAME_MAX_ENTITIES];
global_variable EC_Health           g_health[GAME_MAX_ENTITIES];
global_variable EC_Thinker          g_thinkers[GAME_MAX_ENTITIES];
global_variable EC_MultiRendObj     g_multiRenderers[GAME_MAX_ENTITIES];
global_variable EC_Volume           g_volumes[GAME_MAX_ENTITIES];
global_variable EC_Sensor           g_sensors[GAME_MAX_ENTITIES];

// UI
global_variable UIEntity            g_ui_entities[UI_MAX_ENTITIES];
#define PLAYER_STATUS_TEXT_LENGTH 256
global_variable char                g_playerStatusText[PLAYER_STATUS_TEXT_LENGTH];
#define HUD_CENTRE_TEXT_LENGTH 256
global_variable char                g_hud_centreText[HUD_CENTRE_TEXT_LENGTH];

#define HUD_MAX_RING_ITEMS 16
global_variable HudRingItem g_hudRingItems[HUD_MAX_RING_ITEMS];


// GFX
global_variable LocalEnt            g_localEntities[GAME_MAX_LOCAL_ENTITIES];

/////////////////////////////////////////////////////////////
// Debug
/////////////////////////////////////////////////////////////
Transform g_debugTransform = {};

/////////////////////////////////////////////////////////////
// GLOBAL GETTERS
// Give up:
// Really not sure how to arrange architecture right now...
/////////////////////////////////////////////////////////////
char* GetCurrentSceneName() { return g_currentSceneName; }
GameSession* GetSession() { return &g_session; }
GameScene* GetScene() { return &g_gameScene; }
ClientList* GetClientList() { return &g_session.clientList; }
UIEntity* GetUIEntities() { return g_ui_entities; }
