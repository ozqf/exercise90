#ifndef ZPG_PERLIN_DRAW_H
#define ZPG_PERLIN_DRAW_H

#include "ze_proc_gen.h"
#include "zpg_perlin.h"

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

            f32 noise = Perlin_Get2d((f32)(x * 5), (f32)(y * 5), 2, 2);
            f32 a = sinf((x + noise / 2) * 50);
            result = (1 + a)  / 2;
            grid->GetCellAt(pixelX, pixelY)->type = (i32)(4.f * result);
        }
    }
}

#endif // ZPG_PERLIN_DRAW_H