#pragma once

#include "shared_collision.h"

inline u8 PointVsAABB(
    f32 pX, f32 pY, f32 pZ,
    f32 aabbX, f32 aabbY, f32 aabbZ,
    f32 aabbHalfX, f32 aabbHalfY, f32 aabbHalfZ
    )
{
    if (
        pX > (aabbX - aabbHalfX) &&
        pX < (aabbX + aabbHalfX) &&
        pY > (aabbY - aabbHalfY) &&
        pY < (aabbY + aabbHalfY) &&
        pZ > (aabbZ - aabbHalfZ) &&
        pZ < (aabbZ + aabbHalfZ)
        )
    {
        return 1;
    }
    return 0;
}

inline u8 AABBVsAABB(
    f32 aX, f32 aY, f32 aZ,
    f32 bX, f32 bY, f32 bZ,
    f32 aHalfX, f32 aHalfY, f32 aHalfZ,
    f32 bHalfX, f32 bHalfY, f32 bHalfZ)
{
    Vec3 bExpandedMin;
    Vec3 bExpandedMax;
    bExpandedMin.x = (bX - bHalfX) - aHalfX;
    bExpandedMax.x = (bX + bHalfX) + aHalfX;
    bExpandedMin.y = (bY - bHalfY) - aHalfY;
    bExpandedMax.y = (bY + bHalfY) + aHalfY;
    bExpandedMin.z = (bZ - bHalfZ) - aHalfZ;
    bExpandedMax.z = (bZ + bHalfZ) + aHalfZ;

    if (
        aX > bExpandedMin.x &&
        aX < bExpandedMax.x &&
        aY > bExpandedMin.y &&
        aY < bExpandedMax.y &&
        aZ > bExpandedMin.z &&
        aZ < bExpandedMax.z
        )
    {
        return 1;
    }
    return 0;
}

#if 0
inline u8 AABBVsAABB(ZCollider* a, ZCollider* b)
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
#endif
