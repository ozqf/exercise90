#ifndef ZPG_ENTITIES_H
#define ZPG_ENTITIES_H

static void ZPG_PlaceEntities()
{
    /*
    Tasks:
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
}

#endif // ZPG_ENTITIES_H