#pragma once

#include "../../common/com_module.h"
#include "win32_gl_main.h"

extern "C"
RenderInterface __declspec(dllexport) LinkToRenderer()
{
    RenderInterface r;
    
    r.R_Init = Win32_InitOpenGL;
    r.R_Shutdown = Win32_R_Shutdown;
    
    r.R_BindTexture = Win32_Platform_R_BindTexture;

    r.R_SetupFrame = Win32_R_SetupFrame;
    r.R_FinishFrame = Win32_R_FinishFrame;
    r.R_RenderScene = Win32_RenderFrame;

    return r;
}