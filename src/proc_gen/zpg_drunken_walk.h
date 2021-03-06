#ifndef ZPG_GRID_H
#define ZPG_GRID_H

#include "ze_proc_gen.h"

static void ZPG_RandomStep(
    ZPGGrid* grid, ZPGGrid* stencil, ZPGRect rect, ZPGPoint* cursor, ZPGPoint* dir, i32* seed)
{
    // if already over a stencil cell, keep moving forward.
    i32 bOverStencil = ZPG_CheckStencilOccupied(stencil, cursor->x, cursor->y);
    if (bOverStencil == YES)
    {
        cursor->x += dir->x;
        cursor->y += dir->y;
        return;
    }

    // form 2 - select a tile to move to, and pick another if not acceptable
    #if 0

    for(;;)
    {
        i32 searchFails = 0;
        ZPGPoint candidate;
        *dir = ZPG_RandomFourWayDir(seed);
        candidate.x = cursor->x + dir->x;
        candidate.y = cursor->y + dir->y;
        i32 bOverStencil = ZPG_CheckStencilOccupied(stencil, candidate->x, candidate->y);
        if (bOverStencil == NO)
        {
            break;
        }
        searchFails++;
        if (searchFails > 99)
        {
            printf("Stuck!\n");
            return;
        }
    }
    #endif

    #if 1
    // form 1 - move around within a rectangular border, with random
    // direction changes.
    ZPG_StepGridWithBorder(cursor, dir, rect.min, rect.max);
    const f32 turnChance = 0.3f;
    f32 changeDirChance = ZPG_Randf32(*seed);
    *seed += 1;
    if (changeDirChance > turnChance)
    {
        *dir = ZPG_RandomThreeWayDir(seed, *dir);
        *seed += 1;
    }
    #endif
}

extern "C" void ZPG_GridRandomWalk(
    ZPGGrid* grid, ZPGGrid* stencil, ZPGRect* borderRect, ZPGWalkCfg* cfg, ZPGPoint dir)
{
    ZPGPoint cursor = { cfg->startX, cfg->startY };
    ZPGPoint lastPos = cursor;
    grid->SetCellTagAt(cursor.x, cursor.y, ZPG_CELL_TAG_RANDOM_WALK_START);

    ZPGRect border;
    // setup border
    if (borderRect == NULL)
    {
        // Default, leave a one tile border around the map
        border.min.x = 1;
        border.min.y = 1;
        border.max.x = grid->width - 1;
        border.max.y = grid->height - 1;
    }
    else
    {
        border = *borderRect;
    }
    

    const i32 escapeCounter = 99999;
    i32 iterations = 0;
    i32 tilesPlaced = 0;
    while (tilesPlaced < cfg->tilesToPlace)
    {
        ZPGCell* cell = grid->GetCellAt(cursor.x, cursor.y);
        if (cell != NULL && cell->tile.type != cfg->typeToPaint)
        {
            f32 r = ZPG_Randf32(cfg->seed++);
            if (r < cfg->bigRoomChance)
            {
                ZPGPoint min = { cursor.x - 1, cursor.y - 1 };
                ZPGPoint max = { cursor.x + 1, cursor.y + 1 };
                ZPG_FillRect(grid, min, max, cfg->typeToPaint);
            }
            else
            {
                grid->SetCellTypeAt(cursor.x, cursor.y, cfg->typeToPaint);
            }
            lastPos = cursor;
            tilesPlaced++;
        }
        ZPG_RandomStep(grid, stencil, border, &cursor, &dir, &cfg->seed);
        iterations++;
        if (iterations >= escapeCounter)
        {
            printf("ABORT! Walk ran away\n");
            break;
        }
    }
    grid->SetCellTagAt(lastPos.x, lastPos.y, ZPG_CELL_TAG_RANDOM_WALK_END);
    //printf("Drunken walk placed %d tiles in %d iterations\n",
    //    tilesPlaced, iterations);
}

static void ZPG_PrintPointsArray(ZPGPoint* points, i32 numPoints)
{
    for (i32 i = 0; i < numPoints; ++i)
    {
        printf("Point %d: %d/%d\n", i, points[i].x, points[i].y);
    }
}

extern "C" void ZPG_PlotSegmentedPath(
    ZPGGrid* grid, i32* seed, ZPGPoint* points, i32 numPoints, i32 bVertical)
{
    i32 numLines = (numPoints - 1);
    if (bVertical == YES)
    {
        // set start
        points[0].x = grid->width / 2;
        points[0].y = 0;
        // set end
        points[numPoints - 1].x = grid->width / 2;
        points[numPoints - 1].y = grid->height - 1;
    }
    else
    {
        // set start
        points[0].x = 0;
        points[0].y = grid->height / 2;
        // set end
        points[numPoints - 1].x = grid->width - 1;
        points[numPoints - 1].y = grid->height / 2;
    }
    
    printf("Drawing Horizontal path from %d/%d to %d/%d\n",
        points[0].x, points[0].y, points[numPoints - 1].x, points[numPoints - 1].y);
    // set mid-line node X positions
    i32 sectionLength;
    if (bVertical == YES) { sectionLength = grid->height / numLines; }
    else { sectionLength = grid->width / numLines; }
    for (i32 i = 1; i < numLines; ++i)
    {
        if (bVertical == YES)
        {
            points[i].x = (i32)ZPG_Randf32InRange(*seed, 0, (f32)(grid->width - 1));
            points[i].y = sectionLength * i;
        }
        else
        {
            points[i].x = sectionLength * i;
            points[i].y = (i32)ZPG_Randf32InRange(*seed, 0, (f32)(grid->height - 1));
        }
        seed++;
    }
}

#endif // ZPG_GRID_H