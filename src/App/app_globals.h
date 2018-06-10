#pragma once

#include "app_module.cpp"

///////////////////////////////////////////////////////////////////////////////
// DLL GLOBALS
///////////////////////////////////////////////////////////////////////////////

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
global_variable Transform g_debugCameraTransform = {};
global_variable Vec3 g_debugCameraDegrees = {};
// 0 = never, 1 = in debug camera mode, 2 = always
global_variable u8 g_debugColliders = 1;

global_variable u8 g_paused = 0;
