#pragma once

#include "game.h"

void Game_AddPowerUpOverlayMesh(
    RenderScene* scene,
    Transform* baseT,
    Transform* headT,
    RendObj* baseObj,
    RendObj* headObj
    )
{
    //i32 textureIndex = AppGetTextureIndexByName("textures\\white_latice.bmp");
    i32 textureIndex = AppGetTextureIndexByName("textures\\white_v_lines.bmp");
    //i32 textureIndex = AppGetTextureIndexByName("textures\\white_h_lines.bmp");
    Transform t;
    RendObj obj;

    // Base
    t = *baseT;
    t.scale.x += 0.11f;
    t.scale.y += 0.11f;
    t.scale.z += 0.11f;

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
void Game_BuildRenderList(
    GameScene* gs, RenderScene* scene, GameTime* time)
{
    printf("Time  %.4f\n", time->percentToNextFixedFrame);
    u32 l = gs->singleRendObjList.max;
    // Draw Ent render components
    for (u32 i = 0; i < l; ++i)
    {
        EC_SingleRendObj* rend = &gs->singleRendObjList.items[i];
        if (rend->header.inUse == 1)
        {
            //Ent* ent = Ent_GetEntityByIndex(&gs->entList, rend->header.entId.index);
            EC_Transform* entTrans = EC_FindTransform(gs, &rend->header.entId);

            Transform t = entTrans->t;
            EC_ActorMotor* m = EC_FindActorMotor(gs, &rend->header.entId);
            if (m)
            {
                f32 pitch = m->state.input.degrees.x;
                f32 yaw = m->state.input.degrees.y;
                Transform_SetRotationDegrees(&t, pitch, yaw, 0);
            }

            RendObj_InterpolatePosition(
                &t.pos,
                &rend->rendObj.previousPosition,
                &rend->rendObj.currentPosition,
                time->percentToNextFixedFrame);
            RScene_AddRenderItem(scene, &t, &rend->rendObj);
        }
    }
    l = gs->multiRendObjList.max;
    for (u32 i = 0; i < l; ++i)
    {
        EC_MultiRendObj* rend = &gs->multiRendObjList.items[i];
        if (rend->header.inUse == 1)
        {
            if (gs->cameraEntId.value == rend->header.entId.value) { continue; }
            
            EC_Transform* entTrans = EC_FindTransform(gs, &rend->header.entId);
			Assert(entTrans != NULL);
            // TODO: Multi renderer mode selects which rendobjs are active
            // TODO: Calculate final transform for rend object from
            // ent transform + offsets and pitch/yaw overrides
            RendObj* base = &rend->rendObjs[EC_MULTI_RENDOBJ_BASE];
            RendObj* head = &rend->rendObjs[EC_MULTI_RENDOBJ_HEAD];
            
            Transform baseTrans = entTrans->t;
            Transform headTrans = entTrans->t;
            RendObj_InterpolatePosition(
                &baseTrans.pos,
                &base->previousPosition,
                &base->currentPosition,
                time->percentToNextFixedFrame);
            
            RendObj_InterpolatePosition(
                &headTrans.pos,
                &head->previousPosition,
                &head->currentPosition,
                time->percentToNextFixedFrame);
            
            EC_ActorMotor* m = EC_FindActorMotor(gs, &rend->header.entId);
            if (m != NULL)
            {
                f32 pitch = m->state.input.degrees.x;
                f32 yaw = m->state.input.degrees.y;
                Transform_SetRotationDegrees(&baseTrans, 0, yaw, 0);
                Transform_SetRotationDegrees(&headTrans, pitch, yaw, 0);
            }

            RScene_AddRenderItem(scene, &baseTrans, base);
            
            RScene_AddRenderItem(scene, &headTrans, head);

            // Add FX mesh
            #if 0
            Game_AddPowerUpOverlayMesh(scene, &baseTrans, &headTrans,
                &rend->rendObjs[EC_MULTI_RENDOBJ_BASE],
                &rend->rendObjs[EC_MULTI_RENDOBJ_HEAD]
            );
            #endif
        }
    }
    // Draw local entities
    Game_AddLocalEntitiesToRender(scene);
}

/* Weapon model 'hand' positions:
Right: 0.75, -1, -1.5
Left: -0.75, -1, -1.5
Centre: 0, -1, -1.5
*/
Vec3 g_weaponModelOrigin = { 0.75f, -1, -1.5 };
Vec3 g_weaponModalBack = { 0, -1, -0.6f };
Vec3 g_weaponModalBackMinor = { 0, -1, -1.2f };
Vec3 g_weaponModalCurrent = { 0, -1, -1.5 };

void Game_BuildWeaponModelScene(Client* localClient, GameScene* gs, RenderScene* scene)
{
    f32 z = g_weaponModelOrigin.z;
    if (localClient && localClient->IsPlaying())
    {
        EC_ActorMotor* motor = EC_FindActorMotor(gs, &localClient->entId);
        if (motor)
        {
            Vec3 kickedBackPos;
            if (motor->state.animStyle == 1)
            {
                kickedBackPos = g_weaponModalBack;
            }
            else
            {
                kickedBackPos = g_weaponModalBackMinor;
            }

            f32 max = motor->state.ticker.tickMax;
            f32 step = motor->state.ticker.tick;
            f32 change = kickedBackPos.z - g_weaponModelOrigin.z;
            z = COM_LinearEase(step, -1.5f, change, max);
        }
    }

    Transform t;
    RendObj obj;

    Transform_SetToIdentity(&t);
    t.scale.x = 0.4f;
    t.scale.y = 0.4f;

    t.pos.x = g_weaponModelOrigin.x;
    t.pos.y = g_weaponModelOrigin.y;
    t.pos.z = z;
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

