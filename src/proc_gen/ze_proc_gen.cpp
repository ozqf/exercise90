#ifndef ZE_PROC_GEN_CPP
#define ZE_PROC_GEN_CPP

#include "ze_proc_gen.h"
#include "string.h"
#include "time.h"
#include "zpg_random_table.h"
#include "zpg_utils.h"
#include "zpg_draw_grid_primitives.h"
#include "zpg_perlin_draw.h"
#include "zpg_entities.h"
#include "zpg_drunken_walk.h"
#include "zpg_cave_gen.h"

extern "C" ZPGGrid* ZPG_CreateGrid(i32 width, i32 height)
{
    i32 totalCells = width * height;
    i32 memForGrid = (sizeof(ZPGCell) * totalCells);
    i32 memTotal = sizeof(ZPGGrid) + memForGrid;
    printf("Make grid %d by %d (%d cells, %d bytes)\n",
        width, height, (width * height), memTotal);
    u8* ptr = (u8*)malloc(memTotal);
    // Create grid struct
    ZPGGrid* grid = (ZPGGrid*)ptr;
    *grid = {};
    // init grid memory
    ptr += sizeof(ZPGGrid);
    //memset(ptr, ' ', memForGrid);
    grid->cells = (ZPGCell*)ptr;
    for (i32 i = 0; i < totalCells; ++i)
    {
        grid->cells[i] = {};
        grid->cells[i].tile.type = ZPG_CELL_TYPE_WALL;
        grid->cells[i].tile.tag = ZPG_CELL_TAG_NONE;
    }
    grid->width = width;
    grid->height = height;
    return grid;
}

static ZPGGrid* ZPG_CreateBorderStencil(i32 width, i32 height)
{
    ZPGGrid* stencil = ZPG_CreateGrid(width, height);
    stencil->SetCellTypeAll(ZPG_CELL_TYPE_NONE);
    ZPG_DrawOuterBorder(stencil, ZPG_CELL_TYPE_FLOOR);
    return stencil;
}

extern "C" ZPGGrid* ZPG_TestDrunkenWalk_FromCentre(i32 seed)
{
    printf("Generate: Drunken walk - start from centre\n");
    const i32 width = 64;
    const i32 height = 32;
    ZPGGrid* grid = ZPG_CreateGrid(width, height);
    ZPGGrid* stencil = ZPG_CreateBorderStencil(width, height);
    ZPG_FillRect(stencil, { 16, 8 }, { 48, 24 }, ZPG_CELL_TYPE_FLOOR );
    stencil->PrintValues();
    ZPGWalkCfg cfg = {};
    cfg.seed = seed;
    cfg.startX = 31;
    cfg.startY = 15;
    cfg.bigRoomChance = 0.01f;
    cfg.tilesToPlace = 40;//256;

    i32 numRivers = 4;
    i32 numPaths = 4;
    printf("Drawing %d rivers and %d paths\n", numRivers, numPaths);
    const i32 numDirections = 4;
    ZPGPoint directions[] =
    {
        { -1, 0 },
        { 1, 0 },
        { 0, -1 },
        { 0, 1 }
    };
    // Draw "rivers"
    cfg.typeToPaint = ZPG_CELL_TYPE_WATER;
    cfg.bigRoomChance = 0.1f;
    //cfg.bigRoomChance = 0;
    //cfg.charToPlace = '.';
    for (i32 i = 0; i < numRivers; ++i)
    {
        //ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
        ZPGPoint dir = directions[i % numDirections];
        ZPG_GridRandomWalk(grid, NULL, NULL, &cfg, dir);
    }
    // Draw "tunnels"
    cfg.typeToPaint = ZPG_CELL_TYPE_FLOOR;
    //cfg.charToPlace = '#';
    for (i32 i = 0; i < numPaths; ++i)
    {
        //ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
        ZPGPoint dir = directions[i % numDirections];
        ZPG_GridRandomWalk(grid, stencil, NULL, &cfg, dir);
    }
    //printf("Final seed value: %d\n", cfg.seed);
    return grid;
}

/**
 * Define some rects within a grid and random walk within
 */
