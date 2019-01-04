#pragma once
#ifndef APP_MODULE_H
#define APP_MODULE_H

// TODO: STILL USING WINDOWS INCLUDE FOR DEBUGGING. NOT PLATFORM AGNOSTIC!
#include "../Platform/win32_system_include.h"
#pragma message(">> APP Compiling with windows <<")
#include <stdio.h>

#include "../common/com_module.cpp"

/////////////////////////////////////////////////////////
// Game Layer
/////////////////////////////////////////////////////////
#include "Game/game.h"
struct NetStream;
#include "Game/game_types.cpp"
#include "game/game_command_types.cpp"
#include "app_module.h"


/////////////////////////////////////////////////////////
// Implementations
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// Game Layer
/////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////
// Engine Layer
/////////////////////////////////////////////////////////
#include "../network/znet_interface.h"

// Vars
#include "app_globals.h"

// implementation
#include "app_debug.cpp"
#include "app_menu.cpp"
#include "app_session.cpp"
#include "app_clients.cpp"
#include "app_network_server.cpp"
#include "app_network.cpp"
#include "app_commands.cpp"
#include "app_input.cpp"
#include "assets/app_textures.cpp"
#include "assets/app_meshes.cpp"
#include "app_main.cpp"

/***************************************
* Export DLL functions
***************************************/
#include "app_dll_export.cpp"

#endif