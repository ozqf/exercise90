#pragma once

#include "../Shared/shared.h"

/*
Requirements app has of platform interface:
> Render Scene
	> Set camera tranform.
	> Set Projection mode (3D vs Ortho for 2D)
> Update loop
	> Variable step (rendering)
	> Fixed step (for physics/network)
> Raw Input
	> Mouse
	> Keyboard (raw keys?)
	> Gamepad...?
> Raw Sound
> File IO
> Network IO

Maintaining rendering Scene graph options:
    > Pass through display object list every frame.
        The display list is maintained on the app site.
        > more work on app side but modable
    > add/remove objects via an interface and store
        displaylist and diplay objects on the Platform
        side.
        > cleaner?
*/

struct PlatformInterface
{
    f32 (*PlatformGetViewPortMinX)();
    f32 (*PlatformGetViewPortMinY)();
    f32 (*PlatformGetViewPortMaxX)();
    f32 (*PlatformGetViewPortMaxY)();
    void (*PlatformClearScreen)();
    void (*PlatformRenderBlitItems)(BlitItem* items, i32 numItems);
};

struct PlatformInterface2
{
    bool32 (*PlatformWriteEntireFile)(char *fileName, u32 memorySize, void *memory);
    void* (*PlatformReadEntireFile)(char *fileName);
    void (*PlatformAlloc)(MemoryBlock *mem);
    void (*PlatformFree)(MemoryBlock *mem);
};

/*****************************************************
Empty, default Platform interface
*****************************************************/

f32 PlatformGetViewPortMinX_Stub() { return 0; }
f32 PlatformGetViewPortMinY_Stub() { return 0; }
f32 PlatformGetViewPortMaxX_Stub() { return 0; }
f32 PlatformGetViewPortMaxY_Stub() { return 0; }
void PlatformClearScreen_Stub() { }
void PlatformRenderBlitItems_Stub(BlitItem* items, i32 numItems) { }

void GetPlatformInterfaceStub(PlatformInterface *platInterface)
{
    platInterface->PlatformGetViewPortMinX = PlatformGetViewPortMinX_Stub;
    platInterface->PlatformGetViewPortMinY = PlatformGetViewPortMinY_Stub;
    platInterface->PlatformGetViewPortMaxX = PlatformGetViewPortMaxX_Stub;
    platInterface->PlatformGetViewPortMaxY = PlatformGetViewPortMaxY_Stub;
    platInterface->PlatformClearScreen = PlatformClearScreen_Stub;
    platInterface->PlatformRenderBlitItems = PlatformRenderBlitItems_Stub;
}
