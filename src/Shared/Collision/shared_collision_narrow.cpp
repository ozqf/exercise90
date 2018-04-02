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

inline u8 LineSegmentVsAABB_2D(
    f32 lineOriginX, f32 lineOriginY,
    f32 lineEndX, f32 lineEndY,
    f32 boxMinX,
    f32 boxMinY,
    f32 boxMaxX,
    f32 boxMaxY,
    f32 point[2]
)
{
    f32 vx = lineEndX - lineOriginX;
    f32 vy = lineEndY - lineOriginY;
    //f32 vz = lineEndZ - lineOriginZ;

    // avoid divide by zero
    f32 dirFracX = (vx != 0) ? (1.0f / vx) : (ZINFINITY());
    f32 dirFracY = (vy != 0) ? (1.0f / vy) : (ZINFINITY());
    //f32 dirFracZ = (vz != 0) ? (1.0f / vz) : (INFINITE);
    
    f32 t1 = (boxMinX - lineOriginX) * dirFracX;
    f32 t2 = (boxMaxX - lineOriginX) * dirFracX;
    f32 t3 = (boxMinY - lineOriginY) * dirFracY;
    f32 t4 = (boxMaxY - lineOriginY) * dirFracY;

    f32 tMin = ZMAX((ZMIN(t1, t2)), ZMIN(t3, t4));
    f32 tMax = ZMIN((ZMAX(t1, t2)), ZMAX(t3, t4));

    if (tMax < 0)
    {
        return 0;
    }
    else if (tMin > 1)
    {
        return 0;
    }
    else if (tMin > tMax)
    {
        return 0;
    }
    else
    {
        f32 collisionFracX = vx * tMin;
        f32 collisionFracY = vy * tMin;
        // hurray we do stuff now!
        if (point != 0)
        {
            point[0] = lineOriginX + collisionFracX;
            point[1] = lineOriginY + collisionFracY;
        }
        return 1;
    }
}

inline u8 LineSegmentVsAABB(
    f32 lineOriginX, f32 lineOriginY, f32 lineOriginZ,
    f32 lineEndX, f32 lineEndY, f32 lineEndZ,
    f32 boxMinX,
    f32 boxMinY,
    f32 boxMinZ,
    f32 boxMaxX,
    f32 boxMaxY,
    f32 boxMaxZ,
    f32 point[3]
)
{
    f32 vx = lineEndX - lineOriginX;
    f32 vy = lineEndY - lineOriginY;
    f32 vz = lineEndZ - lineOriginZ;

    // avoid divide by zero
    f32 dirFracX = (vx != 0) ? (1.0f / vx) : (ZINFINITY());
    f32 dirFracY = (vy != 0) ? (1.0f / vy) : (ZINFINITY());
    f32 dirFracZ = (vz != 0) ? (1.0f / vz) : (ZINFINITY());
    //f32 dirFracZ = (vz != 0) ? (1.0f / vz) : (INFINITE);
    
    f32 t1 = (boxMinX - lineOriginX) * dirFracX;
    f32 t2 = (boxMaxX - lineOriginX) * dirFracX;
    f32 t3 = (boxMinY - lineOriginY) * dirFracY;
    f32 t4 = (boxMaxY - lineOriginY) * dirFracY;

    f32 t5 = (boxMinZ - lineOriginZ) * dirFracZ;
    f32 t6 = (boxMaxZ - lineOriginZ) * dirFracZ;

    // f32 tMin = ZMAX((ZMIN(t1, t2)), ZMIN(t3, t4));
    // f32 tMax = ZMIN((ZMAX(t1, t2)), ZMAX(t3, t4));

    // ZMAX3(ZMIN(t1, t2), ZMIN(t3, t4), ZMIN(t5, t6));
    // ZMIN3(ZMAX(t1, t2), ZMAX(t3, t4), ZMAX(t5, t6));

    f32 tMin = ZMAX(ZMAX(ZMIN(t1, t2), ZMIN(t3, t4)), ZMIN(t5, t6));
    f32 tMax = ZMIN(ZMIN(ZMAX(t1, t2), ZMAX(t3, t4)), ZMAX(t5, t6));


    if (tMax < 0)
    {
        return 0;
    }
    else if (tMin > 1)
    {
        return 0;
    }
    else if (tMin > tMax)
    {
        return 0;
    }
    else
    {
        f32 collisionFracX = vx * tMin;
        f32 collisionFracY = vy * tMin;
        f32 collisionFracZ = vz * tMin;
        // hurray we do stuff now!
        if (point != 0)
        {
            point[0] = lineOriginX + collisionFracX;
            point[1] = lineOriginY + collisionFracY;
            point[2] = lineOriginZ + collisionFracZ;
        }
        return 1;
    }
}

#if 0
inline u8 RayVsAABB(
    rayOriginX, f32 rayOriginY, f32 rayOriginZ,
    f32 raydirX, f32 raydirY, f32 raydirZ,
    f32 boxMinX, f32 boxMaxX,
    f32 boxMinY, f32 boxMaxY,
    f32 boxMinZ, f32 boxMaxZ
)
{
    return 0;
}
#endif

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
