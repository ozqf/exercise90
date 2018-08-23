#pragma once

#include "game.h"

/**
 * Load relevant entity renderers into rend object list
 */
inline void Game_BuildRenderList(GameState* gs, RenderScene* scene)
{
    u32 l = gs->singleRendObjList.max;
    // Draw Ent render components
    for (u32 i = 0; i < l; ++i)
    {
        EC_SingleRendObj* rend = &gs->singleRendObjList.items[i];
        if (rend->header.inUse == 1)
        {
            //Ent* ent = Ent_GetEntityByIndex(&gs->entList, rend->header.entId.index);
            EC_Transform* entTrans = EC_FindTransform(gs, &rend->header.entId);
            RScene_AddRenderItem(scene, &entTrans->t, &rend->rendObj);
        }
    }
    l = gs->multiRendObjList.max;
    for (u32 i = 0; i < l; ++i)
    {
        EC_MultiRendObj* rend = &gs->multiRendObjList.items[i];
        if (rend->header.inUse == 1)
        {
            EC_Transform* entTrans = EC_FindTransform(gs, &rend->header.entId);
            // TODO: Multi renderer mode selects which rendobjs are active
            // TODO: Calculate final transform for rend object from
            // ent transform + offsets and pitch/yaw overrides
            RScene_AddRenderItem(scene, &entTrans->t, &rend->rendObjs[0]);
            RScene_AddRenderItem(scene, &entTrans->t, &rend->rendObjs[1]);
            RScene_AddRenderItem(scene, &entTrans->t, &rend->rendObjs[2]);
        }
    }
    // Draw local entities
    Game_AddLocalEntitiesToRender(scene);
}

inline void Game_BuildWeaponModelScene(RenderScene* scene)
{
    /* Weapon model 'hand' positions:
    Right: 0.75, -1, -1.5
    Left: -0.75, -1, -1.5
    Centre: 0, -1, -1.5

    */

    Transform t;
    RendObj obj;

    Transform_SetToIdentity(&t);
    t.scale.x = 0.4f;
    t.scale.y = 0.4f;
    //t.pos.x = 0.75;
    t.pos.y = -1;
    t.pos.z = -1.5;
    obj;
    RendObj_SetAsMesh(&obj, g_meshCube, 1, 1, 1, AppGetTextureIndexByName("textures\\W33_5.bmp"));
    RScene_AddRenderItem(scene, &t, &obj);

#if 0
    Transform_SetToIdentity(&t);
    t.scale.x = 0.4f;
    t.scale.y = 0.4f;
    t.pos.x = -0.75;
    t.pos.y = -1;
    t.pos.z = -1.5;
    obj;
    RendObj_SetAsMesh(&obj, &g_meshCube, 1, 1, 1, AppGetTextureIndexByName("textures\\W33_5.bmp"));
    RScene_AddRenderItem(scene, &t, &obj);
#endif
}

