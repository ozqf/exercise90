#pragma once

//#include "app_module.h"
#include "../common/com_module.h"
#include "../interface/platform_interface.h"

// internal data
internal i32 Tex_LoadAndBindTexture(char *filePath);

#define MAX_TEXTURES 128
#define MAX_MESHES 128

struct TextureHandles
{
    Texture2DHeader textureHeaders[MAX_TEXTURES];
    BlockRef blockRefs[MAX_TEXTURES]; // IF allocated on the heap, find the reference here
    u32 numTextures = 0;
    u32 maxTextures = MAX_TEXTURES;
};

internal AppPlatform g_platform;
internal Heap* g_heap;
internal TextureHandles g_textureHandles;

//////////////////////////////////////////////////////
// Public functions
//////////////////////////////////////////////////////

void Tex_DebugPrint()
{
    i32 l = g_textureHandles.numTextures;
    printf("--- APP TEXTURES (%d) ---\n", l);
    for (i32 i = 0; i < l; ++i)
    {
        Texture2DHeader* h = &g_textureHandles.textureHeaders[i];
        printf("%d: \"%s\" %dx%d\n", i, h->name, h->width, h->height);
    }
}

i32 Tex_GetTextureIndexByName(char* textureName)
{
    i32 l = g_textureHandles.numTextures;
    for (i32 i = 0; i < l; ++i)
    {
        if (COM_CompareStrings(
            g_textureHandles.textureHeaders[i].name, textureName) == 0)
        {
            return i;
        }
    }
    printf("APP tex %s not found, loading\n", textureName);
    return Tex_LoadAndBindTexture(textureName);
    //return -1;
}

//////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////

/**
 * Record that a texture has been loaded onto the heap
 * > header information will be copied and updated
 * > BlockRef should be a Heap block for a Texture2DHeader!
 */
#if 1
internal void Tex_RegisterTexture(Texture2DHeader *header, BlockRef *ref)
{
    i32 index = g_textureHandles.numTextures;
    header->index = index;
    g_textureHandles.textureHeaders[index] = *header;
    if (ref != NULL)
    {
        g_textureHandles.blockRefs[index] = *ref;
    }
    else
    {
        g_textureHandles.blockRefs[index] = {};
    }
    g_textureHandles.numTextures++;
}
#endif
/**
 * Upload a texture to the GPU
 */
internal void Tex_BindTexture(Texture2DHeader *header)
{
    g_platform.BindTexture(
        header->ptrMemory,
        header->width,
        header->height,
        header->index);
    printf("APP tex %s bound to index %d\n",
        header->name, header->index);
}

/**
 * Read a texture onto the Global Heap
 */
internal BlockRef Tex_LoadTexture(char *filePath)
{
    BlockRef ref = {};
    g_platform.LoadTexture(g_heap, &ref, filePath);
    return ref;
}

/**
 * Read a texture onto the global heap and then immediately bind it
 * Returns the texture's index
 */
#if 1
internal i32 Tex_LoadAndBindTexture(char *filePath)
{
    BlockRef ref = Tex_LoadTexture(filePath);
    Heap_GetBlockMemoryAddress(g_heap, &ref);
    Texture2DHeader *header = (Texture2DHeader *)ref.ptrMemory;
    Tex_RegisterTexture(header, &ref);
    Tex_BindTexture(header);
    return header->index;
}
#endif
/**
 * Upload all registered textures
 */
internal void Tex_BindAll()
{
    i32 numTextures = g_textureHandles.numTextures;
    for (i32 i = 0; i < numTextures; ++i)
    {
        Tex_BindTexture(&g_textureHandles.textureHeaders[i]);
    }
}

void Tex_Init(Heap* heap, AppPlatform platform)
{
    g_heap = heap;
    g_platform = platform;
    BlockRef ref;
    Texture2DHeader *header;

    //AppInitTestTextures();

    // 0
    //Tex_RegisterTexture(&testBuffer, NULL);

    // 1
    //Tex_RegisterTexture(&testBuffer2, NULL);

    // 2 - you get the picture
    //Tex_RegisterTexture(&testBuffer3, NULL);

    // 3 - Kinda icky this
    ref = Tex_LoadTexture("textures\\BitmapTest.bmp");
    Heap_GetBlockMemoryAddress(g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    Tex_RegisterTexture(header, &ref);

    // 4 - so, texture management sometime?
    ref = Tex_LoadTexture("textures\\charset.bmp");
    Heap_GetBlockMemoryAddress(g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    Tex_RegisterTexture(header, &ref);

    // 5 - can't be too hard...
    ref = Tex_LoadTexture("textures\\brbrick2.bmp");
    Heap_GetBlockMemoryAddress(g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    Tex_RegisterTexture(header, &ref);

    // 6 - right?
    ref = Tex_LoadTexture("textures\\W33_5.bmp");
    Heap_GetBlockMemoryAddress(g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    Tex_RegisterTexture(header, &ref);

    // 7 - plunging on
    ref = Tex_LoadTexture("textures\\COMP03_1.bmp");
    Heap_GetBlockMemoryAddress(g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    Tex_RegisterTexture(header, &ref);

    // ref = Tex_LoadTexture("BKEYA0.bmp");
    // Heap_GetBlockMemoryAddress(g_heap, &ref);
    // header = (Texture2DHeader *)ref.ptrMemory;
    // Tex_RegisterTexture(header, &ref);

    Tex_BindAll();

    
    g_platform.SetDebugInputTextureIndex(
        Tex_GetTextureIndexByName("textures\\charset.bmp"));
}

i32 Tex_RenderModuleReloaded()
{
    Tex_BindAll();
    return COM_ERROR_NONE;
}
