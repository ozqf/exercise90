#pragma once

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

/***************************************
* Export Windows DLL functions
***************************************/
#include <Windows.h>

extern "C"
AppInterface __declspec(dllexport) LinkToApp(PlatformInterface platInterface)
{
    printf("APP: Library Built on %s at %s\n", __DATE__, __TIME__);
    platform = platInterface;

    AppInterface app;
    app.isvalid = true;
    app.AppInit = App_Init;
    app.AppShutdown = App_Shutdown;
    app.AppRendererReloaded = AppRendererReloaded;
    //app.AppFixedUpdate = App_FixedFrame;
    app.AppUpdate = App_Frame;
    app.AppParseCommandString = App_ParseCommandString;
    return app;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    printf("APP DLL Main\n");
	return 1;
}
