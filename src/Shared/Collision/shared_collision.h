# pragma once

#include "../shared_types.h"

struct ZLineSegment
{
    Vec3 a;
    Vec3 b;
};

struct ZAABB
{
    Vec3 halfSize;
};

// struct ZCollider
// {
//     i32 type;
//     u32 flags;
//     Transform transform;
//     Vec3 velocity;
//     union
//     {
//         ZLineSegment line;
//         ZAABB aabb;
//     };
// };

#include "shared_collision_narrow.cpp"
