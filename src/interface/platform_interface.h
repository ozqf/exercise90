#pragma once

#include "../Shared/shared.h"

#include "../Shared/Memory/Heap.h"

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
    // Memory
    u8 (*Platform_Malloc)(MemoryBlock* mem, u32 bytesToAllocate);
    void (*Platform_Free)(MemoryBlock *mem);
    BlockRef (*Platform_LoadFileIntoHeap)(Heap* heap, char* fileName);
    i32 (*Platform_LoadDebugTextures)(Heap* heap);

    // Input
    void (*Platform_SetMouseMode)(enum ZMouseMode mode);

    // Rendering
    void (*Platform_RenderScene)(RenderScene* scene);
};

// struct PlatformInterface2
// {
//     bool32 (*PlatformWriteEntireFile)(char *fileName, u32 memorySize, void *memory);
//     void* (*PlatformReadEntireFile)(char *fileName);
//     void (*PlatformAlloc)(MemoryBlock *mem);
//     void (*PlatformFree)(MemoryBlock *mem);
// };

/*****************************************************
Empty, default Platform interface
*****************************************************/

void PlatformRenderScene_Stub(RenderScene* scene) { }

void GetPlatformInterfaceStub(PlatformInterface *platInterface)
{
    platInterface->Platform_RenderScene = PlatformRenderScene_Stub;
}
