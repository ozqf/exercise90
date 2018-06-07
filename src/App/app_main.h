#pragma once

#include "../common/com_module.h"

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "app_types.h"

// TODO: Hack to get it to build
struct Cmd_ClientUpdate;

/////////////////////////////////////////////////////
// App functions that game can access
/////////////////////////////////////////////////////
void App_ErrorStop();
Client* App_FindClientById(i32 id);
void Exec_UpdateClient(Cmd_ClientUpdate* cmd);


/////////////////////////////////////////////////////
// Game code
/////////////////////////////////////////////////////

#include "Game/game.h"

#define MAX_TEXTURES 128

struct TextureHandles
{
    Texture2DHeader textureHeaders[MAX_TEXTURES];
    BlockRef blockRefs[MAX_TEXTURES]; // IF allocated on the heap, find the reference here
    u32 numTextures = 0;
    u32 maxTextures = MAX_TEXTURES;
};

///////////////////////////////////////////////////////////////////////////////
// DLL GLOBALS
///////////////////////////////////////////////////////////////////////////////

#define MAX_INPUT_ITEMS 32
global_variable InputAction g_inputActionItems[MAX_INPUT_ITEMS];
//global_variable i32 g_numInputActions = 0;

global_variable InputActionSet g_inputActions = {
    g_inputActionItems,
    0
};

global_variable Client g_clients[GAME_MAX_CLIENTS];
global_variable ClientList g_clientList =
{
    g_clients,
    0,
    GAME_MAX_CLIENTS
};

// id of the client this exe represents
global_variable i32 g_localClientId = 0;

// Interface to the external world
PlatformInterface platform;

// Heap + Preallocated lists
global_variable Heap g_heap;

// Input
global_variable ZMouseMode g_mouseMode;
global_variable GameTime g_time = {};

// Assets
//global_variable i32 g_nextTextureIndex = 0;
global_variable TextureHandles g_textureHandles;
//global_variable TextureHandles g_textures;

// Debugging
global_variable BlockRef g_debugBuffer;
// Ignore local player control input and activate debug fly-around camera
global_variable u8 g_debugCameraOn = 0;
