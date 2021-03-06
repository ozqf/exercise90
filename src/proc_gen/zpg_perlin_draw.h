#ifndef ZPG_PERLIN_DRAW_H
#define ZPG_PERLIN_DRAW_H

#include "ze_proc_gen.h"
#include "zpg_perlin.h"

// Convert a byte (0...255) to float (0...1)
static f32 ZPG_ByteToFloat(u8 byte)
{
    return (f32)byte / (f32)255;
}

// Convert a float (0...1) to byte (0...255)
static u8 ZPG_FloatToByte(f32 f)
{
    return (u8)(255 * f);
}

/**
 * types should be an array of three u8s.
 * 1 for each threshold
 */
static void ZPG_ApplyPerlinThreshold(
    ZPGGrid* grid, ZPGGrid* stencil, u8* types, i32 numTypes)
{
    f32 step = 1.f / numTypes;
    for (i32 pixelY = 0; pixelY < grid->height; ++pixelY)
    {
        for (i32 pixelX = 0; pixelX < grid->width; ++pixelX)
        {
            if (ZPG_CheckStencilOccupied(stencil, pixelX, pixelY)) { continue; }
            ZPGCell* cell = grid->GetCellAt(pixelX, pixelY);
            f32 value = ZPG_ByteToFloat(cell->tile.type);
            #if 0
            if (value > 0.66f) { cell->tile.type = types[0]; }
            else if (value > 0.33f) { cell->tile.type = types[1]; }
            else { cell->tile.type = types[2]; }
            #endif
            #if 1
            u8 newType = types[(i32)(numTypes * value)];
            cell->tile.type = newType;
            #endif
        }
    }
}

static void ZPG_DrawPerlinGrid(ZPGGrid* grid, ZPGGrid* stencil, i32* seed)
{
    f32 seedX = ZPG_Randf32(*seed) * 99999.f;
    *seed += 1;
    f32 seedY = ZPG_Randf32(*seed) * 99999.f;
    *seed += 1;
    for (i32 pixelY = 0; pixelY < grid->height; ++pixelY)
    {
        for (i32 pixelX = 0; pixelX < grid->width; ++pixelX)
        {
            if (ZPG_CheckStencilOccupied(stencil, pixelX, pixelY)) { continue; }
            f32 x = (f32)pixelX / (f32)grid->width; \
            f32 y = (f32)pixelY / (f32)grid->height; \
            //u32 pixelIndex = pixelX + (pixelY * grid->width); \
            //ColourU32* pixel = (ColourU32*)&tex->ptrMemory[pixelIndex];
            f32 result;
            #if 1 // streaks
            f32 scaleFactor = 5;
            f32 noiseScaler = 50;
            f32 sampleX = (x * scaleFactor) + seedX;
            f32 sampleY = (y * scaleFactor) + seedY;
            f32 noise = Perlin_Get2d(sampleX, sampleY, 2, 2);
            //f32 noise = Perlin_Get2d((f32)(x * scaleFactor), (f32)(y * scaleFactor), 2, 2);
            f32 a = sinf((x + noise / 2) * noiseScaler);
            result = (1 + a)  / 2;
            #endif
            #if 0 // double streaks
            f32 scaleFactorX = 5;
            f32 scaleFactorY = 5;
            f32 noiseScalerX = 25;//50;
            f32 noiseScalerY = 25;//50;
            f32 noise = Perlin_Get2d((f32)(x * scaleFactorX), (f32)(y * scaleFactorY), 2, 2);
            f32 a = sinf((x + noise / 2) * noiseScalerX);
            a = (1 + a)  / 2;
            f32 b = sinf((y + noise / 2) * noiseScalerY);
            b = (1 + b) / 2;
            result = (a * 0.5f) + (b * 0.5f);
            #endif
            #if 0 // double sine
            f32 a = (1 + sinf(x * 50 )) / 2;
            f32 b = (1 + sinf(y * 50 )) / 2;
            result = (a * 0.5f) + (b * 0.5f);
            #endif
            #if 0 // scatter
            // Scale factor of 1 means little variation. 10 for a lot
            f32 scaleFactor = 4;//10;//5;
            f32 noise = Perlin_Get2d((f32)(x * scaleFactor), (f32)(y * scaleFactor), 2, 2);
            //result = noise > 0.5f ? 1.0f : 0.0f; // make result binary
            result = noise;
            #endif
            // type is scaled to 0-255
            grid->GetCellAt(pixelX, pixelY)->tile.type = (u8)(255.f * result);
        }
    }
    #if 0
    //u8 types[] = { ZPG_CELL_TYPE_WALL, ZPG_CELL_TYPE_FLOOR, ZPG_CELL_TYPE_WATER };
    //u8 types[] = { ZPG_CELL_TYPE_WATER, ZPG_CELL_TYPE_FLOOR, ZPG_CELL_TYPE_WALL };
    u8 types[] = { ZPG_CELL_TYPE_WALL, ZPG_CELL_TYPE_WATER, ZPG_CELL_TYPE_FLOOR };
    ZPG_ApplyPerlinThreshold(grid, stencil, types, 3);
    #endif
    #if 1
    u8 types[] = { ZPG_CELL_TYPE_WALL, ZPG_CELL_TYPE_FLOOR };
    ZPG_ApplyPerlinThreshold(grid, stencil, types, 2);
    #endif
}

#endif // ZPG_PERLIN_DRAW_H