#ifndef ZPG_CAVE_GEN_H
#define ZPG_CAVE_GEN_H

#include "ze_proc_gen.h"

extern "C" void ZPG_SeedCaves(ZPGGrid* grid, i32 paintType, i32* seed)
{
    // 0.45 very little paint chance. 0.55 much higher.
    const f32 seedChance = 0.55f;
    for (i32 y = 0; y < grid->height; ++y)
    {
        for (i32 x = 0; x < grid->width; ++x)
        {
            f32 rand = ZPG_Randf32(*seed);
            *seed += 1;
            if (rand < seedChance)
            {
                ZPGCell* cell = grid->GetCellAt(x, y);
                cell->type = paintType;
            }
        }
    }
}

extern "C" void ZPG_IterateCaves(ZPGGrid* grid, i32 solidType, i32 emptyType)
{
    const i32 removeCellLimit = 4;
    const i32 addCellLimit = 4;
    for (i32 y = 0; y < grid->height; ++y)
    {
        for (i32 x = 0; x < grid->width; ++x)
        {
            ZPGCell* cell = grid->GetCellAt(x, y);
            i32 neighbours = grid->CountNeighboursAt(x, y);
            if (neighbours < 4)
            {
                if (cell->type == solidType)
                {
                    cell->type = emptyType;
                }
                else if (cell->type == emptyType)
                {
                    cell->type = solidType;
                }
            }
            // if (cell->type == solidType)
            // {
            //     if (neighbours < 4)
            //     {
            //         cell->type = emptyType;
            //     }
            // }
        }
    }
}

#endif // ZPG_CAVE_GEN_H