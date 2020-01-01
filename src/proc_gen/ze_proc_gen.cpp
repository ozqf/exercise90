#ifndef ZE_PROC_GEN_CPP
#define ZE_PROC_GEN_CPP

#include "ze_proc_gen.h"
#include "string.h"
#include "zpg_random_table.h"
#include "zpg_utils.h"
#include "zpg_grid.h"

extern "C" void ZPG_TestDrunkenWalk(i32 seed)
{
    ZPGGrid* grid = ZPG_CreateGrid(64, 32);
    // ZPGCell cell = {};
    // cell.val = 1;
    // cell.c = '#';
    // grid->SetCellAt(16, 16, cell);
    ZPGWalkCfg cfg = {};
    cfg.seed = seed;
    cfg.startX = 31;
    cfg.startY = 15;
    cfg.tilesToPlace = 40;//256;

    i32 numRivers = 4;
    i32 numTunnels = 4;
    const i32 numDirections = 4;
    ZPGPoint directions[] =
    {
        { -1, 0 },
        { 1, 0 },
        { 0, -1 },
        { 0, 1 }
    };
    // Draw "rivers"
    cfg.typeToPaint = ZPGCELL_TYPE_WATER;
    //cfg.charToPlace = '.';
    for (i32 i = 0; i < numRivers; ++i)
    {
        //ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
        ZPGPoint dir = directions[i % numDirections];
        ZPG_GridRandomWalk(grid, &cfg, dir);
    }
    // Draw "tunnels"
    cfg.typeToPaint = ZPGCELL_TYPE_FLOOR;
    //cfg.charToPlace = '#';
    for (i32 i = 0; i < numTunnels; ++i)
    {
        //ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
        ZPGPoint dir = directions[i % numDirections];
        ZPG_GridRandomWalk(grid, &cfg, dir);
    }
    printf("Final seed value: %d\n", cfg.seed);
    grid->CountNeighourRings();
    /*
    i32 plotX = 12;
    i32 plotY = 21;
    i32 ringsTest = 0;
    ringsTest = grid->CountNeighourRingsAt(plotX, plotY);
    grid->GetCellAt(plotX, plotY)->c = 'x';
    //grid->GetCellAt(plotX - 1, plotY - 1)->c = 'o';
    printf("Rings at %d/%d: %d\n", plotX, plotY, ringsTest);
    */
    grid->Print();
    free(grid);
}

extern "C" void ZPG_RunTests()
{
    printf("-- ZE PROC GEN TESTS --\n");
    ZPG_TestDrunkenWalk(0);
    //ZPG_TestDrunkenWalk(876987);
    //ZPG_TestDrunkenWalk(1993);
}

#endif // ZE_PROC_GEN_CPP