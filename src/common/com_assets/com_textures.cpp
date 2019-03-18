#pragma once

#include "com_textures.h"


void COMTex_BMP2Internal(
    u32* sourcePixels,
    u32* destPixels,
    u32 numPixels)
{
    
	// Convert colours to correct format
	for (u32 i = 0; i < numPixels; ++i)
	{
		u8 alpha = (u8)(*sourcePixels);
		u8 blue = (u8)(*sourcePixels >> 8);
		u8 green = (u8)(*sourcePixels >> 16);
		u8 red = (u8)(*sourcePixels >> 24);
		u32_union u32Bytes;
		u32Bytes.bytes[0] = red;
		u32Bytes.bytes[1] = green;
		u32Bytes.bytes[2] = blue;
		u32Bytes.bytes[3] = alpha;
		*destPixels = u32Bytes.value;

		++destPixels;
		++sourcePixels;
	}
}


void Tex_GenerateBW(Texture2DHeader* h)
{
    
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

