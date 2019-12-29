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
    cfg.startX = 16;
    cfg.startY = 16;
    cfg.tilesToPlace = 512;//256;

    i32 numRivers = 2;
    i32 numTunnels = 1;
    // Draw "rivers"
    cfg.charToPlace = '.';
    for (i32 i = 0; i < numRivers; ++i)
    {
        ZPG_GridRandomWalk(grid, &cfg);
    }
    // Draw "tunnels"
    cfg.charToPlace = '#';
    for (i32 i = 0; i < numTunnels; ++i)
    {
        ZPG_GridRandomWalk(grid, &cfg);
    }
    printf("Final seed value: %d\n", cfg.seed);
    grid->Print();
    free(grid);
}

extern "C" void ZPG_RunTests()
{
    printf("-- ZE PROC GEN TESTS --\n");
    ZPG_TestDrunkenWalk(0);

    ZPG_TestDrunkenWalk(876987);

    ZPG_TestDrunkenWalk(1993);
}

#endif // ZE_PROC_GEN_CPP