#pragma once
#ifndef APP_MODULE_H
#define APP_MODULE_H

// TODO: STILL USING WINDOWS INCLUDE FOR DEBUGGING. NOT PLATFORM AGNOSTIC!
#include "../Platform/win32_system_include.h"
#pragma message(">> APP Compiling with windows <<")
#include <stdio.h>

#include "../common/com_module.cpp"

// Types/Defines etc
#include "app_defines.h"
#include "app_types.h"

#include "assets/app_testTextures.cpp"

// TODO: Hack to get it to build
// Current client spawning behaviour
// needs this Cmd to be available to the game via Exec_UpdateClient.
// Messy!
struct Cmd_ClientUpdate;
struct Client;
struct ClientList;
struct ActorInput;
struct UIEntity;
union EntId;
global_variable RendObj g_debugLine = {};

/////////////////////////////////////////////////////
// App functions that game can access
/////////////////////////////////////////////////////

void App_FatalError(char* message, char* heading);

//#define APP_ASSERT(expression, msg, heading) if(!(expression)) { App_FatalError(msg, heading); }

#define APP_ASSERT(expression, msg) if(!(expression)) \
{ \
    char assertBuf[512]; \
    sprintf_s(assertBuf, 512, "%s, %d: %s\n", __FILE__, __LINE__, msg); \
    App_FatalError(assertBuf, "Fatal error"); \
} \

// cmd writing
//void App_WriteGameCmd(u8* ptr, u8 type, u16 size);
char* App_GetBufferName(u8* bufferStartAddr);

inline u8* App_StartCommandStream();
inline u32 App_WriteCommandBytesToFrameOutput(u8* stream, u32 numBytes);
inline void App_FinishCommandStream(u8* ptr, u8 cmdType, u8 cmdFlags, u16 cmdSize);

GameTime* GetAppTime();

UIEntity* UI_GetFreeEntity(UIEntity* list, i32 max);

// Clients
void Exec_UpdateClient(Cmd_ClientUpdate* cmd);
void App_EndAllClients(ClientList* cls);
void App_ClearClientGameLinks(ClientList* cls);
void App_AddLocalClient();
void App_UpdateLocalClient(Client* cl, InputActionSet* actions, u32 frameNumber);
i32 App_GetNextClientId(ClientList* cls);
Client* App_FindClientById(i32 id, ClientList* cls);
Client* App_FindClientByEntId(EntId entId, ClientList* cls);
Client* App_FindOrCreateClient(i32 id, ClientList* cls);
Client* App_FindLocalClient(ClientList* cls, u8 checkIfPlaying);
void App_UpdateLocalClients(GameTime* time, ClientList* cls);
i32 App_NumPlayingClients(ClientList* cls);

// assets and data
internal i32 AppGetTextureIndexByName(char* textureName);
MeshData Assets_GetMeshDataByName(char* name);

i32 App_GetVarAsI32(char* name);
inline Var* App_GetVar(char* name);

/////////////////////////////////////////////////////////
// Game Layer
/////////////////////////////////////////////////////////
#include "Game/game.h"


/////////////////////////////////////////////////////////
// App internals not visible to game

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"
#include "app_globals.h"

/////////////////////////////////////////////////////////
// commands

// Buffers
void App_ClearIOBuffers();
void App_SendToServer(u8* ptr, u8 type, u16 size);
u8 App_ParseCommandString(char* str, char** tokens, i32 numTokens);
void App_ReadStateBuffer(GameState *gs, ByteBuffer *buf);
u8 App_LoadStateFromFile(GameState *gs, char *fileName);
i32 App_WriteStateToFile(char* fileName, u8 closeFileAfterWrite, StateSaveHeader* header);
void App_SaveDataVariables(char* fileName);

// Execution
void Exec_UpdateClient(Cmd_ClientUpdate* cmd);
void Exec_ReadInput(u32 frameNumber, InputEvent ev);
void App_ReadCommand(u32 type, u32 bytes, u8 *ptrRead);
void App_ReadCommandBuffer(ByteBuffer* commands);
i32 App_StartSession(u8 netMode, char* path);

// Debug
void App_DumpHeap();

/////////////////////////////////////////////////////////
// input
void Input_SetMouseMode(ZMouseMode mode);
void Input_ToggleMouseMode();
void App_InitInput(InputActionSet* actions);

/////////////////////////////////////////////////////////
// textures
internal void AppRegisterTexture(Texture2DHeader *header, BlockRef *ref);
internal void AppBindTexture(Texture2DHeader *header);
internal BlockRef AppLoadTexture(char *filePath);
//void AppLoadAndRegisterTexture(char *filePath);
internal i32 AppLoadAndBindTexture(char* filePath);
internal void AppBindAllTextures();
internal void AppLoadTestTextures();
internal i32 AppRendererReloaded();
internal void AppListTextures();

/////////////////////////////////////////////////////////
// Implementations
/////////////////////////////////////////////////////////

#include "../network/znet_interface.h"
#include "app_debug.h"
#include "app_ui.h"
#include "app_menu.h"
#include "app_session.h"
#include "app_clients.h"
#include "app_commands.h"
#include "app_input.h"
#include "network/app_network.h"
#include "assets/app_textures.cpp"
#include "assets/app_meshes.cpp"
#include "app_main.cpp"

/***************************************
* Export DLL functions
***************************************/
#include "app_dll_export.cpp"

#endif