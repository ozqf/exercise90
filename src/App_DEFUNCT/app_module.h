#pragma once

#include "../common/com_module.h"

// Types/Defines etc
#include "app_defines.h"
#include "app_types.h"
#include "app_network_types.h"

#include "assets/app_testTextures.cpp"

// TODO: Hack to get it to build
// Messy!
struct InputAction;
struct InputActionSet;
struct Cmd_ClientUpdate;
struct Client;
struct ClientList;
struct GameSession;
struct ActorInput;
struct UIEntity;
union EntId;
global_variable RendObj g_debugLine = {};

/////////////////////////////////////////////////////
// App functions that game can access
/////////////////////////////////////////////////////

//#define APP_ASSERT(expression, msg, heading) if(!(expression)) { App_FatalError(msg, heading); }

// cmd writing
//void App_WriteGameCmd(u8* ptr, u8 type, u16 size);
char*               App_GetBufferName(u8* bufferStartAddr);

ByteBuffer*         App_GetWriteBuffer();
NetStream*          App_GetRemoteServerStream();

u8*                 App_StartCommandStream();
u32                 App_WriteCommandBytesToFrameOutput(u8* stream, u32 numBytes);
void                App_FinishCommandStream(u8* ptr, u8 cmdType, u8 cmdFlags, u16 cmdSize);
// Use this guy if you can:
u8*                 App_CreateCommandSpace(u8 cmdType, u8 cmdFlags, u16 cmdSize);
void                App_WriteTextCommand(char* text);

GameTime*           GetAppTime();

UIEntity*           UI_GetFreeEntity(UIEntity* list, i32 max);

i32                 Stream_WriteStreamMsgHeader(u8* ptr, u32 msgId, i32 numBytes, f32 resendTimeSeconds);
void                Stream_PrintBufferManifest(ByteBuffer* b);
internal void       Stream_Clear(NetStream* stream);
void                Net_ProcessPacketDelivery(GameSession* session, i32 connId, u32 packetNumber);
void                Net_ProcessPacket(i32 senderConnectionId, u8* bytes, u16 numBytes);

// Clients
internal void       Exec_UpdateClient(Cmd_ClientUpdate* cmd);
/*internal*/ void   App_EndAllClients(ClientList* cls);
void                App_ResetClient(Client* cl);
internal void       App_ClearClientGameLinks(ClientList* cls);
internal void       App_DeleteClients(ClientList* cls);
/*internal*/ void   App_AddLocalClient();
internal void       App_UpdateLocalClient(Client* cl, InputActionSet* actions, u32 frameNumber);
internal i32        App_GetNextClientId(ClientList* cls);
Client*             App_FindClientById(i32 id, ClientList* cls);
internal Client*    App_FindClientByConnectionId(ClientList* cls, i32 connId);
Client*             App_FindClientByEntId(EntId entId, ClientList* cls);
Client*             App_CreateClient(i32 clientId, i32 connectionId, ClientList* cls);
//internal Client*  App_FindOrCreateClient(i32 id, ClientList* cls);
Client*             App_FindLocalClient(ClientList* cls, u8 checkIfPlaying);
internal void       App_UpdateLocalClients(GameTime* time, ClientList* cls);
i32                 App_NumPlayingClients(ClientList* cls);

i32                 App_IsClientStateValid(i32 state);

// Scene control
i32                 App_LoadScene(char *path, GameSession* session, GameScene* gs);

// assets and data
i32                 AppGetTextureIndexByName(char* textureName);
MeshData            Assets_GetMeshDataByName(char* name);

i32                 App_GetVarAsI32(char* name);
Var*         App_GetVar(char* name);

internal void       Debug_AddVerboseFrames(i32 count);

/////////////////////////////////////////////////////////
// Game Layer
/////////////////////////////////////////////////////////
//#include "Game/game.h"


#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

/////////////////////////////////////////////////////////
// commands

// Buffers
void                App_ClearIOBuffers();
void                App_SendToServer(u8* ptr, u8 type, u16 size);
u8                  App_ParseCommandString(char* str, char** tokens, i32 numTokens);
void                App_ReadStateBuffer(GameSession* session, GameScene* gs, ByteBuffer *buf);
u8                  App_LoadStateFromFile(GameSession* session, GameScene *gs, char *fileName);
void                App_SaveDataVariables(char* fileName);

// Execution
void                Exec_UpdateClient(Cmd_ClientUpdate* cmd);
void                Exec_ReadInput(u32 frameNumber, InputEvent ev);
void                App_ReadCommand(u32 type, u32 bytes, u8 *ptrRead);
void                App_ReadCommandBuffer(ByteBuffer* commands);
i32                 App_StartSession(u8 netMode, char *path, GameSession* session, GameScene* gs);
i32                 App_WriteStateToFile(
                        GameSession* session, GameScene* gs,
                        char *fileName, u8 closeFileAfterWrite, StateSaveHeader *header);

// Networking
internal void       Net_WriteImpulse(GameSession* gs, i32 impulse);
internal void       Net_WriteDebug(ZStringHeader* txt, GameSession* session);

// Debug
void                App_DumpHeap();

/////////////////////////////////////////////////////////
// input
void                Input_SetMouseMode(ZMouseMode mode);
void                Input_ToggleMouseMode();
void                App_InitInput(InputActionSet* actions);

/////////////////////////////////////////////////////////
// textures
internal void       AppRegisterTexture(Texture2DHeader *header, BlockRef *ref);
internal void       AppBindTexture(Texture2DHeader *header);
internal BlockRef   AppLoadTexture(char *filePath);
internal i32        AppLoadAndBindTexture(char* filePath);
internal void       AppBindAllTextures();
internal void       AppLoadTestTextures();
internal i32        AppRendererReloaded();
internal void       AppListTextures();

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

u8 IsRunningClient(u8 netMode)
{
    if (netMode == NETMODE_NONE) { return 0; }
    return (
           netMode == NETMODE_SINGLE_PLAYER
        || netMode == NETMODE_LISTEN_SERVER
        || netMode == NETMODE_CLIENT
        || netMode == NETMODE_REPLAY
    );
}

u8 IsRunningServer(u8 netMode)
{
    if (netMode == NETMODE_NONE) { return 0; }
    return (
           netMode == NETMODE_SINGLE_PLAYER
        || netMode == NETMODE_LISTEN_SERVER
        || netMode == NETMODE_DEDICATED_SERVER
    );
}

u8 IsConnectionOpen(u8 netMode)
{
    if (netMode == NETMODE_NONE) { return 0; }
    return (
        netMode == NETMODE_CLIENT
        || netMode == NETMODE_LISTEN_SERVER
        || netMode == NETMODE_DEDICATED_SERVER
    );
}
