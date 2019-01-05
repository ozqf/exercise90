#pragma once

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

//////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////

internal PlatformInterface g_platform = {};

/***************************************
* Define functions accessible to platform
***************************************/
internal i32  g_isValid = 0;

internal i32  App_Init()
{
    printf("App Init\n");
    return COM_ERROR_NONE;
}

internal i32  App_Shutdown()
{
    printf("App Shutdown\n");
    return COM_ERROR_NONE;
}

internal i32  App_RendererReloaded()
{
    return COM_ERROR_NONE;
}

internal void App_Input(GameTime* time, ByteBuffer commands)
{
    
}

internal void App_Update(GameTime* time)
{
    
}

internal void App_Render(GameTime* time, ScreenInfo info)
{
    
}

internal u8 App_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
    return 0;
}

internal void App_ReadInputEvents(GameTime* time, ByteBuffer platformCommands)
{

}

/***************************************
* Export Windows DLL functions
***************************************/
#include <Windows.h>

extern "C"
AppInterface __declspec(dllexport) LinkToApp(PlatformInterface platInterface)
{
    printf("APP: Library Built on %s at %s\n", __DATE__, __TIME__);
    g_platform = platInterface;
    g_isValid = true;

    AppInterface app;
    app.isValid = true;
    app.AppInit = App_Init;
    app.AppShutdown = App_Shutdown;
    app.AppRendererReloaded = App_RendererReloaded;
    //app.AppFixedUpdate = App_FixedFrame;
    app.AppInput = App_ReadInputEvents;
    app.AppUpdate = App_Update;
    app.AppParseCommandString = App_ParseCommandString;
	app.AppRender = App_Render;
    return app;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    printf("APP DLL Main\n");
	return 1;
}
