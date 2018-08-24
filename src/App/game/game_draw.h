#pragma once

#include "game.h"

inline void Game_AddPowerUpOverlayMesh(
    RenderScene* scene,
    Transform* baseT,
    Transform* headT,
    RendObj* baseObj,
    RendObj* headObj
    )
{
    i32 textureIndex = AppGetTextureIndexByName("textures\\charset.bmp");
    Transform t;
    RendObj obj;

    // Base
    t = *baseT;
    t.scale.x += 0.1f;
    t.scale.y += 0.1f;
    t.scale.z += 0.1f;

    obj = *baseObj;
    obj.data.mesh.textureIndex = textureIndex;
    obj.data.mesh.r = 0.2f;
    obj.data.mesh.g = 0.2f;
    obj.data.mesh.b = 1.0f;
    RScene_AddRenderItem(scene, &t, &obj);

    // Head
    t = *headT;
    t.scale.x += 0.1f;
    t.scale.y += 0.1f;
    t.scale.z += 0.1f;

    obj = *headObj;
    obj.data.mesh.textureIndex = textureIndex;
    obj.data.mesh.r = 0.2f;
    obj.data.mesh.g = 0.2f;
    obj.data.mesh.b = 1.0f;
    RScene_AddRenderItem(scene, &t, &obj);
}

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
			Assert(entTrans != NULL);
            // TODO: Multi renderer mode selects which rendobjs are active
            // TODO: Calculate final transform for rend object from
            // ent transform + offsets and pitch/yaw overrides

            Transform baseTrans = entTrans->t;
            Transform headTrans = entTrans->t;
            baseTrans.pos.y -= 0.5f;
            headTrans.pos.y += 0.5f;

            
            EC_ActorMotor* m = EC_FindActorMotor(gs, &rend->header.entId);
            if (m != NULL)
            {
                f32 pitch = m->state.input.degrees.x;
                f32 yaw = m->state.input.degrees.y;
                Transform_SetRotationDegrees(&baseTrans, 0, yaw, 0);
                Transform_SetRotationDegrees(&headTrans, pitch, yaw, 0);
            }

            RScene_AddRenderItem(scene, &baseTrans, &rend->rendObjs[EC_MULTI_RENDOBJ_BASE]);
            
            RScene_AddRenderItem(scene, &headTrans, &rend->rendObjs[EC_MULTI_RENDOBJ_HEAD]);

            // Add FX mesh
            Game_AddPowerUpOverlayMesh(scene, &baseTrans, &headTrans,
                &rend->rendObjs[EC_MULTI_RENDOBJ_BASE],
                &rend->rendObjs[EC_MULTI_RENDOBJ_HEAD]
            );
            #if 0
            RendObj obj = rend->rendObjs[EC_MULTI_RENDOBJ_HEAD];
            headTrans.scale.x += 0.1f;
            headTrans.scale.y += 0.1f;
            headTrans.scale.z += 0.1f;
            obj.data.mesh.textureIndex = AppGetTextureIndexByName("textures\\charset.bmp");
            
            RScene_AddRenderItem(scene, &headTrans, &obj);
            #endif
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

