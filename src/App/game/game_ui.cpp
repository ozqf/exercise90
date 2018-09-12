#pragma once

#include "game.h"

void GameUI_SetCentreMessage(char *text)
{
    sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, text);
#if 0
    for (i32 i = 0; i < UI_MAX_ENTITIES; ++i)
    {
        UIEntity* e = &g_ui_entities[i];
        if (e->inUse == 0) { continue; }
        if (!COM_CompareStrings("CentreMSG", e->name))
        {
            COM_CopyStringLimited(text, )
        }
    }
#endif
}

void Game_UpdateHudRingItem(HudRingItem* item, f32 camX, f32 camZ, f32 camDegrees)
{
    /*
    Cam to target angle

    */
   f32 dz = camZ - item->worldPos.z;
   f32 dx = camX - item->worldPos.x;
   //f32 dz = item->worldPos.z - camZ;
   //f32 dx = item->worldPos.x - camX;

    // calcuate angle
    f32 flatAngle = (f32)atan2(dz, dx);
    flatAngle *= RAD2DEG;
    flatAngle = COM_CapAngleDegrees(flatAngle);
    camDegrees += 90;
    camDegrees = COM_CapAngleDegrees(camDegrees);

    //f32 diff = camDegrees - flatAngle;
    f32 diff = flatAngle + camDegrees;

    //item->degrees = flatAngle - camDegrees;
    item->degrees = 90 + diff;
    //item->degrees = flatAngle;
    printf("World Pos %.2f/%.2f Cam Pos: %.2f/%.2f\n",
        item->worldPos.x, item->worldPos.z,
        camX, camZ
    );
    printf("Flat angle: %.2f. Camera: %.2f. Diff: %.2f. Result: %.2f\n",
        flatAngle,
        camDegrees,
        diff,
        item->degrees);

    // setup for drawing
    f32 vx = (f32)cos(item->degrees * DEG2RAD);
    f32 vy = (f32)sin(item->degrees * DEG2RAD);
    item->screenPos.x = vx * item->radius;
    item->screenPos.y = vy * item->radius;
    RendObj_SetAsMesh(
        &g_hudRingRend,
        Assets_GetMeshDataByName("Cube"),
        1, 1, 1,
        AppGetTextureIndexByName("textures\\white_bordered.bmp")
    );
}

void Game_UpdateUI(GameState* gs, UIEntity *ents, i32 maxEntities, GameTime *time)
{
    Client* cl = App_FindLocalClient(&gs->clientList, 1);
    if (cl)
    {
        //EC_Transform* trans = EC_FindTransform(gs, &cl->entId);
        //EC_ActorMotor* motor = EC_FindActorMotor(gs, &cl->entId);
        Vec3 angles = Transform_GetEulerAnglesDegrees(&gs->cameraTransform);
        Game_UpdateHudRingItem(&g_hudRingItem, gs->cameraTransform.pos.x, gs->cameraTransform.pos.z, angles.y);
    }
    else
    {
        Game_UpdateHudRingItem(&g_hudRingItem, 0, 0, 0);
    }

    //printf("Hud ring item degrees: %.2f\n", g_hudRingItem.degrees);
#if 0
    f32 speed = 8.0f;
    Ent* ent = &gs->entList.items[0];
    if (input->yawLeft)
    {
        ent->transform.pos.x -= speed * time->deltaTime;
    }
    if (input->yawRight)
    {
        ent->transform.pos.x += speed * time->deltaTime;
    }

    if (input->pitchUp)
    {
        ent->transform.pos.y += speed * time->deltaTime;
    }
    if (input->pitchDown)
    {
        ent->transform.pos.y -= speed * time->deltaTime;
    }

    if (input->rollLeft)
    {
        ent->transform.pos.z -= speed * time->deltaTime;
    }
    if (input->rollRight)
    {
        ent->transform.pos.z += speed * time->deltaTime;
    }
#endif
}
