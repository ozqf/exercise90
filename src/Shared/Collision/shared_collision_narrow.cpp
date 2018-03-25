#pragma once

#include "shared_collision.h"

u8 AABBVsAABB(ZCollider& a, ZCollider& b)
{
    Vec3 bExpandedMin;
    Vec3 bExpandedMax;
    bExpandedMin.x = (b->transform.pos.x - b.aab.halfSize.x) - a.aabb.halfSize.x;
    bExpandedMax.x = (b->transform.pos.x + b.aab.halfSize.x) + a.aabb.halfSize.x;
    bExpandedMin.y = (b->transform.pos.y - b.aab.halfSize.y) - a.aabb.halfSize.y;
    bExpandedMax.y = (b->transform.pos.y + b.aab.halfSize.y) + a.aabb.halfSize.y;
    bExpandedMin.z = (b->transform.pos.z - b.aab.halfSize.z) - a.aabb.halfSize.z;
    bExpandedMax.z = (b->transform.pos.z + b.aab.halfSize.z) + a.aabb.halfSize.z;

    if (
        a->transform.pos.x > bExpandedMin.x &&
        a->transform.pos.x < bExpandedMax.x &&
        a->transform.pos.y > bExpandedMin.y &&
        a->transform.pos.y < bExpandedMax.y &&
        a->transform.pos.z > bExpandedMin.z &&
        a->transform.pos.z < bExpandedMax.z &&
        )
    {
        return 1;
    }
    return 0;
}
