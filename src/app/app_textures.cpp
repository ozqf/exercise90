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
    COM_CALL_PRINT(g_platform.Log, 512, "APP tex %s bound to index %d\n",
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


struct BW8x8Block
{
  // bit == x
  // index == y
  unsigned char pixels[8];
};

struct BWImage
{
  int numBlocksX;
  int numBlocksY;
  unsigned char *blocks;
};

internal i32 Tex_BWBlockIndex2BlockX(i32 blockIndex, i32 blocksWide)
{
    return blockIndex % blocksWide;
}

internal i32 Tex_BWBlockIndex2BlockY(i32 blockIndex, i32 blocksWide)
{
    return blockIndex / blocksWide;
}

void Tex_GenerateBW(char* sourceTextureName)
{
    i32 texIndex = Tex_GetTextureIndexByName(sourceTextureName);
    Texture2DHeader* h = &g_textureHandles.textureHeaders[texIndex];
    printf("  Gen BW tex from %s\n Size %d, %d\n", h->name, h->width, h->height);
    i32 modWidth = h->width % 8;
    i32 modHeight = h->height % 8;
    if (modWidth || modHeight)
    {
        printf("  Cannot gen texture. width not divisible by 8\n");
        return;
    }
    i32 blocksX = h->width / 8;
    i32 blocksY = h->height / 8;
    i32 totalBlocks = blocksX * blocksY;
    i32 totalBytes = totalBlocks * 8;
    printf("  Total blocks %d (%d by %d) - %d bytes\n",
        totalBlocks, blocksX, blocksY, totalBytes);

    BW8x8Block* blocks = (BW8x8Block*)malloc(totalBytes);
    COM_SET_ZERO(blocks, totalBytes);
    for (i32 i = 0; i < totalBlocks; ++i)
    {
        BW8x8Block* block = &blocks[i];
        
        i32 blockX = i % blocksX;
        i32 blockY = i / blocksX;
        
        i32 firstPixelX = blockX * 8;
        i32 firstPixelY = blockY * 8;

        if (blockX == 15 && blockY == 15)
        {
            printf("First Pixel x/y: %d, %d\n", firstPixelX, firstPixelY);
        }
        // Go down bytes
        for (i32 y = 0; y < 8; ++y)
        {
            i32 pixelY = firstPixelY + y;
            u8 val = 0;
            // Go across bits
            for (i32 x = 0; x < 8; ++x)
            {
                i32 pixelX = firstPixelX + x;

                // Sample colour in BMP
                i32 sourcePixelIndex = pixelX + (pixelY * h->height);
                u32 sourcePixel = h->ptrMemory[sourcePixelIndex];
                u32_union* u32Bytes = (u32_union*)&sourcePixel;

                val |= (1 << x);

                if (blockX == 15 && blockY == 15)
                {
                    printf("Pixel: %d, %d: RGBA: %d, %d, %d, %d\n",
                        pixelX, pixelY,
                        u32Bytes->bytes[0],
                        u32Bytes->bytes[1],
                        u32Bytes->bytes[2],
                        u32Bytes->bytes[3]
                    );
                }
            }
            block->pixels[y] = val;
        }
    }

    // Print results

    for (i32 i = 0; i < totalBlocks; ++i)
    {
        BW8x8Block* block = &blocks[i];
        for (i32 y = 0; y < )
    }

    // for (i32 y = 0; y < blocksY; ++y)
    // {
    //     for (i32 x = 0; x < blocksX; ++x)
    //     {
    //         i32 index = x + (y * blocksY);
    //         BW8x8Block* block = &blocks[index];
    //         printf("%d, ", b)
    //     }
    // }
}
