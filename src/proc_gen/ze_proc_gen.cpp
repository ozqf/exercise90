#ifndef ZE_PROC_GEN_CPP
#define ZE_PROC_GEN_CPP

#include "ze_proc_gen.h"
#include "string.h"
#include "zpg_random_table.h"
#include "zpg_utils.h"
#include "zpg_grid.h"

extern "C" void ZPG_RunTests()
{
    printf("-- ZE PROC GEN TESTS --\n");
    ZPGGrid* grid = ZPG_CreateGrid(64, 32);
    // ZPGCell cell = {};
    // cell.val = 1;
    // cell.c = '#';
    // grid->SetCellAt(16, 16, cell);
    ZPGWalkCfg cfg = {};
    cfg.seed = 0;
    cfg.startX = 16;
    cfg.startY = 16;
    cfg.tilesToPlace = 256;
    cfg.charToPlace = '0';
    ZPG_GridRandomWalk(grid, &cfg);
    ZPG_GridRandomWalk(grid, &cfg);
    cfg.charToPlace = '#';
    ZPG_GridRandomWalk(grid, &cfg);
    ZPG_GridRandomWalk(grid, &cfg);
    

    grid->Print();
}

#endif // ZE_PROC_GEN_CPP