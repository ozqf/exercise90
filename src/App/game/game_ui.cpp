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

internal void Hud_RecycleRingItem(HudRingItem* item)
{
    item->state = 0;
}

internal void Hud_ClearAllHudRings()
{
    for (int i = 0; i < HUD_MAX_RING_ITEMS; ++i)
    {
        g_hudRingItems[i].state = 0;
    }
}

internal HudRingItem* Hud_GetFreeHudRingItem()
{
    HudRingItem* item = NULL;
    int oldestIndex = INT_MAX;
    for (int i = 0; i < HUD_MAX_RING_ITEMS; ++i)
    {
        if (g_hudRingItems[i].state == 0)
        {
            g_hudRingItems[i].state = 1;
            return &g_hudRingItems[i];
        }
        else
        {
            oldestIndex = i;
        }
    }
    return &g_hudRingItems[oldestIndex];
}

internal void Exec_SpawnHudItem(GameScene* gs, Cmd_SpawnHudItem* cmd)
{
    HudRingItem* r = Hud_GetFreeHudRingItem();
    if (!r) { return; }
    r->radius = 0.5f;
    r->worldPos.x = cmd->pos.x;
    r->worldPos.y = cmd->pos.y;
    r->worldPos.z = cmd->pos.z;

    r->tick = 0;
    r->tickMax = 1.0f;
    r->startScale = 1;
    r->endScale = 0;
}

internal void Game_UpdateHudRingItem(HudRingItem* item, f32 camX, f32 camZ, f32 camDegrees, f32 deltaTime)
{
    if (item->tick >= item->tickMax)
    {
        Hud_RecycleRingItem(item);
    }
    item->scale = COM_LinearEase(item->tick, item->startScale, item->endScale - item->startScale, item->tickMax);
    /*
    Cam to target angle
	- lets learn some basic trigonometry
    */
	
	f32 dz = camZ - item->worldPos.z;
	f32 dx = camX - item->worldPos.x;
   
    // calcuate angle - Careful about axis order in atan2
	// a yaw of 0 == facing down Z axis, with the 'secondary' being the X
	// so y == x, x == z
    f32 flatAngle = (f32)atan2(dx, dz);
    flatAngle *= RAD2DEG;
    flatAngle = COM_CapAngleDegrees(flatAngle);
    camDegrees = COM_CapAngleDegrees(camDegrees);
	
    f32 diff = flatAngle - camDegrees;
	
	// offset by 90 so that screen top == forward
    item->degrees = 90 + diff;
    
	#if 0
    printf("World Pos %.2f/%.2f Cam Pos: %.2f/%.2f. Flat angle: %.2f. Camera: %.2f. Diff: %.2f. Result: %.2f\n",
		item->worldPos.x, item->worldPos.z,
        camX, camZ,
        flatAngle,
        camDegrees,
        diff,
        item->degrees);
	#endif

    // setup for drawing
    f32 vx = (f32)cos(item->degrees * DEG2RAD);
    f32 vy = (f32)sin(item->degrees * DEG2RAD);
    item->screenPos.x = vx * item->radius;
    item->screenPos.y = vy * item->radius;

    RendObj_SetAsMesh(
        &item->rendObj,
        Assets_GetMeshDataByName("Cube"),
        1, 0.2f, 0.2f,
        AppGetTextureIndexByName("textures\\white_bordered.bmp")
    );

    item->tick += deltaTime;
}

internal void Game_UpdateUI(GameScene* gs, UIEntity *ents, i32 maxEntities, GameTime *time)
{
    Client* cl = App_FindLocalClient(&gs->clientList, 1);
    if (cl)
    {
        //EC_Transform* trans = EC_FindTransform(gs, &cl->entId);
        //EC_ActorMotor* motor = EC_FindActorMotor(gs, &cl->entId);
        Vec3 angles = Transform_GetEulerAnglesDegrees(&gs->cameraTransform);
        for (int i = 0; i < HUD_MAX_RING_ITEMS; ++i)
        {
            HudRingItem* item = &g_hudRingItems[i];
            Game_UpdateHudRingItem(
               item,
               gs->cameraTransform.pos.x,
               gs->cameraTransform.pos.z,
               angles.y,
               time->deltaTime
        )   ;
        }
        
    }
    else
    {
        Hud_ClearAllHudRings();
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
