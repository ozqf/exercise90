#pragma once

#include "../common/com_module.h"

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "app_types.h"


/////////////////////////////////////////////////////
// App functions that game can access
/////////////////////////////////////////////////////
void App_ErrorStop();

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

//global_variable ClientTick g_ClientTick = {};

#define MAX_INPUT_ITEMS 32
global_variable InputAction g_inputActions[MAX_INPUT_ITEMS];
global_variable u32 g_numInputActions = 0;

// Interface to the external world
PlatformInterface platform;

// Heap + Preallocated lists
global_variable Heap g_heap;

// Input
global_variable ZMouseMode g_mouseMode;

// Assets
//global_variable i32 g_nextTextureIndex = 0;
global_variable TextureHandles g_textureHandles;
//global_variable TextureHandles g_textures;

// Debugging
global_variable BlockRef g_debugBuffer;
