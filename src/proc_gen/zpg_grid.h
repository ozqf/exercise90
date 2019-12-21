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
    memset(ptr, ' ', memForGrid);
    grid->cells = (ZPGCell*)ptr;
    grid->width = width;
    grid->height = height;
    return grid;
}

extern "C" void ZPG_GridRandomWalk(ZPGGrid* grid, ZPGWalkCfg* cfg)
{
    ZPGPoint cursor = { cfg->startX, cfg->startY };
    ZPGPoint dir = ZPG_RandomFourWayDir(&cfg->seed);
    i32 escapeCounter = 999999;
    i32 tilesPlaced = 0;
    while (tilesPlaced < cfg->tilesToPlace)
    {
        ZPGCell* cell = grid->GetCellAt(cursor.x, cursor.y);
        if (cell != NULL && cell->c != cfg->charToPlace)
        {
            grid->SetCellAt(cursor.x, cursor.y, cfg->charToPlace, 0);
            tilesPlaced++;
        }
        grid->MoveWithBounce(&cursor, &dir);
        f32 changeDirChance = ZPG_Randf32(cfg->seed++);
        if (changeDirChance > 0.6f)
        {
            dir = ZPG_RandomFourWayDir(&cfg->seed);
        }
        escapeCounter--;
        if (escapeCounter == 0)
        {
            printf("ABORT! Walk ran away after %d tiles!\n",
                tilesPlaced);
            break;
        }
    }
}

#endif // ZPG_GRID_H