#pragma once

#include "app_module.cpp"


/**
 * Record that a texture has been loaded
 * > header information will be copied and updated
 * > BlockRef should be a Heap block for a Texture2DHeader!
 */
void AppRegisterTexture(Texture2DHeader *header, BlockRef *ref)
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

/**
 * Upload a texture to the GPU
 */
void AppBindTexture(Texture2DHeader *header)
{
    platform.Platform_BindTexture(header->ptrMemory, header->width, header->height, header->index);
}

/**
 * Read a texture onto the Global Heap
 */
BlockRef AppLoadTexture(char *filePath)
{
    BlockRef ref = {};
    platform.Platform_LoadTexture(&g_heap, &ref, filePath);
    return ref;
}

/**
 * Read a texture onto the global heap and then immediately bind it
 */
void AppLoadAndRegisterTexture(char *filePath)
{
    BlockRef ref = AppLoadTexture(filePath);

    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    Texture2DHeader *header = (Texture2DHeader *)ref.ptrMemory;
    AppRegisterTexture(header, &ref);
    //AppBindTexture((Texture2DHeader*)ref.ptrMemory, &ref);
}

/**
 * Upload all registered textures
 */
void AppBindAllTextures()
{
    i32 numTextures = g_textureHandles.numTextures;
    for (i32 i = 0; i < numTextures; ++i)
    {
        AppBindTexture(&g_textureHandles.textureHeaders[i]);
    }
}

void AppLoadTestTextures()
{
    BlockRef ref;
    Texture2DHeader *header;

    AppInitTestTextures();

    // 0
    AppRegisterTexture(&testBuffer, NULL);

    // 1
    AppRegisterTexture(&testBuffer2, NULL);

    // 2 - you get the picture
    AppRegisterTexture(&testBuffer3, NULL);

    // 3 - Kinda icky this
    ref = AppLoadTexture("BitmapTest.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    AppRegisterTexture(header, &ref);

    // 4 - so, texture management sometime?
    ref = AppLoadTexture("charset.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    AppRegisterTexture(header, &ref);

    // 5 - can't be too hard...
    ref = AppLoadTexture("brbrick2.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    AppRegisterTexture(header, &ref);

    // 6 - right?
    ref = AppLoadTexture("W33_5.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    AppRegisterTexture(header, &ref);

    // ref = AppLoadTexture("BKEYA0.bmp");
    // Heap_GetBlockMemoryAddress(&g_heap, &ref);
    // header = (Texture2DHeader *)ref.ptrMemory;
    // AppRegisterTexture(header, &ref);

    AppBindAllTextures();
}

i32 AppRendererReloaded()
{
    AppBindAllTextures();
    return 1;
}
