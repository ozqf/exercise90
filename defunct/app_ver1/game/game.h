#pragma once

#include "../../common/com_module.h"
#include "../Physics/ZPhysics_interface.h"

#include "game_types.h"
#include "game_command_types.h"

#define GAME_DEBUG_MODE_NONE 0
#define GAME_DEBUG_MODE_CAMERA 1
#define GAME_DEBUG_MODE_TRANSFORM 2
#define GAME_DEBUG_MODE_PHYSICS 3
#define GAME_DEBUG_MODE_ACTOR_INPUT 4
#define GAME_DEBUG_MODE_INPUT 5
#define GAME_DEBUG_MODE_RECORDING 6
#define GAME_DEBUG_MODE_NETWORK 7

#define ENTITY_STATUS_FREE 0
#define ENTITY_STATUS_RESERVED 1
#define ENTITY_STATUS_IN_USE 2
#define ENTITY_STATUS_DEAD 3

#define TEAM_FREE 0
#define TEAM_ENEMIES 1
#define TEAM_PLAYERS 2
#define TEAM_NON_COMBATANT 3

// Entity and entity component lists
#define ENTITY_BLOCK_SIZE 2048
#define ENTITY_BLOCK_COUNT 2
#define GAME_MAX_ENTITIES (ENTITY_BLOCK_SIZE * ENTITY_BLOCK_COUNT)
#define GAME_MAX_PLAYERS 64
#define UI_MAX_ENTITIES 512
#define GAME_MAX_LOCAL_ENTITIES 2048

#define GAME_DEBUG_BUFFER_LENGTH 1024

#define PLAYER_MOVE_SPEED 6.5f
#define PLAYER_ROTATION_SPEED 135.0f

#define TEST_PROJECTILE_SPEED 60.0f

// Interface
#define MAX_SCENE_NAME_CHARS 64
char* GetCurrentSceneName();

GameSession* GetSession();
GameScene* GetScene();
ClientList* GetClientList();
UIEntity* GetUIEntities();
void Game_DebugPrintEntities(GameScene* gs);

internal void Ent_ApplySpawnOptions(EntityState* state, EntitySpawnOptions* options);
u16 Ent_WriteStateCmdToAppBuffer(u8* optionalOutputStream, EntityState* state);
internal u8 Ent_PrepareSpawnCmd(GameScene* gs, i32 factoryType, i32 catagory, EntityState* target, EntitySpawnOptions* options);
EntId Ent_QuickSpawnCmd(GameScene* gs, i32 factoryType, i32 catagory, EntitySpawnOptions* options);
void Ent_CopyFullEntityState(GameScene* gs, Ent* ent, EntityState* state);
void GS_Clear(GameScene* gs);
void GameSession_Clear(GameSession* session);
void Game_BuildTestScene(GameScene* gs, i32 sceneIndex);
void Game_CreateClientInput(InputActionSet* actions, ActorInput* input);
Ent* Game_GetLocalClientEnt(ClientList* clients, EntList* ents);
void Game_SpawnLocalEntity(f32 x, f32 y, f32 z, Vec3 *dir, f32 power, i32 type);
Vec3 Game_RandomSpawnOffset(f32 rangeX, f32 rangeY, f32 rangeZ);
i32 Game_ReadCommandBuffer(GameSession* session, GameScene* gs, ByteBuffer* commands, u8 verbose);
void Game_Init();
void Game_InitDebugStr();
void Game_Tick(
    Game *game,
    ByteBuffer* input,
    ByteBuffer* output,
    GameTime *time,
    InputActionSet* actions);
void Game_ApplyInputFlyMode(InputActionSet* actions, Vec3* degrees, Transform* t, f32 dt, u32 frame);
void Game_DrawColliderAABBs(GameScene* gs, GameTime* time, RenderScene* scene);
void Game_IntersectionTest(GameScene* gs, RenderScene* scene);
void Game_BuildWeaponModelScene(Client* localClient, GameScene* gs, RenderScene* scene);
void Game_BuildRenderList(GameScene* gs, RenderScene* scene, GameTime* time);
void Game_UpdateUI(Client* localClient, GameScene* gs, UIEntity *ents, i32 maxEntities, GameTime *time);
void Game_SetDebugStringRender();
UIEntity* UI_GetFreeEntity(UIEntity* list, i32 max);
void UI_BuildUIRenderScene(RenderScene* scene, UIEntity* ents, i32 maxEntities);

// debugging
i32 Game_DebugWriteActiveActorInput(GameScene *gs, char *buf, i32 maxChars);

//internal u8 Game_WriteSpawnTemplate(i32 factoryType, EntityState* state, EntitySpawnOptions* options);
internal i32 Ent_CopyTemplate(i32 templateId, EntityState* target, EntitySpawnOptions* options);
