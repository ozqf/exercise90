#pragma once

#include "../../common/com_defines.h"

extern "C"
RenderInterface __declspec(dllexport) LinkToRenderer(RendererPlatform platform)
{
    g_platform = platform;
    COM_CALL_PRINT(g_platform.Log, 512, "RENDERER: Built on %s at %s\n", __DATE__, __TIME__);
    //COM_STRING(buf, 512, "RENDERER: Built on %s at %s\n", __DATE__, __TIME__);
    //g_platform.Log(buf);
    //printf("RENDERER: Built on %s at %s\n", __DATE__, __TIME__);
    RenderInterface r;
    
    r.R_Init = Win32_InitOpenGL;
    r.R_Shutdown = Win32_R_Shutdown;
    
    r.R_BindTexture = Win32_Platform_R_BindTexture;

    r.R_SetupFrame = Win32_R_SetupFrame;
    r.R_FinishFrame = Win32_R_FinishFrame;
    r.R_RenderScene = Win32_RenderFrame;
	
	r.R_Execute = R_Execute;
	r.GetScreenInfo = Win32_R_GetScreenInfo;

    return r;
}
