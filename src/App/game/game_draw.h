#pragma once

#include "game.h"

/**
 * Load relevant entity renderers into rend object list
 */
inline void Game_BuildRenderList(GameState* gs, RenderScene* scene)
{
    // Draw Ent render components
    for (u32 i = 0; i < gs->rendererList.max; ++i)
    {
        EC_Renderer* rend = &gs->rendererList.items[i];
        if (rend->header.inUse == 1)
        {
            //Ent* ent = Ent_GetEntityByIndex(&gs->entList, rend->header.entId.index);
            EC_Transform* entTrans = EC_FindTransform(gs, &rend->header.entId);
            RScene_AddRenderItem(scene, &entTrans->t, &rend->rendObj);
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
