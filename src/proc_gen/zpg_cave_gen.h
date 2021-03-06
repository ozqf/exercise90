#ifndef ZPG_CAVE_GEN_H
#define ZPG_CAVE_GEN_H

#include "ze_proc_gen.h"

// 0.45 very little paint chance. 0.55 much higher.
#define ZPG_CAVE_GEN_DEFAULT_CELL_SEED_CHANCE 0.55f
#define ZPG_CAVE_GEN_CRITICAL_NEIGHBOUR_COUNT 4

/**
 * Randomly paint tiles for growing caves
 * Any Stencil cell whose value is not 0 will be skipped
 */
extern "C" void ZPG_SeedCaves(ZPGGrid* grid, ZPGGrid* stencil, u8 paintType, i32* seed)
{
    const f32 seedChance = ZPG_CAVE_GEN_DEFAULT_CELL_SEED_CHANCE;
    for (i32 y = 0; y < grid->height; ++y)
    {
        for (i32 x = 0; x < grid->width; ++x)
        {
            if (ZPG_CheckStencilOccupied(stencil, x, y) == YES) { continue; }
            f32 rand = ZPG_Randf32(*seed);
            *seed += 1;
            if (rand < seedChance)
            {
                ZPGCell* cell = grid->GetCellAt(x, y);
                cell->tile.type = paintType;
            }
        }
    }
}

/**
 * Add/Remove tiles based on their neighbour count
 * Any Stencil cell whose value is not 0 will be skipped
 */
extern "C" void ZPG_IterateCaves(ZPGGrid* grid, ZPGGrid* stencil, u8 solidType, u8 emptyType)
{
    for (i32 y = 0; y < grid->height; ++y)
    {
        for (i32 x = 0; x < grid->width; ++x)
        {
            if (ZPG_CheckStencilOccupied(stencil, x, y) == YES) { continue; }
            ZPGCell* cell = grid->GetCellAt(x, y);
            i32 neighbours = grid->CountNeighboursAt(x, y);
            if (neighbours < ZPG_CAVE_GEN_CRITICAL_NEIGHBOUR_COUNT)
            {
                if (cell->tile.type == solidType)
                {
                    cell->tile.type = emptyType;
                }
                else if (cell->tile.type == emptyType)
                {
                    cell->tile.type = solidType;
                }
            }
        }
    }
}

#endif // ZPG_CAVE_GEN_H