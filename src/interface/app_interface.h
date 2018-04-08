#pragma once

#include "../Shared/shared.h"
#include "platform_interface.h"
#include <stdio.h>

// Contains pointers to App functions
struct AppInterface
{
    i32 isvalid;
    i32 (*AppInit)();
    i32 (*AppShutdown)();
    i32 (*AppRendererReloaded)();
    void (*AppUpdate)(GameTime* time, InputTick* tick);
    //void (*AppFixedUpdate)(GameTime* time, InputTick* tick);
};

/*****************************************************
Platform calls DLL with platform function pointers, DLL should return pointers
*****************************************************/
typedef AppInterface (Func_LinkToApp)(PlatformInterface);

// eg. in SDL:
// Func_LinkToApp* linkToApp = (Func_LinkToApp *)SDL_LoadFunction(gameModule, "LinkToApp");
