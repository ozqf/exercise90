#pragma once

#include "game.h"

internal void GameSession_Clear(GameSession* session)
{
    COM_SET_ZERO(session, sizeof(GameSession));
    // Don't wanna forget to assign all these.
    session->clientList.items = g_clients;
    session->clientList.count = 0;
    session->clientList.max = GAME_MAX_CLIENTS;
}

internal void GS_Init(GameScene *gs)
{
    printf("GAME Init State\n");
    printf("  GAME sizeof(Ent): %d\n", sizeof(Ent));
    printf("  GAME sizeof(EntId): %d\n", sizeof(EntId));
    printf("  GAME sizeof(EntityState): %d\n", sizeof(EntityState));
    printf("  GAME sizeof(LocalEnt): %d\n", sizeof(LocalEnt));
    printf("  GAME sizeof(Cmd_Quick): %d\n", sizeof(Cmd_Quick));
    // Nice to preserve the debug mode!
    u16 debugMode = gs->debugMode;
    *gs = {};
    gs->debugMode = debugMode;
    Transform_SetToIdentity(&gs->cameraTransform);
    Transform_SetPosition(&gs->cameraTransform, 0, -0.5f, 8);

    gs->entList.items = g_gameEntities;
    gs->entList.count = GAME_MAX_ENTITIES;
    gs->entList.max = GAME_MAX_ENTITIES;
    // MUST do this before using entity list
	Ent_ResetEntityIds(&gs->entList);

    gs->transformList.items = g_transforms;
    gs->transformList.count = GAME_MAX_ENTITIES;
    gs->transformList.max = GAME_MAX_ENTITIES;

    gs->singleRendObjList.items = g_renderers;
    gs->singleRendObjList.count = GAME_MAX_ENTITIES;
    gs->singleRendObjList.max = GAME_MAX_ENTITIES;

    gs->aiControllerList.items = g_aiControllers;
    gs->aiControllerList.count = GAME_MAX_ENTITIES;
    gs->aiControllerList.max = GAME_MAX_ENTITIES;

    gs->colliderList.items = g_colliders;
    gs->colliderList.count = GAME_MAX_ENTITIES;
    gs->colliderList.max = GAME_MAX_ENTITIES;

    gs->actorMotorList.items = g_actorMotors;
    gs->actorMotorList.count = GAME_MAX_ENTITIES;
    gs->actorMotorList.max = GAME_MAX_ENTITIES;

    gs->projectileList.items = g_prjControllers;
    gs->projectileList.count = GAME_MAX_ENTITIES;
    gs->projectileList.max = GAME_MAX_ENTITIES;
    
    gs->labelList.items = g_entLabels;
    gs->labelList.count = GAME_MAX_ENTITIES;
    gs->labelList.max = GAME_MAX_ENTITIES;
    
    gs->healthList.items = g_health;
    gs->healthList.count = GAME_MAX_ENTITIES;
    gs->healthList.max = GAME_MAX_ENTITIES;
    
    gs->thinkerList.items = g_thinkers;
    gs->thinkerList.count = GAME_MAX_ENTITIES;
    gs->thinkerList.max = GAME_MAX_ENTITIES;

    gs->multiRendObjList.items = g_multiRenderers;
    gs->multiRendObjList.count = GAME_MAX_ENTITIES;
    gs->multiRendObjList.max = GAME_MAX_ENTITIES;

    gs->volumeList.items = g_volumes;
    gs->volumeList.count = GAME_MAX_ENTITIES;
    gs->volumeList.max = GAME_MAX_ENTITIES;

    gs->sensorList.items = g_sensors;
    gs->sensorList.count = GAME_MAX_ENTITIES;
    gs->sensorList.max = GAME_MAX_ENTITIES;

    // Make sure ALL component arrays are completely cleared here.
    // TODO: Allocate these buffers on the Heap and realloc when restarted
    COM_ZeroMemory((u8*)gs->transformList.items, sizeof(EC_Transform) * gs->transformList.max);
    COM_ZeroMemory((u8*)gs->singleRendObjList.items, sizeof(EC_SingleRendObj) * gs->singleRendObjList.max);
	COM_ZeroMemory((u8*)gs->colliderList.items, sizeof(EC_Collider) * gs->colliderList.max);
    COM_ZeroMemory((u8*)gs->aiControllerList.items, sizeof(EC_AIController) * gs->aiControllerList.max);
	COM_ZeroMemory((u8*)gs->actorMotorList.items, sizeof(EC_ActorMotor) * gs->actorMotorList.max);
    COM_ZeroMemory((u8*)gs->healthList.items, sizeof(EC_Health) * gs->healthList.max);
	COM_ZeroMemory((u8*)gs->projectileList.items, sizeof(EC_Projectile) * gs->projectileList.max);
	COM_ZeroMemory((u8*)gs->labelList.items, sizeof(EC_Label) * gs->labelList.max);
    COM_ZeroMemory((u8*)gs->thinkerList.items, sizeof(EC_Thinker) * gs->thinkerList.max);
    COM_ZeroMemory((u8*)gs->multiRendObjList.items, sizeof(EC_MultiRendObj) * gs->multiRendObjList.max);
    COM_ZeroMemory((u8*)gs->volumeList.items, sizeof(EC_Volume) * gs->volumeList.max);
    COM_ZeroMemory((u8*)gs->sensorList.items, sizeof(EC_Sensor) * gs->sensorList.max);
    
    Game_InitEntityFactory();
}

internal void GS_Clear(GameScene* gs)
{
	Game_ResetLocalEntities();
    GS_Init(gs);
}
