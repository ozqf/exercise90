#ifndef ZE_PROC_GEN_CPP
#define ZE_PROC_GEN_CPP

#include "ze_proc_gen.h"
#include "string.h"
#include "time.h"
#include "zpg_random_table.h"
#include "zpg_utils.h"
#include "zpg_grid.h"
#include "zpg_cave_gen.h"

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

extern "C" void ZPG_TestCaveGen(i32 seed)
{
    srand((i32)time(NULL));
    ZPGGrid* grid = ZPG_CreateGrid(72, 32);
    grid->SetCellTypeAll(ZPGCELL_TYPE_WALL);
    ZPG_SeedCaves(grid, ZPGCELL_TYPE_FLOOR, &seed);
    grid->Print();
    i32 numIterations = 3;
    for (i32 i = 0; i < numIterations; ++i)
    {
        ZPG_IterateCaves(grid, ZPGCELL_TYPE_WALL, ZPGCELL_TYPE_FLOOR);
    }
    grid->Print();
    free(grid);
}

extern "C" void ZPG_RunTests()
{
    const i32 mode = 1;
    const i32 seed = 0;
    printf("-- ZE PROC GEN TESTS --\n");
    switch (mode)
    {
        case 1: ZPG_TestCaveGen(seed); break;
        case 2: ZPG_TestDrunkenWalk(0); break;
    }
    
    //ZPG_TestDrunkenWalk(876987);
    //ZPG_TestDrunkenWalk(1993);
}

#endif // ZE_PROC_GEN_CPP