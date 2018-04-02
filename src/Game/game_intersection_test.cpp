#pragma once

#include "../Shared/shared.h"
#include "../Shared/Collision/shared_collision.h"

void DEBUG_DrawLineToScene(
    RenderScene* scene,
    f32 x0, f32 y0, f32 z0,
    f32 x1, f32 y1, f32 z1,
    f32 r0, f32 g0, f32 b0,
    f32 r1, f32 g1, f32 b1
)
{
    Transform t;
    Transform_SetToIdentity(&t);
    RendObj obj;

    RendObj_SetAsLine(&obj,
        x0, y0, z0,
        x1, y1, z1,
        r0, g0, b0,
        r1, g1, b1
    );
    RScene_AddRenderItem(scene, &t, &obj);
}

void Game_IntersectionTest(RenderScene* scene)
{
    Vec3 cPos = scene->cameraTransform.pos;
    Vec3 lineOrigin = { cPos.x - 1, cPos.y + 0.5f, 0 };
    Vec3 lineDest = { cPos.x + 1, cPos.y + -0.5f, 0 };

    Vec3 boxMin = { -0.5f, -1, 0 };
    Vec3 boxMax = { 0.5f, 1, 0 };
    f32 point2D[2];
    //f32 point3D[3];

    u8 isColliding = LineSegmentVsAABB_2D(
        lineOrigin.x,
        lineOrigin.y,
        lineDest.x,
        lineDest.y,
        
        boxMin.x, boxMin.y,
        boxMax.x, boxMax.y,
        point2D
    );

    // Line segment
    if (isColliding)
    {
        DEBUG_DrawLineToScene(
            scene,
            lineOrigin.x, lineOrigin.y, lineOrigin.z,
            point2D[0], point2D[1], 0,
            0, 1, 0, 0, 1, 0
        );
        DEBUG_DrawLineToScene(
            scene,
            point2D[0], point2D[1], 0,
            lineDest.x, lineDest.y, lineDest.z,
            1, 0, 0, 1, 0, 0
        );
    }
    else
    {
        DEBUG_DrawLineToScene(
            scene,
            lineOrigin.x, lineOrigin.y, lineOrigin.z,
            lineDest.x, lineDest.y, lineDest.z,
            0, 1, 0, 0, 1, 0
        );
    }

    // AABB itself
    DEBUG_DrawLineToScene(
        scene,
        boxMin.x, boxMin.y, 0,
        boxMin.x, boxMax.y, 0,
        0, 0, 1, 0, 0, 1
    );
    DEBUG_DrawLineToScene(
        scene,
        boxMax.x, boxMin.y, 0,
        boxMax.x, boxMax.y, 0,
        0, 0, 1, 0, 0, 1
    );
    DEBUG_DrawLineToScene(
        scene,
        boxMin.x, boxMin.y, 0,
        boxMax.x, boxMin.y, 0,
        0, 0, 1, 0, 0, 1
    );
    DEBUG_DrawLineToScene(
        scene,
        boxMin.x, boxMax.y, 0,
        boxMax.x, boxMax.y, 0,
        0, 0, 1, 0, 0, 1
    );
    
    // Transform t;
    // Transform_SetToIdentity(&t);
    // RendObj rendObj;
    // RendObj_SetAsLine(&rendObj,
    //     lineOrigin.x, lineOrigin.y, lineOrigin.z,
    //     lineDest.x, lineDest.y, lineDest.z,
    //     colour.x,
    //     colour.y,
    //     colour.z,
    //     colour.x,
    //     colour.y,
    //     colour.z
    // );
    // RScene_AddRenderItem(&g_worldScene, &t, &rendObj);
}