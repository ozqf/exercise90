#pragma once

#include "../Shared/shared.h"

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "app_types.h"

///////////////////////////////////////////////////////////////////////////////
// DLL GLOBALS
///////////////////////////////////////////////////////////////////////////////

// Interface to the external world
PlatformInterface platform;

// Render Scene
#define R_MAX_RENDER_OBJECTS 2048

global_variable RendObj g_rendObjects[R_MAX_RENDER_OBJECTS];
global_variable RenderScene g_scene;

global_variable GL_Test_Input testInput;
