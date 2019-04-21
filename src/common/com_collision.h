#pragma once

#include "common.h"

u8 PointVsAABB(
    f32 pX, f32 pY, f32 pZ,
    f32 aabbX, f32 aabbY, f32 aabbZ,
    f32 aabbHalfX, f32 aabbHalfY, f32 aabbHalfZ
);
u8 AABBVsAABB(
    f32 aX, f32 aY, f32 aZ,
    f32 bX, f32 bY, f32 bZ,
    f32 aHalfX, f32 aHalfY, f32 aHalfZ,
    f32 bHalfX, f32 bHalfY, f32 bHalfZ
);
u8 LineSegmentVsAABB_2D(
    f32 lineOriginX, f32 lineOriginY,
    f32 lineEndX, f32 lineEndY,
    f32 boxMinX,
    f32 boxMinY,
    f32 boxMaxX,
    f32 boxMaxY,
    f32 point[2]
);
u8 LineSegmentVsAABB(
    f32 lineOriginX, f32 lineOriginY, f32 lineOriginZ,
    f32 lineEndX, f32 lineEndY, f32 lineEndZ,
    f32 boxMinX,
    f32 boxMinY,
    f32 boxMinZ,
    f32 boxMaxX,
    f32 boxMaxY,
    f32 boxMaxZ,
    f32 point[3]
);