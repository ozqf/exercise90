#pragma once

#include "sim_internal.h"

internal SimProjectileType Sim_GetProjectileType(i32 index)
{
    SimProjectileType t = {};
    t.speed = 3.5f;
    t.numProjectiles = 4;
    return t;
}
