#ifndef ZPG_PERLIN_DRAW_H
#define ZPG_PERLIN_DRAW_H

#include "ze_proc_gen.h"
#include "zpg_perlin.h"

static void ZPG_ApplyPerlinThreshold(ZPGGrid* grid, u8 threshold)
{
    for (i32 pixelY = 0; pixelY < grid->height; ++pixelY)
    {
        for (i32 pixelX = 0; pixelX < grid->width; ++pixelX)
        {
            ZPGCell* cell = grid->GetCellAt(pixelX, pixelY);
            if (cell->type > threshold)
            {
                cell->type = ZPG_CELL_TYPE_FLOOR;
            }
            else
            {
                cell->type = ZPG_CELL_TYPE_WALL;
            }
        }
    }
}

static void ZPG_DrawPerlinGrid(ZPGGrid* grid)
{
    for (i32 pixelY = 0; pixelY < grid->height; ++pixelY)
    {
        for (i32 pixelX = 0; pixelX < grid->width; ++pixelX)
        {
            f32 x = (f32)pixelX / (f32)grid->width; \
            f32 y = (f32)pixelY / (f32)grid->height; \
            //u32 pixelIndex = pixelX + (pixelY * grid->width); \
            //ColourU32* pixel = (ColourU32*)&tex->ptrMemory[pixelIndex];
            f32 result;
            #if 0
            f32 noise = Perlin_Get2d((f32)(x * 5), (f32)(y * 5), 2, 2);
            f32 a = sinf((x + noise / 2) * 50);
            result = (1 + a)  / 2;
            #endif
            #if 1
            f32 a = (1 + sinf(x * 50 )) / 2;
            f32 b = (1 + sinf(y * 50 )) / 2;
            result = (a * 0.5f) + (b * 0.5f);
            #endif
            // type is scaled to 0-255
            grid->GetCellAt(pixelX, pixelY)->type = (i32)(255.f * result);
        }
    }
    ZPG_ApplyPerlinThreshold(grid, 72);
}

#endif // ZPG_PERLIN_DRAW_H