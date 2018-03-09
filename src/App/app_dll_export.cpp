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
    printf("LinkToApp Library Built on %s at %s\n", __DATE__, __TIME__);
    platform = platInterface;

    AppInterface app;
    app.isvalid = true;
    app.AppInit = AppInit;
    app.AppShutdown = AppShutdown;
    app.AppFixedUpdate = AppFixedFrame;
    app.AppUpdate = AppFrame;

    return app;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    printf("DLL Main\n");
	return 1;
}
