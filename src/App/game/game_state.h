#pragma once

#include "game.h"


void GS_Init(GameState *gs)
{
    printf("GAME Init State\n");
    printf("GAME sizeof(Ent): %d\n", sizeof(Ent));
    printf("GAME sizeof(EntityState): %d\n", sizeof(EntityState));
    // Nice to preserve the debug mode!
    u16 debugMode = gs->debugMode;
    *gs = {};
    gs->debugMode = debugMode;
    //gs->debugMode = GAME_DEBUG_MODE_ACTOR_INPUT;
    Transform_SetToIdentity(&gs->cameraTransform);
    Transform_SetPosition(&gs->cameraTransform, 0, -0.5f, 8);
    
    // Don't wanna forget to assign all these.
    // gs->playerList.items = g_players;
    // gs->playerList.count = GAME_MAX_PLAYERS;
    // gs->playerList.max = GAME_MAX_PLAYERS;

    gs->clientList.items = g_clients;
    gs->clientList.count = 0;
    gs->clientList.max = GAME_MAX_CLIENTS;

    gs->entList.items = g_gameEntities;
    gs->entList.count = GAME_MAX_ENTITIES;
    gs->entList.max = GAME_MAX_ENTITIES;
    // MUST do this before using entity list
	Ent_ResetEntityIds(&gs->entList);

    gs->rendererList.items = g_renderers;
    gs->rendererList.count = GAME_MAX_ENTITIES;
    gs->rendererList.max = GAME_MAX_ENTITIES;

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

    COM_ZeroMemory((u8*)gs->rendererList.items, sizeof(EC_Renderer) * gs->rendererList.max);
	COM_ZeroMemory((u8*)gs->colliderList.items, sizeof(EC_Collider) * gs->colliderList.max);
	COM_ZeroMemory((u8*)gs->actorMotorList.items, sizeof(EC_ActorMotor) * gs->actorMotorList.max);
	COM_ZeroMemory((u8*)gs->projectileList.items, sizeof(EC_Projectile) * gs->projectileList.max);
	COM_ZeroMemory((u8*)gs->labelList.items, sizeof(EC_Label) * gs->labelList.max);
    COM_ZeroMemory((u8*)gs->thinkerList.items, sizeof(EC_Thinker) * gs->thinkerList.max);
    
    Game_InitEntityFactory();
}

void GS_Clear(GameState* gs)
{
    GS_Init(gs);
}