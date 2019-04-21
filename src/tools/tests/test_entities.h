#pragma once

#include "../../common/common.h"
||

struct SimEntInstance
{
    Transform t;
    i32 nextThink;
};

struct SimEntityType
{
    i32 factoryIndex;
    i32 thinkIndex;
    i32 thinkTime;

    i32 flags;
    Vec3 scale;

    Colour colourA;
    Colour colourB;
};

struct SimEnt
{
    i32 typeIndex;
};
::