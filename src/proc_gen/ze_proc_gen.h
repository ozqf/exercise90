#ifndef ZE_PROC_GEN_H
#define ZE_PROC_GEN_H

#include "../common/common.h"

extern "C" f32 ZPG_Randf32(i32 index);

struct ZPGPoint
{
    i32 x;
    i32 y;
};

struct ZPGWalkCfg
{
    i32 seed;
    i32 startX;
    i32 startY;
    i32 tilesToPlace;
    char charToPlace;
};

struct ZPGCell
{
    char c;
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

    void SetCellAt(i32 x, i32 y, ZPGCell newCell)
    {
        ZPGCell* cell = GetCellAt(x, y);
        if (cell == NULL)
        {
            printf("No cell at %d, %d\n", x, y);
            return;
        }
        *cell = newCell;
    }

    void SetCellAt(i32 x, i32 y, char c, i32 val)
    {
        ZPGCell cell {};
        cell.c = c;
        cell.val = val;
        SetCellAt(x, y, cell);
    }

    ZPGCell* GetCellAt(i32 x, i32 y)
    {
        i32 i = PositionToIndex(x, y);
        if (i == -1) { return NULL; }
        return &cells[i];
    }

    void MoveWithBounce(ZPGPoint* cursor, ZPGPoint* dir)
    {
        // Step
        cursor->x += dir->x;
        cursor->y += dir->y;
        // if out of grid, move back and change dir on that axis
        // Horizontal:
        if (dir->x != 0)
        {
            // left side
            if (cursor->x < 0)
            {
                cursor->x = 0;
                dir->x = 1;
            }
            // right side
            else if (cursor->x >= width)
            {
                cursor->x = width - 1;
                dir->x = -1;
            }
        }
        // Vertical
        if (dir->y != 0)
        {
            // top
            if (cursor->y < 0)
            {
                cursor->y = 0;
                dir->y = 1;
            }
            // bottom
            else if (cursor->y >= height)
            {
                cursor->y = height - 1;
                dir->y = -1;
            }
        }
    }

    void Print()
    {
        printf("------ Grid %d/%d ------\n", width, height);
        for (i32 y = 0; y < height; ++y)
        {
            printf("|");
            for (i32 x = 0; x < width; ++x)
            {
                ZPGCell* cell = GetCellAt(x, y);
                printf("%c", cell->c);
            }
            printf("|\n");
        }
        printf("------------------\n");
    }
};

extern "C" void ZPG_RunTests();
extern "C" ZPGGrid* ZPG_CreateGrid(i32 width, i32 height);
extern "C" void ZPG_GridRandomWalk(ZPGGrid* grid, ZPGWalkCfg* cfg);

#endif // ZE_PROC_GEN_H