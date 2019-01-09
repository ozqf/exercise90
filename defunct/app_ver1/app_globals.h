#pragma once

#include "app_module.h"

///////////////////////////////////////////////////////////////////////////////
// DLL GLOBALS
///////////////////////////////////////////////////////////////////////////////
#define MAX_VARS 256
Var g_vars[MAX_VARS];
i32 g_nextVar = 0;

global_variable InputAction g_inputActionItems[MAX_INPUT_ITEMS];

global_variable InputActionSet g_inputActions = {
    g_inputActionItems,
    0
};


global_variable ZStringHeader g_debugStr;
global_variable char g_debugStrBuffer[GAME_DEBUG_BUFFER_LENGTH];
global_variable RendObj g_debugStrRenderer;
global_variable u8 g_verbose = 0;


// Interface to the external world
PlatformInterface platform;

// Heap + Preallocated lists
global_variable Heap g_heap;

// Input
global_variable ZMouseMode g_mouseMode;
global_variable GameTime g_time = {};
GameTime* GetAppTime() { return &g_time; }

// Assets
global_variable TextureHandles g_textureHandles;
global_variable MeshHandles g_meshHandles;

// Menu items
global_variable UIEntity g_menu_entities[APP_MAX_MENU_ITEMS];

// Menu Rendering
global_variable ScreenInfo g_screenInfo;
global_variable RenderScene g_menuScene;
global_variable RenderListItem g_menuRenderList[APP_MAX_MENU_ITEMS];

// Debugging
global_variable BlockRef g_debugBuffer;
// Ignore local player control input and activate debug fly-around camera
global_variable u8 g_debugCameraOn = 0;
global_variable Transform g_debugCameraTransform = {};
global_variable Vec3 g_debugCameraDegrees = {};
// 0 = never, 1 = in debug camera mode, 2 = always
global_variable u8 g_debugColliders = 1;

global_variable u8 g_paused = 0;
global_variable u8 g_minimised = 0;
global_variable u8 g_menuOn = 1;

global_variable ReplayMode g_replayMode = NoReplayMode;
global_variable i32 g_replayFileId = -1;
global_variable StateSaveHeader g_replayHeader = {};
global_variable BlockRef g_replayData;
global_variable ByteBuffer g_replayReadBuffer = {};
global_variable u8* g_replayPtr = NULL;

// Game Command I/O buffer handles

// pointers used for I/O
global_variable ByteBuffer* g_appReadBuffer = NULL;
global_variable ByteBuffer* g_appWriteBuffer = NULL;
ByteBuffer* App_GetWriteBuffer() { return g_appWriteBuffer; }

// Double buffers, swapped at the end of each app frame.
global_variable ByteBuffer g_appBufferA;
global_variable BlockRef g_appBufferA_Ref;

global_variable ByteBuffer g_appBufferB;
global_variable BlockRef g_appBufferB_Ref;


global_variable NetStream g_serverStream;
NetStream*      App_GetRemoteServerStream() { return &g_serverStream; }

global_variable BlockRef g_serverStreamInputRef;
global_variable BlockRef g_serverStreamOutputRef;
global_variable ByteBuffer g_serverStreamInput;
global_variable ByteBuffer g_serverStreamOutput;

// Physics engine buffer handles
global_variable BlockRef g_collisionCommandBuffer;
global_variable BlockRef g_collisionEventBuffer;

#define APP_STATE_OP_NONE 0
#define APP_STATE_OP_SAVE 1
#define APP_STATE_OP_LOAD 2
#define APP_STATE_OP_RECORD 3
#define APP_STATE_OP_PLAY 4
#define APP_STATE_OP_JOIN 5
#define APP_STATE_OP_HOST 6
#define APP_STATE_OP_EMPTY 7

struct AppStateOperation
{
	i32 op;
	char fileName[64];
} g_appStateOperation;


/////////////////////////////////////////////////////////////
// Render Objects Memory
/////////////////////////////////////////////////////////////
// World scene
global_variable RenderScene g_worldScene;
global_variable RenderListItem g_scene_renderList[GAME_MAX_ENTITIES];

// Weapon Model
global_variable RenderScene g_weaponModelScene;
global_variable RenderListItem g_weaponModel_renderList[UI_MAX_ENTITIES];

// UI
global_variable RenderScene g_uiScene;
global_variable RenderListItem g_ui_renderList[UI_MAX_ENTITIES];
