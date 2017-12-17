#ifndef PLATFORM_INTERFACE_H
#define PLATFORM_INTERFACE_H

#include "../Shared/shared.h"

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

#endif