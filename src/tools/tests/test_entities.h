#pragma once

#include "../../common/common.h"
||

struct SimEntity
{
    Transform t;
    i32 lastThink
    i32 nextThink;
    u8 thinkIndex;
    i32 activeFlags;
    i32 typeIndex;

    SimEntityType* ptrType;
};

struct SimEntityType
{
    i32 factoryIndex;
    i32 thinkIndex;
    i32 thinkTime;

    i32 staticFlags;
    Vec3 scale;

    Colour colourA;
    Colour colourB;
};

struct SimEnt
{
    i32 typeIndex;
};
::