#ifndef ZPG_UTILS_H
#define ZPG_UTILS_H

#include "ze_proc_gen.h"

/**
 * returns NO if type at given cell is 0, YES otherwise
 */
static i32 ZPG_CheckStencilOccupied(ZPGGrid* grid, i32 x, i32 y)
{
    if (grid == NULL) { return YES; }
    ZPGCell* cell = grid->GetCellAt(x, y);
    if (cell == NULL) { return YES; }
    return (cell->tile.type != ZPG_CELL_TYPE_NONE);
}

static i32 ZPG_RandomDir(i32* seed)
{
    f32 r = ZPG_Randf32(*seed);
    *seed += 1;
    if (r > 0.5f) { return -1; }
    else { return 1; }
}

static ZPGPoint ZPG_RandomGridCell(ZPGGrid* grid, i32* seed)
{
    ZPGPoint p;
    p.x = (i32)ZPG_Randf32InRange(*seed, 0, (f32)grid->width - 1);
    *seed++;
    p.y = (i32)ZPG_Randf32InRange(*seed, 0, (f32)grid->height - 1);
    *seed++;
    return p;
}

static ZPGPoint ZPG_RandomFourWayDir(i32* seed)
{
    f32 r = ZPG_Randf32(*seed);
    *seed += 1;
    ZPGPoint p = {};
    // left
    if (r < 0.25f) { p.x = -1; }
    // right
    else if (r < 0.5f) { p.x = 1; }
    // up
    else if (r < 0.75f) { p.y = -1; }
    // down
    else { p.y = 1; }
    return p;
}

static ZPGPoint ZPG_RandomThreeWayDir(i32* seed, ZPGPoint curDir)
{
    f32 r = ZPG_Randf32(*seed);
    *seed += 1;
    ZPGPoint p = curDir;
    f32 radians = atan2f((f32)p.y, (f32)p.x);
    f32 degrees = radians * RAD2DEG;
    if (r < 0.33333333f)
    {
        // turn left
        degrees += 90;
        radians = degrees * DEG2RAD;
        p.x = (i32)cosf(radians);
        p.y = (i32)sinf(radians);
    }
    else if (r < 0.6666666f)
    {
        // turn right
        degrees -= 90;
        radians = degrees * DEG2RAD;
        p.x = (i32)cosf(radians);
        p.y = (i32)sinf(radians);
    }
    // (else do nothing)
    return p;
}

extern "C" void ZPG_WriteGridAsAsci(ZPGGrid* grid, char* fileName)
{
    if (grid == NULL) { return; }
    if (fileName == NULL) { return; }
    FILE* f;
    i32 err = fopen_s(&f, fileName, "w");
    if (err != 0)
    {
        printf("Cound not open file %s for writing\n", fileName);
        return;
    }
    for (i32 y = 0; y < grid->height; ++y)
    {
        for (i32 x = 0; x < grid->width; ++x)
        {
            ZPGCell* cell = grid->GetCellAt(x, y);
            char c = grid->CellToChar(cell->tile.type, cell->tile.tag);
            fprintf(f, "%c", c);
        }
        if (y < (grid->height - 1)) { fprintf(f, "\n"); }
    }
    printf("Wrote %d chars to %s\n", ftell(f), fileName);
    fclose(f);
}

static void ZPG_StepGridWithBorder(
    ZPGPoint *cursor, ZPGPoint *dir, ZPGPoint topLeft, ZPGPoint bottomRight)
{
    // Step
    cursor->x += dir->x;
    cursor->y += dir->y;
    // if out of grid, move back and change dir on that axis
    // Horizontal:
    if (dir->x != 0)
    {
        // left side
        if (cursor->x < topLeft.x)
        {
            cursor->x = topLeft.x;
            dir->x = 1;
        }
        // right side
        else if (cursor->x >= bottomRight.x)
        {
            cursor->x = bottomRight.x - 1;
            dir->x = -1;
        }
    }
    // Vertical
    if (dir->y != 0)
    {
        // top
        if (cursor->y < topLeft.y)
        {
            cursor->y = topLeft.y;
            dir->y = 1;
        }
        // bottom
        else if (cursor->y >= bottomRight.y)
        {
            cursor->y = bottomRight.y - 1;
            dir->y = -1;
        }
    }
}

#endif // ZPG_UTILS_H