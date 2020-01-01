#ifndef ZPG_UTILS_H
#define ZPG_UTILS_H

#include "ze_proc_gen.h"

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

#endif // ZPG_UTILS_H