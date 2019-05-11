#pragma once

#include <windows.h>
#include "../../common/common.h"
#include "../../interface/renderer_interface.h"

#define RENDERER_EXPORT_FUNC_NAME LinkToRenderer

/////////////////////////////////////////////
// Model interfaces
/////////////////////////////////////////////
struct RenderInterface
{
    i32 (*R_Init)(HWND window);
    i32 (*R_Shutdown)();

    void (*R_BindTexture)(void* rgbaPixels, u32 width, u32 height, u32 textureIndex);
	
	// TODO: Replace this with command buffer
	// Old render path
    ScreenInfo (*R_SetupFrame)();
    void (*R_FinishFrame)();
    void (*R_RenderScene)(RenderScene* scene, PlatformTime* time);
	
	// New render path
	void (*R_Execute)(RenderCommand* commands, i32 numCommands, PlatformTime* time);
	ScreenInfo (*GetScreenInfo)();
};

struct RendererPlatform
{
    void (*Log)         (char* msg);
    void (*Print)       (char* msg);
    void (*Error)       (char* msg, char* title);
    u8   (*Malloc)      (MemoryBlock* mem, u32 bytesToAllocate);
    void (*Free)        (MemoryBlock *mem);
    i64  (*SampleClock) ();
};

typedef RenderInterface (Func_LinkToRenderer)(RendererPlatform platform);
