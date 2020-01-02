#ifndef ZPG_ENTITIES_H
#define ZPG_ENTITIES_H

struct ZPGEntityInfo
{
    ZPGPoint pos;
    i32 i;
    u8 tag;
    i32 entType;
    f32 avgDist;
};

static i32 ZPG_CompareEntsByDistance(const void* a, const void* b)
{
    return ((ZPGEntityInfo*)a)->avgDist > ((ZPGEntityInfo*)b)->avgDist ? 1 : -1;
}

/**
 * Return none zero if something went wrong
 */
static i32 ZPG_PlaceObjectives(ZPGGrid* grid, ZPGEntityInfo* ents, i32 numEnts)
{
    if (numEnts < 2)
    {
        printf("Must have at least TWO entities to place objectives\n");
        return 1;
    }
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
    // sort list lowest to highest average distance
    qsort(ents, numEnts, sizeof(ZPGEntityInfo), ZPG_CompareEntsByDistance);
    
    // end of list is start
    ents[numEnts - 1].entType = ZPG_ENTITY_TYPE_START;
    ents[numEnts - 2].entType = ZPG_ENTITY_TYPE_END;
    // mark any remaining items to objectives
    i32 numRemainingEnts = numEnts - 2;
    for (i32 i = 0; i < numRemainingEnts; ++i)
    {
        ents[i].entType = ZPG_ENTITY_TYPE_OBJECTIVE;
    }

    // debug
    for (i32 i = 0; i < numEnts; ++i)
    {
        ZPGEntityInfo* info = &ents[i];
        printf("Ent %d type %d pos %d/%d avg dist %.3f\n",
            info->i, info->entType, info->pos.x, info->pos.y, info->avgDist);
        //printf("Average distance for ent %d to others: %.3f\n", ents[i].i, ents[i].avgDist);
    }


    // Cleanup
    free(avgDistWorking);
    return 0;
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
    if (grid->stats.numObjectiveTags < 2)
    {
        printf("Abort: At least two entities are required to place entities\n");
        return;
    }

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

    if (ZPG_PlaceObjectives(grid, objectives, numObjectives) == NO)
    {
        for (i32 i = numObjectives - 1; i >= 0; --i)
        {

        }
    }


    // Free working arrays
    free(floorTiles);
    free(objectives);
}

#endif // ZPG_ENTITIES_H