extern "C" ZPGGrid* ZPG_TestDrunkenWalk_WithinSpace(i32 seed)
{
    printf("Generate: Drunken walk - Within borders\n");
    ZPGGrid* grid = ZPG_CreateGrid(72, 32);
    i32 halfWidth = grid->width / 2;
    i32 halfHeight = grid->height / 2;
    const i32 numSquares = 4;
    ZPGRect squares[numSquares];
    i32 borderSize = 1;
    // top left
    squares[0].min.x = borderSize;
    squares[0].min.y = borderSize;
    squares[0].max.x = halfWidth - (borderSize + 1);
    squares[0].max.y = halfHeight - (borderSize + 1);
    // top right
    squares[1].min.x = halfWidth + borderSize;
    squares[1].min.y = (0) + borderSize;
    squares[1].max.x = grid->width - (borderSize + 1);
    squares[1].max.y = halfHeight - (borderSize + 1);
    // bottom left
    squares[2].min.x = (0) + borderSize;
    squares[2].min.y = halfHeight + borderSize;
    squares[2].max.x = halfWidth - (borderSize + 1);
    squares[2].max.y = grid->height - (borderSize + 1);
    // bottom right
    squares[3].min.x = halfWidth + borderSize;
    squares[3].min.y = halfHeight + borderSize;
    squares[3].max.x = grid->width - (borderSize + 1);
    squares[3].max.y = grid->height - (borderSize + 1);


    ZPGWalkCfg cfg = {};
    cfg.seed = seed;
    cfg.tilesToPlace = 120;//256;
    cfg.typeToPaint = ZPG_CELL_TYPE_FLOOR;
    for (i32 i = 0; i < numSquares; ++i)
    {
        ZPGRect rect = squares[i];
        //ZPGRect rect = squares[1];
        printf("Draw in rect %d/%d to %d/%d\n", rect.min.x, rect.min.y, rect.max.x, rect.max.y);
        ZPG_DrawRect(grid, rect.min, rect.max, ZPG_CELL_TYPE_WATER);
        ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
        ZPGPoint centre = rect.Centre();
        cfg.startX = centre.x;
        cfg.startY = centre.y;
        ZPG_GridRandomWalk(grid, NULL, &rect, &cfg, dir);
    }
    /*
    ZPGRect rect;
    rect.min.x = quarterWidth;
    rect.min.y = quarterHeight;
    rect.max.x = quarterWidth * 3;
    rect.max.y = quarterHeight * 3;
    ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
    ZPGPoint centre = rect.Centre();
    printf("Quarter size: %d/%d\n", quarterWidth, quarterHeight);
    printf("Draw from %d/%d\n", centre.x, centre.y);
    cfg.startX = (i32)ZPG_Randf32InRange(cfg.seed++, 0, (f32) centre.x);
    cfg.startY = (i32)ZPG_Randf32InRange(cfg.seed++, 0, (f32) centre.y);
    cfg.startX = centre.x;
    cfg.startY = centre.y;
    ZPG_GridRandomWalk(grid, &rect, &cfg, dir);
    */
   return grid;
}

extern "C" ZPGGrid* ZPG_TestDrunkenWalk_Scattered(i32 seed)
{
    printf("Generate: Drunken walk - scattered starting points\n");
    ZPGGrid* grid = ZPG_CreateGrid(64, 32);
    ZPGWalkCfg cfg = {};
    cfg.seed = seed;
    cfg.tilesToPlace = 80;//256;
    cfg.typeToPaint = ZPG_CELL_TYPE_FLOOR;
    
    i32 numRivers = 4;
    i32 numPaths = 4;
    printf("Drawing %d rivers and %d paths\n", numRivers, numPaths);
    // Rivers
    cfg.typeToPaint = ZPG_CELL_TYPE_WATER;
    for (i32 i = 0; i < numPaths; ++i)
    {
        ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
        cfg.startX = (i32)ZPG_Randf32InRange(cfg.seed++, 0, 63);
        cfg.startY = (i32)ZPG_Randf32InRange(cfg.seed++, 0, 31);
        ZPG_GridRandomWalk(grid, NULL, NULL, &cfg, dir);
    }
    // Paths
    cfg.typeToPaint = ZPG_CELL_TYPE_FLOOR;
    for (i32 i = 0; i < numPaths; ++i)
    {
        ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
        cfg.startX = (i32)ZPG_Randf32InRange(cfg.seed++, 0, 63);
        cfg.startY = (i32)ZPG_Randf32InRange(cfg.seed++, 0, 31);
        ZPG_GridRandomWalk(grid, NULL, NULL, &cfg, dir);
    }
    return grid;
}

extern "C" ZPGGrid* ZPG_TestCaveGen(i32 seed)
{
    // Create canvas
    ZPGGrid* grid = ZPG_CreateGrid(72, 32);
    grid->SetCellTypeAll(ZPG_CELL_TYPE_WALL);
    // Stencil grid - blocks writing of tiles
    ZPGGrid* stencil = ZPG_CreateBorderStencil(72, 32);
    //ZPGGrid* stencil = ZPG_CreateGrid(72, 32);
    //stencil->SetCellTypeAll(ZPG_CELL_TYPE_NONE);
    //ZPG_DrawOuterBorder(stencil, ZPG_CELL_TYPE_FLOOR);

    ZPG_SeedCaves(grid, stencil, ZPG_CELL_TYPE_FLOOR, &seed);
    grid->PrintChars();
    i32 numIterations = 2;
    for (i32 i = 0; i < numIterations; ++i)
    {
        ZPG_IterateCaves(grid, stencil, ZPG_CELL_TYPE_WALL, ZPG_CELL_TYPE_FLOOR);
    }
    return grid;
}

