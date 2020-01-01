#ifndef ZE_PROC_GEN_H
#define ZE_PROC_GEN_H

#include "../common/common.h"
//abcdefghijklmnopqrstuvwxyz
//zyxwuvtsrqpomnlkjihgfedabc
struct ZPGPoint
{
    i32 x;
    i32 y;
};

struct ZPGRect
{
    ZPGPoint min;
    ZPGPoint max;
};

struct ZPGEntity
{
    ZPGPoint pos;
    i32 degrees;
    i32 type;
    i32 tag;
};

struct ZPGWalkCfg
{
    i32 seed;
    i32 startX;
    i32 startY;
    i32 tilesToPlace;
    i32 typeToPaint;
    //char charToPlace;
};

#define ZPG_CELL_TYPE_NONE 0
#define ZPG_CELL_TYPE_FLOOR 1
#define ZPG_CELL_TYPE_WALL 2
#define ZPG_CELL_TYPE_WATER 3

#define ZPG_STENCIL_TYPE_EMPTY 0
#define ZPG_STENCIL_TYPE_FULL 1

#define ZPG_CELL_TAG_NONE 0
#define ZPG_CELL_TAG_RANDOM_WALK_START 1
#define ZPG_CELL_TAG_RANDOM_WALK_END 2

struct ZPGCell
{
    // specific class of this cell
    i32 type;
    // how many concentric rings of the same cell surround this cell
    i32 rings;
    // for additional categorisation
    i32 tag;
};

struct ZPGGrid
{
    i32 id;
    i32 width;
    i32 height;
    ZPGCell *cells;

    i32 PositionToIndex(i32 x, i32 y)
    {
        if (x < 0 || x >= width)
        {
            return -1;
        }
        if (y < 0 || y >= height)
        {
            return -1;
        }
        return x + (y * width);
    }

    ZPGCell *GetCellAt(i32 x, i32 y)
    {
        i32 i = PositionToIndex(x, y);
        if (i == -1)
        {
            return NULL;
        }
        return &cells[i];
    }

    void SetCellAt(i32 x, i32 y, ZPGCell newCell)
    {
        ZPGCell *cell = GetCellAt(x, y);
        if (cell == NULL)
        {
            printf("No cell at %d, %d\n", x, y);
            return;
        }
        *cell = newCell;
    }

    void SetCellAt(i32 x, i32 y, i32 type)
    {
        ZPGCell cell{};
        cell.type = type;
        SetCellAt(x, y, cell);
    }

    void SetCellTypeAll(i32 type)
    {
        i32 totalCells = width * height;
        for (i32 i = 0; i < totalCells; ++i)
        {
            cells[i].type = type;
        }
    }

    void SetCellTagAt(i32 x, i32 y, i32 tag)
    {
        ZPGCell *cell = GetCellAt(x, y);
        if (cell == NULL)
        {
            return;
        }
        cell->tag = tag;
    }

    i32 CountNeighboursAt(i32 x, i32 y)
    {
        ZPGCell *cell = GetCellAt(x, y);
        if (cell == NULL)
        {
            return 0;
        }
        i32 matchType = cell->type;
        i32 neighbours = 0;
        cell = GetCellAt(x - 1, y - 1);
        if (cell != NULL && cell->type == matchType)
        {
            neighbours++;
        }
        cell = GetCellAt(x, y - 1);
        if (cell != NULL && cell->type == matchType)
        {
            neighbours++;
        }
        cell = GetCellAt(x + 1, y - 1);
        if (cell != NULL && cell->type == matchType)
        {
            neighbours++;
        }

        cell = GetCellAt(x - 1, y);
        if (cell != NULL && cell->type == matchType)
        {
            neighbours++;
        }
        cell = GetCellAt(x + 1, y);
        if (cell != NULL && cell->type == matchType)
        {
            neighbours++;
        }

        cell = GetCellAt(x - 1, y + 1);
        if (cell != NULL && cell->type == matchType)
        {
            neighbours++;
        }
        cell = GetCellAt(x, y + 1);
        if (cell != NULL && cell->type == matchType)
        {
            neighbours++;
        }
        cell = GetCellAt(x + 1, y + 1);
        if (cell != NULL && cell->type == matchType)
        {
            neighbours++;
        }

        return neighbours;
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
        ZPGCell *cell = GetCellAt(x, y);
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
        for (;;)
        {
            for (i32 iY = -ringTest; iY <= ringTest; ++iY)
            {
                for (i32 iX = -ringTest; iX <= ringTest; ++iX)
                {
                    plotX = x + iX;
                    plotY = y + iY;
                    //printf("  Test %d/%d\n", plotX, plotY);
                    ZPGCell *queryCell = GetCellAt(plotX, plotY);
                    if (queryCell == NULL)
                    {
                        bScanning = NO;
                        break;
                    }
                    if (queryCell == cell)
                    {
                        continue;
                    }
                    if (queryCell->type != cell->type)
                    {
                        //printf("  Char mismatch (%c vs %c)\n",
                        //    cell->c, queryCell->c);
                        bScanning = NO;
                        break;
                    }
                }
                if (bScanning == NO)
                {
                    break;
                }
            }
            if (bScanning == NO)
            {
                break;
            }
            else
            {
                result++;
                ringTest++;
            }
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
                ZPGCell *cell = GetCellAt(x, y);
                cell->rings = CountNeighourRingsAt(x, y);
            }
        }
    }

    char CellToChar(i32 type, i32 tag)
    {
        char c = ' ';
        switch (type)
        {
        //case ZPG_CELL_TYPE_WALL: c = ' '; break;
        case ZPG_CELL_TYPE_WALL:
            c = '#';
            break;
        case ZPG_CELL_TYPE_FLOOR:
            c = ' ';
            switch (tag)
            {
            case ZPG_CELL_TAG_RANDOM_WALK_START:
                c = 's';
                break;
            case ZPG_CELL_TAG_RANDOM_WALK_END:
                c = 'x';
                break;
            default:
                c = ' ';
                break;
            }
            break;
        case ZPG_CELL_TYPE_WATER:
            c = '.';
            break;
        default:
            c = '?';
            break;
        }
        return c;
    }

    void Print()
    {
        printf("------ Grid %d/%d ------\n", width, height);
        for (i32 y = 0; y < height; ++y)
        {
            printf("|");
            for (i32 x = 0; x < width; ++x)
            {
                ZPGCell *cell = GetCellAt(x, y);
                char c = CellToChar(cell->type, cell->tag);
                //printf("%c%d", cell->c, cell->rings);

                printf("%c", c);
            }
            printf("|\n");
        }
        printf("------------------\n");
    }
};

extern "C" void ZPG_RunTests();
extern "C" ZPGGrid *ZPG_CreateGrid(i32 width, i32 height);

extern "C" void ZPG_GridRandomWalk(ZPGGrid* grid, ZPGRect* borderRect, ZPGWalkCfg* cfg, ZPGPoint dir);
extern "C" void ZPG_SeedCaves(ZPGGrid *grid, ZPGGrid *stencil, i32 paintType, i32 *seed);
extern "C" void ZPG_IterateCaves(ZPGGrid *grid, ZPGGrid *stencil, i32 solidType, i32 emptyType);

#endif // ZE_PROC_GEN_H