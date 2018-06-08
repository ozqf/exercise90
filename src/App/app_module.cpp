#pragma once

// TODO: STILL USING WINDOWS INCLUDE FOR DEBUGGING. NOT PLATFORM AGNOSTIC!
#include "../Platform/win32_system_include.h"
#include <stdio.h>

#include "app_main.h"
#include "../common/com_module.h"

#include "app_testTextures.h"

#include <stdio.h>


void App_ReadCommandBuffer(ByteBuffer commands);
void App_EnqueueCmd(u8* ptr, u32 type, u32 size);
i32 App_StartSession(u8 netMode, char* path);

/////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////
// Clients
void App_EndAllClients();
void App_ClearClientGameLinks();
void App_AddLocalClient();
void App_UpdateLocalClient(Client* cl, InputActionSet* actions, u32 frameNumber);
Client* App_FindClientById(i32 id);
Client* App_FindOrCreateClient(i32 id);
void App_UpdateLocalClients(GameTime* time);

// commands
void App_SendToServer(u8* ptr, u32 type, u32 size);
void App_DumpHeap();
u8 App_ParseCommandString(char* str, char** tokens, i32 numTokens);
void App_EnqueueCmd(u8* ptr, u32 type, u32 size);
void App_ReadStateBuffer(GameState *gs, ByteBuffer *buf);
u8 App_LoadStateFromFile(GameState *gs, char *fileName);
void Exec_UpdateClient(Cmd_ClientUpdate* cmd);
void Exec_ReadInput(u32 frameNumber, InputEvent ev);
void App_ReadCommand(u32 type, u32 bytes, u8 *ptrRead);
void App_ReadCommandBuffer(ByteBuffer commands);

// input
void Input_SetMouseMode(ZMouseMode mode);
void Input_ToggleMouseMode();
void App_InitInput(InputActionSet* actions);

// textures
void AppRegisterTexture(Texture2DHeader *header, BlockRef *ref);
void AppBindTexture(Texture2DHeader *header);
BlockRef AppLoadTexture(char *filePath);
void AppLoadAndRegisterTexture(char *filePath);
void AppBindAllTextures();
void AppLoadTestTextures();
i32 AppRendererReloaded();

/////////////////////////////////////////////////////////
// Implementations
/////////////////////////////////////////////////////////
#include "app_clients.h"
#include "app_commands.h"
#include "app_input.h"
#include "app_textures.h"
#include "app_main.cpp"
