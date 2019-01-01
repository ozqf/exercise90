#pragma once
#ifndef APP_MODULE_H
#define APP_MODULE_H

// TODO: STILL USING WINDOWS INCLUDE FOR DEBUGGING. NOT PLATFORM AGNOSTIC!
#include "../Platform/win32_system_include.h"
#pragma message(">> APP Compiling with windows <<")
#include <stdio.h>

#include "../common/com_module.cpp"


#include "app_module.h"

/////////////////////////////////////////////////////////
// Implementations
/////////////////////////////////////////////////////////

#include "../network/znet_interface.h"
#include "app_debug.h"
#include "app_ui.h"
#include "app_menu.h"
#include "app_session.h"
#include "app_clients.h"
#include "app_network_server.h"
#include "app_network.cpp"
#include "app_commands.h"
#include "app_input.h"
#include "assets/app_textures.cpp"
#include "assets/app_meshes.cpp"
#include "app_main.cpp"

/***************************************
* Export DLL functions
***************************************/
#include "app_dll_export.cpp"

#endif