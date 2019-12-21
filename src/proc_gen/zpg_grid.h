#ifndef ZPG_GRID_H
#define ZPG_GRID_H

#include "ze_proc_gen.h"

extern "C" ZPGGrid* ZPG_CreateGrid(i32 width, i32 height)
{
    i32 totalCells = width * height;
    i32 memTotal = sizeof(ZPGGrid) + (sizeof(ZPGCell) * totalCells);
    printf("Make grid %d by %d (%d bytes)\n", width, height, memTotal);
    u8* ptr = (u8*)malloc(memTotal);
    memset(ptr, 0, memTotal);
    ZPGGrid* grid = (ZPGGrid*)ptr;
    ptr += sizeof(ZPGGrid);
    grid->cells = (ZPGCell*)ptr;
    grid->width = width;
    grid->height = height;
    return grid;
}

#endif // ZPG_GRID_H