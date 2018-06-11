#pragma once

#include "app_module.cpp"

void AppListTextures()
{
    i32 l = g_textureHandles.numTextures;
    printf("--- APP TEXTURES (%d) ---\n", l);
    for (i32 i = 0; i < l; ++i)
    {
        Texture2DHeader* h = &g_textureHandles.textureHeaders[i];
        printf("%d: \"%s\" %dx%d\n", i, h->name, h->width, h->height);
    }
}

i32 AppGetTextureIndexByName(char* textureName)
{
    i32 l = g_textureHandles.numTextures;
    for (i32 i = 0; i < l; ++i)
    {
        if (COM_CompareStrings(g_textureHandles.textureHeaders[i].name, textureName) == 0)
        {
            return i;
        }
    }
    printf("APP tex %s not found, loading\n", textureName);
    return AppLoadAndBindTexture(textureName);
    //return -1;
}

/**
 * Record that a texture has been loaded
 * > header information will be copied and updated
 * > BlockRef should be a Heap block for a Texture2DHeader!
 */
#if 1
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
#endif
/**
 * Upload a texture to the GPU
 */
void AppBindTexture(Texture2DHeader *header)
{
    platform.Platform_BindTexture(header->ptrMemory, header->width, header->height, header->index);
    printf("APP tex %s bound to index %d\n", header->name, header->index);
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
 * Returns the texture's index
 */
#if 1
i32 AppLoadAndBindTexture(char *filePath)
{
    BlockRef ref = AppLoadTexture(filePath);
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    Texture2DHeader *header = (Texture2DHeader *)ref.ptrMemory;
    AppRegisterTexture(header, &ref);
    AppBindTexture(header);
    return header->index;
}
#endif
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

    //AppInitTestTextures();

    // 0
    //AppRegisterTexture(&testBuffer, NULL);

    // 1
    //AppRegisterTexture(&testBuffer2, NULL);

    // 2 - you get the picture
    //AppRegisterTexture(&testBuffer3, NULL);

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

    // 7 - plunging on
    ref = AppLoadTexture("COMP03_1.bmp");
    Heap_GetBlockMemoryAddress(&g_heap, &ref);
    header = (Texture2DHeader *)ref.ptrMemory;
    AppRegisterTexture(header, &ref);

    // ref = AppLoadTexture("BKEYA0.bmp");
    // Heap_GetBlockMemoryAddress(&g_heap, &ref);
    // header = (Texture2DHeader *)ref.ptrMemory;
    // AppRegisterTexture(header, &ref);

    AppBindAllTextures();

    
    platform.Platform_SetDebugInputTextureIndex(AppGetTextureIndexByName("charset.bmp"));
}

i32 AppRendererReloaded()
{
    AppBindAllTextures();
    return 1;
}
