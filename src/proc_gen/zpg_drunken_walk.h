#ifndef ZPG_GRID_H
#define ZPG_GRID_H

#include "ze_proc_gen.h"

extern "C" void ZPG_GridRandomWalk(ZPGGrid* grid, ZPGWalkCfg* cfg, ZPGPoint dir)
{
    ZPGPoint cursor = { cfg->startX, cfg->startY };
    ZPGPoint lastPos = cursor;
    grid->SetCellTagAt(cursor.x, cursor.y, ZPGCELL_TAG_RANDOM_WALK_START);
    //ZPGPoint dir = ZPG_RandomFourWayDir(&cfg->seed);
	//printf("Drunken Walk starting angle %.1f\n", (atan2f((f32)dir.y, (f32)dir.x) * RAD2DEG));
    const i32 escapeCounter = 999999;
    i32 iterations = 0;
    i32 tilesPlaced = 0;
    while (tilesPlaced < cfg->tilesToPlace)
    {
        ZPGCell* cell = grid->GetCellAt(cursor.x, cursor.y);
        if (cell != NULL && cell->type != cfg->typeToPaint)
        {
            grid->SetCellAt(cursor.x, cursor.y, cfg->typeToPaint);
            lastPos = cursor;
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
    grid->SetCellTagAt(lastPos.x, lastPos.y, ZPGCELL_TAG_RANDOM_WALK_END);
    printf("Drunken walk placed %d tiles in %d iterations\n",
        tilesPlaced, iterations);
}

static void ZPG_PrintPointsArray(ZPGPoint* points, i32 numPoints)
{
    for (i32 i = 0; i < numPoints; ++i)
    {
        printf("Point %d: %d/%d\n", i, points[i].x, points[i].y);
    }
}

extern "C" void ZPG_DrawHorizontalDrunkenPath(ZPGGrid* grid, i32* seed)
{
    ZPGPoint start;
    start.x = 0;
    start.y = grid->height / 2;
    ZPGPoint end;
    end.x = grid->width - 1;
    end.y = grid->height / 2;

    const i32 numPoints = 10;
    const i32 numLines = (numPoints - 1);
    ZPGPoint points[numPoints];
    // set start
    points[0].x = 0;
    points[0].y = grid->height / 2;
    // set end
    points[numPoints - 1].x = grid->width - 1;
    points[numPoints - 1].y = grid->height / 2;
    printf("Drawing Horizontal path from %d/%d to %d/%d\n",
        points[0].x, points[0].y, points[numPoints - 1].x, points[numPoints - 1].y);
    // set mid-line node X positions
    i32 sectionLength = grid->width / numLines;
    for (i32 i = 1; i < numLines; ++i)
    {
        points[i].x = sectionLength * i;
        points[i].y = (i32)ZPG_Randf32InRange(*seed, 0, (f32)(grid->height - 1));
        seed++;
    }
    ZPG_PrintPointsArray(points, numPoints);
    // Draw lines between nodes
    for (i32 i = 0; i < numLines; ++i)
    {
        ZPGPoint a = points[i];
        ZPGPoint b = points[i + 1];
        printf("Draw %d/%d to %d/%d\n", a.x, a.y, b.x, b.y);
        ZPG_DrawLine(grid, (f32)a.x, (f32)a.y, (f32)b.x, (f32)b.y, ZPGCELL_TYPE_FLOOR);
    }
}

#endif // ZPG_GRID_H