#pragma once

#include "app_module.cpp"

///////////////////////////////////////////////////////////////////////////////
// DLL GLOBALS
///////////////////////////////////////////////////////////////////////////////
#define MAX_VARS 256
Var g_vars[MAX_VARS];
i32 g_nextVar = 0;

global_variable InputAction g_inputActionItems[MAX_INPUT_ITEMS];
//global_variable i32 g_numInputActions = 0;

global_variable InputActionSet g_inputActions = {
    g_inputActionItems,
    0
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
inline GameTime* GetAppTime() { return &g_time; }

// Assets
//global_variable i32 g_nextTextureIndex = 0;
global_variable TextureHandles g_textureHandles;
//global_variable TextureHandles g_textures;

// Menu Rendering
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
global_variable u8 g_menuOn = 1;

global_variable ReplayMode g_replayMode = NoReplayMode;
global_variable i32 g_replayFileId = -1;
global_variable StateSaveHeader g_replayHeader = {};
global_variable BlockRef g_replayData;
global_variable ByteBuffer g_replayReadBuffer = {};
global_variable u8* g_replayPtr = NULL;
