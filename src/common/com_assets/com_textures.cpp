#pragma once

#include "com_textures.h"

// Convert a byte (0...255) to float (0...1)
f32 COM_ByteToFloat(u8 byte)
{
    return (f32)byte / (f32)255;
}

// Convert a float (0...1) to byte (0...255)
u8 COM_FloatToByte(f32 f)
{
    return (u8)(255 * f);
}

#define BEGIN_PIXEL_FUNCTION(functionName) \
void TexDraw_##functionName##(Texture2DHeader* tex) \
{ \
    for (i32 pixelY = 0; pixelY < tex->height; ++pixelY) \
    { \
        for (i32 pixelX = 0; pixelX < tex->width; ++pixelX) \
        { \
            f32 x = (f32)pixelX / (f32)tex->width; \
            f32 y = (f32)pixelY / (f32)tex->width; \
            u32 pixelIndex = pixelX + (pixelY * tex->width); \
            ColourU32* pixel = (ColourU32*)&tex->ptrMemory[pixelIndex]; \
            f32 result;

#define END_PIXEL_FUNCTION \
u8 value = COM_FloatToByte(result); \
*pixel = { value, value, value, 255 }; \
} } }

BEGIN_PIXEL_FUNCTION(BasicGradient)
result = x;
END_PIXEL_FUNCTION

BEGIN_PIXEL_FUNCTION(DoubleSine)
f32 a = (1 + sinf(x * 50 )) / 2;
f32 b = (1 + sinf(y * 50 )) / 2;
result = (a * 0.5f) + (b * 0.5f);
END_PIXEL_FUNCTION

BEGIN_PIXEL_FUNCTION(CrudeSineGradient)
//f32 f = sinf(lerpX * 10);
f32 a = (1 + sinf(x * 24)) / 2;
f32 b = Perlin_Get2d(x * 5, y * 5, 24, 12);
result = (a * 0.6f) + (b * 0.4f);
END_PIXEL_FUNCTION

BEGIN_PIXEL_FUNCTION(SineGradient)
f32 a = (x * y);
f32 b = Perlin_Get2d(x, y, 24, 12);
result = (a * 0.7f) + (b * 0.3f);
END_PIXEL_FUNCTION

BEGIN_PIXEL_FUNCTION(Streaks)
f32 noise = Perlin_Get2d((f32)(x * 5), (f32)(y * 5), 2, 2);
f32 a = sinf((x + noise / 2) * 50);
result = (1 + a)  / 2;
END_PIXEL_FUNCTION

BEGIN_PIXEL_FUNCTION(DoubleStreaks)
f32 noise = Perlin_Get2d((f32)(x * 5), (f32)(y * 5), 2, 2);
f32 a = sinf((x + noise / 2) * 50);
a = (1 + a)  / 2;
f32 b = sinf((y + noise / 2) * 100 );
b = (1 + b) / 2;
result = (a * 0.5f) + (b * 0.5f);
END_PIXEL_FUNCTION

BEGIN_PIXEL_FUNCTION(Scatter)
f32 noise = Perlin_Get2d((f32)(x * 5), (f32)(y * 5), 2, 2);
result = noise > 0.5f ? 1.0f : 0.0f;
END_PIXEL_FUNCTION

void TexDraw_Gradient(Texture2DHeader* tex, i32 type)
{
    //TexDraw_BasicGradient(tex);
    //TexDraw_DoubleSine(tex);
    //TexDraw_CrudeSineGradient(tex);
    //TexDraw_SineGradient(tex);
    //TexDraw_Streaks(tex);
    //TexDraw_DoubleStreaks(tex);
    TexDraw_Scatter(tex);
}

void COMTex_SetAllPixels(Texture2DHeader* tex, ColourU32 col)
{
	i32 bytesForPixels = sizeof(u32) * (tex->width * tex->height);
	u8 pattern[4] = { col.r, col.g, col.b, col.a };
	COM_SetMemoryPattern((u8*)tex->ptrMemory, bytesForPixels, pattern, 4);
}

void TexDraw_FillRect(
    Texture2DHeader* tex, Point topLeft, Point size, ColourU32 col)
{
    i32 endX = size.x + topLeft.x;
    i32 endY = size.y + topLeft.y;
    if (topLeft.x < 0) { topLeft.x = 0; }
    if (topLeft.y < 0) { topLeft.y = 0; }
    if (endX > tex->width) { endX = tex->width; }
    if (endY > tex->height) { endY = tex->height; }
    i32 predictedPixels = size.x * size.y;
    i32 pixels = 0;
    for (i32 y = topLeft.y; y < endY; ++y)
    {
        for (i32 x = topLeft.x; x < endX; ++x)
        {
            tex->ptrMemory[x + (y * tex->width)] = col.value;
            pixels++;
        }
    }
    printf("Fill rect %d, %d size %d, %d - predicted %d wrote %d\n",
        topLeft.x, topLeft.y, size.x, size.y, predictedPixels, pixels
    );
}

