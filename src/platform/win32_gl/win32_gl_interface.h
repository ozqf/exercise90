#pragma once

#include <windows.h>
#include "../../common/com_module.h"

#define RENDERER_EXPORT_FUNC_NAME LinkToRenderer

struct RenderInterface
{
    i32 (*R_Init)(HWND window);
    i32 (*R_Shutdown)();

    void (*R_BindTexture)(void* rgbaPixels, u32 width, u32 height, u32 textureIndex);

    ScreenInfo (*R_SetupFrame)(HWND window);
    void (*R_FinishFrame)(HWND window);
    void (*R_RenderScene)(RenderScene* scene, PlatformTime* time);
};

struct RendererPlatform
{
    void (*Log)(char* msg);
    void (*Print)(char* msg);
};

typedef RenderInterface (Func_LinkToRenderer)(RendererPlatform platform);
