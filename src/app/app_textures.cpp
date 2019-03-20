#pragma once

//#include "app_module.h"
#include "../common/com_module.h"
#include "../interface/platform_interface.h"
#include "app_textures.h"

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


internal i32 Tex_RegisterTexture(Texture2DHeader *header, BlockRef *ref);


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
    COM_CALL_PRINT(g_platform.Log, 512, "APP tex %s not found, loading\n", textureName);
    return Tex_LoadAndBindTexture(textureName);
    //return -1;
}

Texture2DHeader* Tex_GetTextureByName(char* textureName)
{
    i32 index = Tex_GetTextureIndexByName(textureName);
    return &g_textureHandles.textureHeaders[index];
}

/**
 * Upload a texture to the GPU
 */
void Tex_BindTexture(Texture2DHeader *header)
{
    g_platform.BindTexture(
        header->ptrMemory,
        header->width,
        header->height,
        header->index);
    COM_CALL_PRINT(g_platform.Log, 512, "APP tex %s bound to index %d\n",
        header->name, header->index);
}

Texture2DHeader* Tex_AllocateTexture(char* name, i32 width, i32 height)
{
    Assert(width > 0)
    Assert(height > 0)
    Assert(name)
    Assert(COM_StrLen(name) < TEXTURE2D_MAX_NAME_LENGTH)
    i32 bytesForPixels = sizeof(u32) * (width * height);
    i32 totalBytes = bytesForPixels + sizeof(Texture2DHeader);
    BlockRef ref;
    Heap_Allocate(g_heap, &ref, totalBytes, name, 1);
    Texture2DHeader* tex = (Texture2DHeader*)ref.ptrMemory;
    COM_CopyStringLimited(name, tex->name, TEXTURE2D_MAX_NAME_LENGTH);
    u32* pixels = (u32*)((u8*)ref.ptrMemory + sizeof(Texture2DHeader));
    tex->ptrMemory = pixels;
    tex->width = width;
    tex->height = height;

    // Set All Magenta
	ColourU32 col;
	col.r = 255;
	col.g = 0;
	col.b = 255;
	col.a = 255;
	COMTex_SetAllPixels(tex, col);
    //u8 pattern[4] = { 255, 0, 255, 255 };
    //COM_SetMemoryPattern((u8*)tex->ptrMemory, bytesForPixels, pattern, 4);
	
    Tex_RegisterTexture(tex, &ref);
    return tex;
}

//////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////

/**
 * Record that a texture has been loaded onto the heap
 * > header information will be copied and updated
 * > BlockRef should be a Heap block for a Texture2DHeader!
 */
internal i32 Tex_RegisterTexture(Texture2DHeader *header, BlockRef *ref)
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
    return index;
}

/**
 * Read a texture onto the Global Heap
 */
internal BlockRef Tex_LoadTexture(char *filePath)
{
    BlockRef ref = {};
    g_platform.LoadFileIntoHeap(g_heap, &ref, filePath, 0);
    return ref;
}

/**
 * Read a texture onto the global heap and then immediately bind it
 * Returns the texture's index
 */
i32 Tex_LoadAndBindTexture(char *filePath)
{
    BlockRef ref = Tex_LoadTexture(filePath);
    Heap_GetBlockMemoryAddress(g_heap, &ref);
    Texture2DHeader *header = (Texture2DHeader *)ref.ptrMemory;
    Tex_RegisterTexture(header, &ref);
    Tex_BindTexture(header);
    return header->index;
}

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
}

// Terminate texture list with an empty string
void Tex_LoadTextureList(char** textures)
{
    printf("APP Load texture list\n");
    i32 i = 0;
    char* str = textures[i];
    while (*str != '\0')
    {
        printf("Load and Bind texture %s\n", str);
        Tex_LoadAndBindTexture(str);
        str = textures[++i];
    }
}

i32 Tex_RenderModuleReloaded()
{
    Tex_BindAll();
    return COM_ERROR_NONE;
}
