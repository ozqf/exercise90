#ifndef ZPG_GRID_H
#define ZPG_GRID_H

#include "ze_proc_gen.h"

extern "C" ZPGGrid* ZPG_CreateGrid(i32 width, i32 height)
{
    i32 totalCells = width * height;
    i32 memForGrid = (sizeof(ZPGCell) * totalCells);
    i32 memTotal = sizeof(ZPGGrid) + memForGrid;
    printf("Make grid %d by %d (%d cells, %d bytes)\n",
        width, height, (width * height), memTotal);
    u8* ptr = (u8*)malloc(memTotal);
    // Create grid struct
    ZPGGrid* grid = (ZPGGrid*)ptr;
    *grid = {};
    // init grid memory
    ptr += sizeof(ZPGGrid);
    //memset(ptr, ' ', memForGrid);
    grid->cells = (ZPGCell*)ptr;
    for (i32 i = 0; i < totalCells; ++i)
    {
        grid->cells[i] = {};
        grid->cells[i].type = ZPGCELL_TYPE_WALL;
    }
    grid->width = width;
    grid->height = height;
    return grid;
}

extern "C" void ZPG_GridRandomWalk(ZPGGrid* grid, ZPGWalkCfg* cfg, ZPGPoint dir)
{
    ZPGPoint cursor = { cfg->startX, cfg->startY };
    //ZPGPoint dir = ZPG_RandomFourWayDir(&cfg->seed);
	printf("Drunken Walk starting angle %.1f\n", (atan2f((f32)dir.y, (f32)dir.x) * RAD2DEG));
    const i32 escapeCounter = 999999;
    i32 iterations = 0;
    i32 tilesPlaced = 0;
    while (tilesPlaced < cfg->tilesToPlace)
    {
        ZPGCell* cell = grid->GetCellAt(cursor.x, cursor.y);
        if (cell != NULL && cell->type != cfg->typeToPaint)
        {
            grid->SetCellAt(cursor.x, cursor.y, cfg->typeToPaint);
            tilesPlaced++;
        }
        grid->MoveWithBounce(&cursor, &dir);
        const f32 turnChance = 0.3f; //0.6f
        #if 1
        f32 changeDirChance = ZPG_Randf32(cfg->seed++);
        if (changeDirChance > turnChance)
        {
            dir = ZPG_RandomThreeWayDir(&cfg->seed, dir);
            //dir = ZPG_RandomFourWayDir(&cfg->seed);
        }
        #endif
        iterations++;
        if (iterations >= escapeCounter)
        {
            printf("ABORT! Walk ran away\n");
            break;
        }
    }
    printf("Drunken walk placed %d tiles in %d iterations\n",
        tilesPlaced, iterations);
}

#endif // ZPG_GRID_H