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
    i32 rings;
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
    
    /*
    eg:
    0   1     2
    ### ##### #######
    #p# #...# #.....#
    ### #.p.# #.....#
        #...# #..p..#
        ##### #.....#
              #.....#
              #######
    */
    i32 CountNeighourRingsAt(i32 x, i32 y)
    {
        ZPGCell* cell = GetCellAt(x, y);
        if (cell == NULL)
        {
            printf("Cannot Plot ringss at %d/%d - Cell is null\n",
                x, y);
            return 0;
        }
        i32 result = 0;
        i32 ringTest = 1;
        i32 plotX, plotY;
        i32 bScanning = YES;
        // Count until a test fails
        for(;;)
        {
            for (i32 iY = -ringTest; iY <= ringTest; ++iY)
            {
                for (i32 iX = -ringTest; iX <= ringTest; ++iX)
                {
                    plotX = x + iX;
                    plotY = y + iY;
                    //printf("  Test %d/%d\n", plotX, plotY);
                    ZPGCell* queryCell = GetCellAt(plotX, plotY);
                    if (queryCell == NULL) { bScanning = NO; break; }
                    if (queryCell == cell) { continue; }
                    if (queryCell->c != cell->c)
                    {
                        //printf("  Char mismatch (%c vs %c)\n",
                        //    cell->c, queryCell->c);
                        bScanning = NO;
                        break;
                    }
                }
                if (bScanning == NO) { break; }
            }
            if (bScanning == NO) { break; }
            else { result++; ringTest++; }
        }
        return result;
    }

    void CountNeighourRings()
    {
        printf("Calc rings\n");
        for (i32 y = 0; y < height; ++y)
        {
            for (i32 x = 0; x < width; ++x)
            {
                ZPGCell* cell = GetCellAt(x, y);
                cell->rings = CountNeighourRingsAt(x, y);
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
                //printf("%c%d", cell->c, cell->rings);
                printf("%c", cell->c);
            }
            printf("|\n");
        }
        printf("------------------\n");
    }
};

extern "C" void ZPG_RunTests();
extern "C" ZPGGrid* ZPG_CreateGrid(i32 width, i32 height);
extern "C" void ZPG_GridRandomWalk(ZPGGrid* grid, ZPGWalkCfg* cfg, ZPGPoint dir);

#endif // ZE_PROC_GEN_H