#ifndef ZE_PROC_GEN_CPP
#define ZE_PROC_GEN_CPP

#include "ze_proc_gen.h"
#include "string.h"
#include "zpg_grid.h"

extern "C" void ZPG_RunTests()
{
    printf("-- ZE PROC GEN TESTS --\n");
    ZPGGrid* grid = ZPG_CreateGrid(32, 32);
    grid->Print();
}

#endif // ZE_PROC_GEN_CPP