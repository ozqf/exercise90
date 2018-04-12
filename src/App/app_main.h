#pragma once

#include "../Shared/shared.h"
#include "../Shared/Memory/Heap.h"

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "app_types.h"
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

// Interface to the external world
PlatformInterface platform;

// Heap + Preallocated lists
global_variable Heap g_heap;

//global_variable GameState g_world;

// Input
global_variable ZMouseMode g_mouseMode;
global_variable GL_Test_Input testInput;

// Assets
//global_variable i32 g_nextTextureIndex = 0;
global_variable TextureHandles g_textureHandles;
//global_variable TextureHandles g_textures;

// Debugging
global_variable BlockRef g_debugBuffer;
