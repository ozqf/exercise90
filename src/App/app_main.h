#pragma once

#include "../Shared/shared.h"
#include "../Shared/Memory/Heap.h"

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "app_types.h"
#include "../Game/game.h"

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
global_variable i32 g_numDebugTextures;

// Debugging
global_variable BlockRef g_debugBuffer;
