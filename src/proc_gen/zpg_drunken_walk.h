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

extern "C" void ZPG_DrawHorizontalDrunkenPath(ZPGGrid* grid)
{
    ZPGPoint start;
    start.x = 0;
    start.y = grid->height / 2;
    ZPGPoint end;
    end.x = grid->width - 1;
    end.y = grid->height / 2;
}

#endif // ZPG_GRID_H