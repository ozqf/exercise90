#pragma once

#include "../common/com_defines.h"

struct PlatformEventHeader
{
    i32 type;
    i32 size;
};

struct PlatformInterface
{
    // Oops
    void (*Platform_Error)(char* msg, char* title);

    // Memory Allocation
    u8 (*Platform_Malloc)(MemoryBlock* mem, u32 bytesToAllocate);
    void (*Platform_Free)(MemoryBlock *mem);
    
    // Loading assets
    //i32 (*Platform_LoadDebugTextures)(Heap* heap);
    u8 (*Platform_LoadFileIntoHeap)(Heap* heap, BlockRef* destRef, char* fileName, u8 assertOnFailure);
    void (*Platform_LoadTexture)(Heap* heap, BlockRef* destRef, char* path);
    void (*Platform_BindTexture)(void* rgbaPixels, u32 width, u32 height, u32 textureIndex);
    // Need to know the base directory for writing files
    i32 (*Platform_GetBaseDirectoryName)(char* buffer, i32 bufferSize);
	
	// Writing Files
	i32 (*Platform_OpenFileForWriting)(char* fileName);
    i32 (*Platform_WriteToFile)(i32 fileId, u8* ptr, u32 numBytes);
	void (*Platform_SeekInFileFromStart)(i32 fileId, u32 offset);
    i32 (*Platform_CloseFileForWriting)(i32 fileId);
    void (*Platform_GetDateTime)(DateTime* data);

    // Input
    void (*Platform_SetMouseMode)(enum ZMouseMode mode);

    // Commands
    void (*Platform_WriteTextCommand)(char* ptr);

    // Rendering
    void (*Platform_RenderScene)(RenderScene* scene);
    // Set the texture the texture the platform should use when renderering
    void (*Platform_SetDebugInputTextureIndex)(i32 index);

    void (*Platform_LoadSound)(u8* data, i32 numBytes);
};

/*****************************************************
Empty, default Platform interface
*****************************************************/
#if 0
void PlatformRenderScene_Stub(RenderScene* scene) { }

void GetPlatformInterfaceStub(PlatformInterface *platInterface)
{
    platInterface->Platform_RenderScene = PlatformRenderScene_Stub;
}
#endif