extern "C" ZPGGrid* ZPG_TestDrawOffsetLines()
{
    const i32 width = 72;
    const i32 height = 32;
    ZPGGrid* grid = ZPG_CreateGrid(width, height);
    grid->SetCellTypeAll(ZPG_CELL_TYPE_WALL);

    i32 seed = 0;
    const i32 numPoints = 8;
    i32 numLines = numPoints - 1;
    i32 numRivers = 4;
    i32 numTilesPerRiver = 80;
    i32 numTilesPerPath = 40;

    // Draw rivers
    ZPGWalkCfg cfg = {};
    cfg.seed = seed;
    cfg.tilesToPlace = numTilesPerRiver;
    cfg.typeToPaint = ZPG_CELL_TYPE_WATER;

    for (i32 i = 0; i < numRivers; ++i)
    {
        ZPGPoint dir = ZPG_RandomFourWayDir(&cfg.seed);
        ZPGPoint p = ZPG_RandomGridCell(grid, &cfg.seed);
        cfg.startX = p.x;
        cfg.startY = p.y;
        ZPG_GridRandomWalk(grid, NULL, NULL, &cfg, dir);
    }

    // Draw main path
    i32 bVertical = NO;
    ZPGPoint* points = (ZPGPoint*)malloc(sizeof(ZPGPoint) * numPoints);
    ZPG_PlotSegmentedPath(grid, &seed, points, numPoints, bVertical);
    ZPG_DrawSegmentedLine(grid, points, numPoints);

    // Draw side paths
    cfg.tilesToPlace = numTilesPerPath;
    cfg.typeToPaint = ZPG_CELL_TYPE_FLOOR;
    for (i32 i = 1; i < numLines; ++i)
    {
        ZPGPoint dir = {};
        if (bVertical == YES)
        {
            dir.x = ZPG_RandomDir(&cfg.seed);
        }
        else
        {
            dir.y = ZPG_RandomDir(&cfg.seed);
        }
        cfg.startX = points[i].x;
        cfg.startY = points[i].y;
        ZPG_GridRandomWalk(grid, NULL, NULL, &cfg, dir);
    }
    return grid;
}

extern "C" ZPGGrid* ZPG_TestDrawLines()
{
    ZPGGrid* grid = ZPG_CreateGrid(72, 32);
    grid->SetCellTypeAll(ZPG_CELL_TYPE_WALL);

    ZPG_DrawOuterBorder(grid, ZPG_CELL_TYPE_FLOOR);
    ZPG_DrawLine(grid, 0, 0, 71, 31, ZPG_CELL_TYPE_FLOOR);
    return grid;
}

static ZPGGrid* ZPG_TestPerlin(i32 seed)
{
    ZPGGrid* grid = ZPG_CreateGrid(72, 32);
    grid->SetCellTypeAll(ZPG_CELL_TYPE_WALL);

    ZPGGrid* stencil = ZPG_CreateGrid(72, 32);
    stencil->SetCellTypeAll(ZPG_CELL_TYPE_NONE);
    ZPG_DrawOuterBorder(stencil, ZPG_CELL_TYPE_WALL);
    ZPG_DrawPerlinGrid(grid, stencil, &seed);
    ZPG_IterateCaves(grid, stencil, ZPG_CELL_TYPE_WALL, ZPG_CELL_TYPE_FLOOR);
    return grid;
}

extern "C" void ZPG_RunTests()
{
    // Seed rand
    srand((i32)time(NULL));

    const i32 mode = 1;
    i32 seed = 0;
    printf("-- ZE PROC GEN TESTS --\n");
    ZPGGrid* grid = NULL;
    i32 bPrintChars = YES;
    switch (mode)
    {
        case 1: grid = ZPG_TestDrunkenWalk_FromCentre(0); break;
        case 2: grid = ZPG_TestDrunkenWalk_Scattered(0); break;
        case 3: grid = ZPG_TestCaveGen(seed); break;
        case 4: grid = ZPG_TestDrawOffsetLines(); break;
        case 5: grid = ZPG_TestDrawLines(); break;
        case 6: grid = ZPG_TestDrunkenWalk_WithinSpace(seed); break;
        case 7: grid = ZPG_TestPerlin(seed);  break;
    }
    
    grid->CountNeighourRings();
    ZPG_PlaceScatteredEntities(grid, &seed);

    if (grid != NULL)
    {
        if (bPrintChars)
        {
            grid->PrintChars();    
        }
        else
        {
            grid->PrintValues();
        }
        ZPG_WriteGridAsAsci(grid, "test_grid.txt");
        free(grid);
    }

    //ZPG_TestDrunkenWalk(876987);
    //ZPG_TestDrunkenWalk(1993);
}

#endif // ZE_PROC_GEN_CPP