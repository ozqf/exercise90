#ifndef ZPG_UTILS_H
#define ZPG_UTILS_H

#include "ze_proc_gen.h"

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

#endif // ZPG_UTILS_H