/*
Single pixel line, uses integers.
Not the same as the grid traversing algorithm used by Ray traces for collisions
 */
void TexDraw_Line(Texture2DHeader* tex, ColourU32 col, i32 x0, i32 y0, i32 x1, i32 y1)
{
	//y0 -= 20;
	//y1 -= 20;
	i32 dx = x1 - x0;
	if (dx < 0) { dx *= -1; }	//abs
	i32 dy = y1 - y0;
	if (dy < 0) { dy *= -1; }	//abs
	i32 plotX = x0;
	i32 plotY = y0;
	i32 n = 1 + dx + dy;
	i32 x_inc;
	i32 y_inc;
	if (x1 > x0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
	}
	
	if (y1 > y0)
	{
		y_inc = 1;
	}
	else
	{
		y_inc = -1;
	}
	i32 error = dx - dy;
	dx *= 2;
	dy *= 2;
	
	for (; n > 0; --n)
	{
		tex->ptrMemory[plotX + (plotY * tex->width)] = col.value;
		
		if (error > 0)
		{
			plotX += x_inc;
			error -= dy;
		}
		else
		{
			plotY += y_inc;
			error += dx;
		}
	}
}

void TexDraw_Outline(Texture2DHeader* tex, ColourU32 col)
{
    i32 x = 0;
    i32 y = 0;
    i32 end = tex->width;
    // 
    for (; x < end; ++x)
    {
        i32 index = x + (y * tex->width);
        u32* pixel = &tex->ptrMemory[index];
        *pixel = col.value;
    }
    y = tex->height - 1;
}

/**
 * Convert bmp file pixel byte order to opengl rgba
 */
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

///////////////////////////////////////////////////
// Black and white bit-packed
///////////////////////////////////////////////////
i32 Tex_CalcBytesForBWPixels(i32 sourceWidth, i32 sourceHeight)
{
    i32 modWidth = sourceWidth % 8;
    i32 modHeight = sourceHeight % 8;
    // Must be divisible by 8
    if (modWidth || modHeight)
    {
        return -1;
    }
    return (sourceWidth * sourceHeight)/ 8;
}

void Tex_BWSetAllPixels(BW8x8Block* block)
{
   for (i32 i = 0; i < 8; ++i)
   {
       block->pixels[i] = 255;
   }
}


Point Tex_CalcInternalImageSizeFromBW(BWImage* img)
{
    Point p;
    p.x = img->size.x * 8;
    p.y = img->size.y * 8;
    return p;
}

#if 0
void Tex_BW2Bitmap(BWImage* img, Texture2DHeader* h)
{
    u32* firstPixel = h->ptrMemory;
    i32 numBlocks = img->size.x * img->size.y;
    for (i32 y = 0; y < img->size.y; ++y)
    {
        for (i32 x = 0; x < img->size.x; ++x)
        {
            i32 i = x + (y * img->size.x);
            BW8x8Block* block = &img->blocks[i];
            for (i32 byte = 0; byte < 8; ++byte)
            {
                for (i32 bit = 0; bit < 8; ++bit)
                {
                    u32 val = block->pixels[byte] & bit;
                    i32 pixX = (x * 8) + bit;
                    i32 pixY = (y * 8) + byte;
                    u32 pixelIndex = pixX + (pixY * h->width);
                    
                    u32_union* u32Bytes = (u32_union*)&h->ptrMemory[pixelIndex];
                    u32Bytes->bytes[0] = (u8)(255 * val);
                    u32Bytes->bytes[1] = (u8)(255 * val);
                    u32Bytes->bytes[2] = (u8)(255 * val);
                    u32Bytes->bytes[3] = 255;
                    
                }
            }
        }
    }
}
#endif

Point Tex_CalcBWImageSizeFromBitmap(Texture2DHeader* h)
{
    Point p;
    // validate source
    if (!h->width || !h->height)
    {
        return {};
    }
    i32 modWidth = h->width % 8;
    i32 modHeight = h->height % 8;
    // Must be divisible by 8
    if (modWidth || modHeight)
    {
        return {};
    }
    p.x = h->width / 8;
    p.y = h->height / 8;
    return p;
}

void Tex_GenerateBW(Texture2DHeader* h, BWImage* img)
{

    i32 totalBlocks = img->size.x * img->size.y;
    for (i32 i = 0; i < totalBlocks; ++i)
    {
        BW8x8Block* block = &img->blocks[i];
        
        i32 blockX = i % img->size.x;
        i32 blockY = i / img->size.y;
        
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
        BW8x8Block* block = &img->blocks[i];
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
