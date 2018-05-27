#pragma once

#include "../common/com_defines.h"
#include "app_interface.h"
#include "platform_interface.h"

#include <stdio.h>

/*****************************************************
Empty, default App implementation
*****************************************************/

PlatformInterface platformStub;

// struct AppStubState
// {

// };

//AppStubState appStubState;

i32 AppInit_Stub() { return 1; }
i32 AppShutdown_Stub() { printf("Shutdown App Stub\n"); return 1; }
void AppUpdate_Stub(GameTime* time, ByteBuffer commands) { }

// TODO: This doesn't work... lawl
//void AppFixedUpdate_Stub(GameTime* time, ClientTick* tick) { }

// Fill out an app stub
AppInterface GetAppInterfaceStub(PlatformInterface platInterface)
{
    printf("Link to App Stub\n");
    platformStub = platInterface;

    AppInterface app = { };
    app.isvalid = true;
    app.AppInit = AppInit_Stub;
    app.AppShutdown = AppShutdown_Stub;
    app.AppUpdate = AppUpdate_Stub;
    //app.AppFixedUpdate = AppFixedUpdate_Stub;
    return app;
}
