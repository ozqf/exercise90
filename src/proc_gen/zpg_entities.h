#ifndef ZPG_ENTITIES_H
#define ZPG_ENTITIES_H

struct ZPGEntityInfo
{
    ZPGPoint pos;
    i32 i;
    u8 tag;
    f32 avgDist;
};

static i32 ZPG_CompareEntsByDistance(const void* a, const void* b)
{
    return ((ZPGEntityInfo*)a)->avgDist > ((ZPGEntityInfo*)b)->avgDist ? 1 : -1;
}

//static void ZPG_SortFloatArray

static void ZPG_PlaceObjectives(ZPGGrid* grid, ZPGEntityInfo* ents, i32 numEnts)
{
    // record average distances from tile to tile here
    f32* avgDistResults = (f32*)malloc(sizeof(f32) * numEnts);
    // record distances from each tile to every other tile here
    i32 totalDistances = numEnts * numEnts;
    f32* avgDistWorking = (f32*)malloc(sizeof(f32) * totalDistances);

    for (i32 i = 0; i < numEnts; ++i)
    {
        for (i32 j = 0; j < numEnts; ++j)
        {
            i32 distIndex = (i * numEnts) + j;
            f32 dist = 0;
            if (i != j)
            {
                dist = ZPG_Distance(ents[i].pos, ents[j].pos);
            }
            avgDistWorking[distIndex] = dist;
            //printf("Dist objective %d to %d == %.3f\n", i, j, dist);

        }
    }
    for (i32 i = 0; i < numEnts; ++i)
    {
        f32 avg = 0;
        for (i32 j = 0; j < numEnts; ++j)
        {
            i32 distIndex = (i * numEnts) + j;
            avg += avgDistWorking[distIndex];
        }
        avg /= numEnts;
        ents[i].avgDist = avg;
        ents[i].i = i;
    }
    qsort(ents, numEnts, sizeof(ZPGEntityInfo), ZPG_CompareEntsByDistance);
    for (i32 i = 0; i < numEnts; ++i)
    {
        printf("Average distance for ent %d to others: %.3f\n", ents[i].i, ents[i].avgDist);
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
    ZPGEntityInfo* objectives = (ZPGEntityInfo*)malloc(sizeof(ZPGEntityInfo) * grid->stats.numObjectiveTags);
    i32 numObjectives = 0;
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
                objectives[numObjectives].pos.x = x;
                objectives[numObjectives].pos.y = y;
                objectives[numObjectives].tag = cell->tile.tag;
                numObjectives++;
            }
        }
    }
    printf("Build entities found %d path tiles and %d objectives\n",
        floorTilesLen, numObjectives);

    ZPG_PlaceObjectives(grid, objectives, numObjectives);


    // Free working arrays
    free(floorTiles);
    free(objectives);
}

#endif // ZPG_ENTITIES_H