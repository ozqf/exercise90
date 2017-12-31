#pragma once

#include "../Shared/shared.h"
#include "platform_interface.h"
#include <stdio.h>

// Contains pointers to App functions
struct AppInterface
{
    i32 isvalid;
    void (*AppInit)();
    void (*AppShutdown)();
    void (*AppUpdate)(GameTime* time, InputTick* tick);
    void (*AppFixedUpdate)(GameTime* time, InputTick* tick);
};

/*****************************************************
Platform calls DLL with platform function pointers, DLL should return pointers
*****************************************************/
typedef AppInterface (Func_LinkToApp)(PlatformInterface);
