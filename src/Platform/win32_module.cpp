#ifndef WIN32_MODULE_CPP
#define WIN32_MODULE_CPP

/**********************************************************************
 * Core implementation of Windows platform layer
 *********************************************************************/

#include <stdio.h>

void Win32_Shutdown();

#include "../common/com_module.h"

#include "win32_types.h"
#include "win32_globals.h"

#include "win32_timing.h"
#include "win32_input.h"
#include "win32_draw.h"
#include "win32_debug.h"
#include "win32_data.h"
#include "win32_fileIO.h"
#include "win32_gl/win32_gl_interface.h"
#include "win32_commands.h"

// Last as it probably calls into the others
#include "win32_app_interface.h"
#include "win32_main.h"

#endif