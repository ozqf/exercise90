#pragma once

#include "../Shared/shared.h"

struct Entity
{
    i32 ID;
    Transform transform;
    f32 speed;
    RendObj rendObj;
    f32 timeToLive;
    f32 tick;
    f32 tock;

    u32 flags;
};
