#ifndef ZPG_ENTITIES_H
#define ZPG_ENTITIES_H

struct ZPGEntityInfo
{
    ZPGPoint pos;
    u8 tag;
    f32 avgDist;
};

static i32 ZPG_CompareFloats(f32 a, f32 b)
{
    return a > b ? 1 : -1;
}

//static void ZPG_SortFloatArray

static void ZPG_PlaceObjectives(ZPGGrid* grid, ZPGPoint* tiles, i32 numTiles)
{
    // record average distances from tile to tile here
    f32* avgDistResults = (f32*)malloc(sizeof(f32) * numTiles);
    // record distances from each tile to every other tile here
    i32 totalDistances = numTiles * numTiles;
    f32* avgDistWorking = (f32*)malloc(sizeof(f32) * totalDistances);

    for (i32 i = 0; i < numTiles; ++i)
    {
        for (i32 j = 0; j < numTiles; ++j)
        {
            i32 distIndex = (i * numTiles) + j;
            f32 dist = 0;
            if (i != j)
            {
                dist = ZPG_Distance(tiles[i], tiles[j]);
            }
            avgDistWorking[distIndex] = dist;
            //printf("Dist objective %d to %d == %.3f\n", i, j, dist);

        }
    }
    for (i32 i = 0; i < numTiles; ++i)
    {
        f32 avg = 0;
        for (i32 j = 0; j < numTiles; ++j)
        {
            i32 distIndex = (i * numTiles) + j;
            avg += avgDistWorking[distIndex];
        }
        avg /= numTiles;
        printf("Average distance for tile %d to others: %.3f\n", i, avg);
    }
}

static void ZPG_PlaceEntities(ZPGGrid* grid)
{
    /*
    Tasks:
    > Build stats of grid and alloc working arrays

    > Main route
        > Scan for objective tags
            > If none are found, randomly place some...somehow.
        > Measure distances between each. Item with greatest average distance is the start.
        > Randomly select one of the remaining objectives as the end.
        > Others become generic 'keys' for opening the end tile.
    > Other objects:
        > Build list of all floor tiles not within a specific range of the start tile
        > Pop a cell from this list and place an object there.
            > Repeat until desired count is placed.
    */
    /////////////////////////////////////////////
    // calculate capacity for working arrays
    grid->CalcStats();

    /////////////////////////////////////////////
    // allocate working arrays
    ZPGPoint* floorTiles = (ZPGPoint*)malloc(sizeof(ZPGPoint) * grid->stats.numFloorTiles);
    i32 floorTilesLen = 0;
    ZPGPoint* objectiveTiles = (ZPGPoint*)malloc(sizeof(ZPGPoint) * grid->stats.numObjectiveTags);
    i32 objectiveTilesLen = 0;
    for (i32 y = 0; y < grid->height; ++y)
    {
        for (i32 x = 0;  x < grid->width; ++x)
        {
            ZPGCell* cell = grid->GetCellAt(x, y);
            if (cell->tile.type != ZPG_CELL_TYPE_FLOOR) { continue; }

            floorTiles[floorTilesLen].x = x;
            floorTiles[floorTilesLen].y = y;
            floorTilesLen++;
            if (cell->tile.tag == ZPG_CELL_TAG_RANDOM_WALK_START
                || cell->tile.tag == ZPG_CELL_TAG_RANDOM_WALK_END)
            {
                objectiveTiles[objectiveTilesLen].x = x;
                objectiveTiles[objectiveTilesLen].y = y;
                objectiveTilesLen++;
            }
        }
    }
    printf("Build entities found %d path tiles and %d objectives\n",
        floorTilesLen, objectiveTilesLen);

    ZPG_PlaceObjectives(grid, objectiveTiles, objectiveTilesLen);


    // Free working arrays
    free(floorTiles);
    free(objectiveTiles);
}

#endif // ZPG_ENTITIES_H