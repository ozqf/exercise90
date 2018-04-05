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

void DEBUG_DrawWorldOriginWidget(RenderScene* scene, f32 originX, f32 originY, f32 originZ)
{
    DEBUG_DrawLineToScene(scene, originX, originY, originZ, originX + 1, originY, originZ,
        1, 0, 0, 1, 0, 0);
    DEBUG_DrawLineToScene(scene, originX, originY, originZ, originX, originY + 1, originZ,
        0, 1, 0, 0, 1, 0);
    DEBUG_DrawLineToScene(scene, originX, originY, originZ, originX, originY, originZ + 1,
        0, 0, 1, 0, 0, 1);
}

void DEBUG_DrawAngleVectorsWidget(RenderScene* scene, AngleVectors* vectors, f32 originX, f32 originY, f32 originZ)
{
    Vec3* f = &vectors->forward;
    Vec3* l = &vectors->left;
    Vec3* u = &vectors->up;
    DEBUG_DrawLineToScene(scene, originX, originY, originZ,
        originX + f->x, originY + f->y, originZ + f->z, 
        1, 0, 0, 1, 0, 0);
    DEBUG_DrawLineToScene(scene, originX, originY, originZ,
        originX + l->x, originY + l->y, originZ + l->z, 
        0, 1, 0, 0, 1, 0);
    DEBUG_DrawLineToScene(scene, originX, originY, originZ,
        originX + u->x, originY + u->y, originZ + u->z, 
        0, 0, 1, 0, 0, 1);
}

void Game_IntersectionTest_2D(RenderScene* scene, Vec3* origin, Vec3* dest)
{
    Vec3 lineOrigin;
    Vec3 lineDest;
    if (origin == 0 || dest == 0)
    {
        Vec3 cPos = scene->cameraTransform.pos;
        lineOrigin = { cPos.x - 1, cPos.y + 0.5f, 0 };
        lineDest = { cPos.x + 1, cPos.y + -0.5f, 0 };
    }
    else
    {
        lineOrigin = *origin;
        lineDest = *dest;
    }

    Vec3 boxMin = { -0.5f, -1, -0.5f };
    Vec3 boxMax = { 0.5f, 1, 0.5f };
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
}

void Game_IntersectionTest_3D(RenderScene* scene, Vec3* origin, Vec3* dest)
{
    Vec3 lineOrigin = { -0.25f, 0.5f, -1 };
    Vec3 lineDest = { 0.25f, -0.5f, 1 };
    if (origin != 0 && dest != 0)
    {
        lineOrigin = *origin;
        lineDest = *dest;
    }
    
    Vec3 boxMin = { -0.5f, -1, -0.5f };
    Vec3 boxMax = { 0.5f, 1, 0.5f };
    f32 point3D[3];

    u8 isColliding = LineSegmentVsAABB(
        lineOrigin.x,
        lineOrigin.y,
        lineOrigin.z,
        lineDest.x,
        lineDest.y,
        lineDest.z,
        
        boxMin.x, boxMin.y, boxMin.z,
        boxMax.x, boxMax.y, boxMax.z,
        point3D
    );

    // Line segment
    if (isColliding)
    {
        DEBUG_DrawLineToScene(
            scene,
            lineOrigin.x, lineOrigin.y, lineOrigin.z,
            point3D[0], point3D[1], point3D[2],
            0, 1, 0, 0, 1, 0
        );
        DEBUG_DrawLineToScene(
            scene,
            point3D[0], point3D[1], point3D[2],
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
}

void DEBUG_AngleTestEntity(GameState* gs, RenderScene* scene)
{
    Ent* testEnt = Ent_GetEntityByTag(&gs->entList, 1);
    if (testEnt != NULL)
    {
        AngleVectors vecs;
        Calc_AnglesToAxesZYX(&testEnt->transform.rot, &vecs);
        Vec3 pos = testEnt->transform.pos;
        DEBUG_DrawAngleVectorsWidget(scene, &vecs, pos.x, pos.y, pos.z);
    }

}

void Game_IntersectionTest(GameState* gs, RenderScene* scene)
{
    //Game_IntersectionTest_2D(scene);
    Game_IntersectionTest_3D(scene, NULL, NULL);

    DEBUG_DrawWorldOriginWidget(scene, 4, 0, 0);

    Vec3 rot = scene->cameraTransform.rot;
    //ZSWAPF(&rot.x, &rot.y);
    //rot.x = 0;
    //rot.z = 0;

    AngleVectors vecs;
    Calc_AnglesToAxesZYX(&rot, &vecs);

    DEBUG_DrawAngleVectorsWidget(scene, &vecs, -4, 0, 0);

    DEBUG_AngleTestEntity(gs, scene);

    Vec3 origin = scene->cameraTransform.pos;
    origin.x = origin.x - (vecs.up.x * 0.2f);
    origin.y = origin.y - (vecs.up.y * 0.2f);
    origin.z = origin.z - (vecs.up.z * 0.2f);

    origin.x = origin.x - (vecs.left.x * 0.2f);
    origin.y = origin.y - (vecs.left.y * 0.2f);
    origin.z = origin.z - (vecs.left.z * 0.2f);

    Vec3 dest = {};
    dest.x = origin.x + -(vecs.forward.x * 1);
    dest.y = origin.y + -(vecs.forward.y * 1);
    dest.z = origin.z + -(vecs.forward.z * 1);
    // DEBUG_DrawLineToScene(scene,
    //     origin.x, origin.y, origin.z,
    //     dest.x, dest.y, dest.z,
    //     0, 1, 0, 1, 0, 0);
    
    Game_IntersectionTest_3D(scene, &origin, &dest);
}
