#ifndef ZE_PROC_GEN_H
#define ZE_PROC_GEN_H

#include "../common/common.h"

struct ZPGCell
{
    i32 val;
};

struct ZPGGrid
{
    i32 id;
    i32 width;
    i32 height;
    ZPGCell* cells;

    i32 PositionToIndex(i32 x, i32 y)
    {
        if (x < 0 || x >= width) { return -1; }
        if (y < 0 || y >= height) { return -1; }
        return x + (y * width);
    }

    void SetCellAt(i32 x, i32 y, ZPGCell cell)
    {
        i32 i = PositionToIndex(x, y);
        if (i == -1) { return; }


    }

    ZPGCell* GetCellAt(i32 x, i32 y)
    {
        i32 i = PositionToIndex(x, y);
        if (i == -1) { return NULL; }
        return &cells[i];
    }

    void Print()
    {
        printf("-- Grid %d/%d\n", width, height);
        for (i32 y = 0; y < height; ++y)
        {
            for (i32 x = 0; x < width; ++x)
            {
                ZPGCell* cell = GetCellAt(x, y);
                printf("%d", cell->val);
            }
            printf("\n");
        }
    }
};

extern "C" void ZPG_RunTests();
extern "C" ZPGGrid* ZPG_CreateGrid(i32 width, i32 height);

#endif // ZE_PROC_GEN_H