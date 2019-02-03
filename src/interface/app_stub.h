#pragma once

#include "../common/com_defines.h"
#include "app_interface.h"
#include "platform_interface.h"

#include <stdio.h>
#if 0
/*****************************************************
Empty, default App implementation
*****************************************************/

PlatformInterface platformStub;

// struct AppStubState
// {

// };

//AppStubState appStubState;

i32 AppInit_Stub() { return 1; }
i32 AppShutdown_Stub() { printf("APP STUB Shutdown App Stub\n"); return 1; }
void AppUpdate_Stub(GameTime* time) { }

// Fill out an app stub
AppInterface GetAppInterfaceStub(PlatformInterface platInterface)
{
    printf("APP STUB Link to App Stub\n");
    platformStub = platInterface;

    AppInterface app = { };
    app.isValid = true;
    app.AppInit = AppInit_Stub;
    app.AppShutdown = AppShutdown_Stub;
    app.AppUpdate = AppUpdate_Stub;
    //app.AppFixedUpdate = AppFixedUpdate_Stub;
    return app;
}
#endif
