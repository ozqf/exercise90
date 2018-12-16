#pragma once

#include "game.h"

#include "game_intersection_test.cpp"

internal void Game_BuildTestHud()
{
    UIEntity* ent;
    i32 bytesOfUI = sizeof(UIEntity) * UI_MAX_ENTITIES;
    COM_ZeroMemory((u8*)g_ui_entities, bytesOfUI);

    // 0 Crosshair
    ent = UI_GetFreeEntity(g_ui_entities, UI_MAX_ENTITIES);
    ent->transform.pos.z = 0;
    ent->transform.scale = { 0.04f, 0.04f, 0.04f };
    RendObj_SetAsSprite(
        &ent->rendObj,
        SPRITE_MODE_UI,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1);
    RendObj_CalculateSpriteAsciUVs(&ent->rendObj.data.sprite, '+');


    // 1 Info text
    ent = UI_GetFreeEntity(g_ui_entities, UI_MAX_ENTITIES);
    char* chars = "Info\n";
    i32 numChars = COM_StrLen(chars);
    ent->transform.pos.x = 0;
    ent->transform.pos.y = 1;
    RendObj_SetAsAsciCharArray(
        &ent->rendObj,
        chars,
        numChars,
        0.05f,
        TEXT_ALIGNMENT_TOP_LEFT,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1, 1
    );


    // 2 Player Status
    ent = UI_GetFreeEntity(g_ui_entities, UI_MAX_ENTITIES);
    ent->transform.pos.x = -1;
    ent->transform.pos.y = -1;// -(1 - (0.075f * 3));
    COM_CopyStringLimited("CentreMSG", ent->name, 16);
    //char* placeholderChars2 = "LINE 1\nLINE 2\nLINE 3\nLINE 4\nLINE 5";

    RendObj_SetAsAsciCharArray(
        &ent->rendObj, 
        g_playerStatusText,
        -1, 
        0.075f, 
        TEXT_ALIGNMENT_BOTTOM_LEFT,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1, 1
    );
    //printf("ALIGNMENT MODE FOR PLAYER STATUS: %d\n", ent->rendObj.data.charArray.alignmentMode);
    
    // 3 Centre Message
    ent = UI_GetFreeEntity(g_ui_entities, UI_MAX_ENTITIES);
    //char* message = "GAME OVER";
    //numChars = COM_StrLen(chars);
    sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, ""/*"WAITING"*/);
    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0.5f;
    RendObj_SetAsAsciCharArray(
        &ent->rendObj,
        g_hud_centreText,
        0,
        0.2f,
        TEXT_ALIGNMENT_MIDDLE_MIDDLE,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1, 1
    );
    
}

internal void Game_BuildTestMenu()
{
    
}

internal void Game_Init()
{
    GameSession_Clear(&g_session);
    GS_Init(&g_gameScene);
    Game_CreateEntityTemplates();
    Game_BuildTestHud();
    Game_BuildTestMenu();
}

internal i32 Game_ReadCommandBuffer(
    GameSession* session, GameScene* gs, ByteBuffer* commands, u8 verbose)
{
    ClientList* clients = &session->clientList;
    i32 numExecuted = 0;
    u8* ptrRead = commands->ptrStart;
    if (verbose)
    {
        u32 size = commands->ptrWrite - commands->ptrStart;
        printf("GAME Reading %d bytes from %s\n",
            size, App_GetBufferName(commands->ptrStart));
    }
    u32 totalRead = 0;
    while(ptrRead < commands->ptrWrite)
    {
        CmdHeader h = {};
		i32 headerRead = h.Read(ptrRead);
		ptrRead += headerRead;
        totalRead += (sizeof(CmdHeader) + h.GetSize());
        if (verbose)
        {
            printf("  GAME EXEC %d (%d bytes) Total read: %d. Remaining: %d\n",
                h.GetType(),
                h.GetSize(),
                totalRead,
                (u32)(commands->ptrWrite - (ptrRead + h.GetSize()))
            );
        }
        
        if (h.GetType() == NULL)
        {
            printf("  GAME Read cmd type 0. End read\n");
            ptrRead = commands->ptrWrite;
        }
        else
        {
            if (Game_ReadCmd(session, gs, &h, ptrRead) != COM_ERROR_NONE)
            {
                printf("!GAME:  Unknown command type %d...\n", h.GetType());
                ILLEGAL_CODE_PATH
            }
            numExecuted++;
        }
        ptrRead += h.GetSize();
    }
    return numExecuted;
}

internal Ent* Game_GetLocalClientEnt(ClientList* clients, EntList* ents)
{
    Client* localClient = App_FindLocalClient(clients, 1);
    if (localClient)
    {
        Ent* ent = Ent_GetEntityById(ents, &localClient->entId);
        return ent;
    }
    else
    {
        return NULL;
    }
}

internal void Game_SetCameraPosition(GameSession* session, GameScene* gs)
{
    
    Client* localClient = App_FindLocalClient(&session->clientList, 1);
    if (localClient)
    {
        EntId id = localClient->entId;
        gs->cameraEntId = id;
        Ent* ent = Ent_GetEntityById(&gs->entList, &id);
        if (ent != NULL)
        {
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &id);
            EC_Transform* ecT = EC_FindTransform(gs, ent);
            EC_Health* hp = EC_FindHealth(gs, ent);
			if (ecT != NULL)
			{
                Transform_SetByPosAndDegrees(&gs->cameraTransform, &ecT->t.pos, &motor->state.input.degrees);
				// raise camera to eye height
				gs->cameraTransform.pos.y += (1.85f / 2) * 0.8f;
			}
			else if (gs->Verbose())
			{
				printf("GAME: Could not find transform for local avatar %d/%d\n", id.iteration, id.index);
			}
            if (hp != NULL)
            {
                sprintf_s(g_playerStatusText, PLAYER_STATUS_TEXT_LENGTH, "HP: %d", hp->state.hp);
            }
        }
        else if (gs->Verbose())
        {
            printf("GAME: Could not find local avatar %d/%d\n", id.iteration, id.index);
        }
    }
    else
    {
        // Clear attached entity
        gs->cameraEntId.value = 0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Game Tick
/////////////////////////////////////////////////////////////////////////////
internal void Game_Tick(
    Game *game,
    ByteBuffer* input,
    ByteBuffer* output,
    GameTime *time,
    InputActionSet* actions)
{
    i32 verbose = (u8)time->singleFrame;
    GameScene* gs = game->scene;
    ClientList* clients = &game->session->clientList;
    //gs->verbose = 1;
    if (time->singleFrame)
    {
        printf("GAME Frame %d\n", time->gameFrameNumber);
    }
	#if 1
    if (IS_SERVER)
    {
        f32 testRadius = 23;
        if (Input_CheckActionToggledOn(actions, "Spawn Test", time->platformFrameNumber))
        {

            EntitySpawnOptions options = {};
            options.pos = Game_RandomSpawnOffset(testRadius, 0, testRadius);
            Ent_QuickSpawnCmd(
                gs, ENTITY_TYPE6_ENEMY, ENTITY_CATAGORY_REPLICATED, &options);
        }
        if (Input_CheckActionToggledOn(actions, "Spawn Test 2", time->platformFrameNumber))
        {
            EntitySpawnOptions options = {};
            options.pos = Game_RandomSpawnOffset(testRadius, 0, testRadius);
            Ent_QuickSpawnCmd(
                gs, ENTITY_TYPE8_ENEMY_BRUTE, ENTITY_CATAGORY_REPLICATED, &options);
        }
        if (Input_CheckActionToggledOn(actions, "Spawn Test 3", time->platformFrameNumber))
        {
            EntitySpawnOptions options = {};
            options.pos = Game_RandomSpawnOffset(testRadius, 0, testRadius);
            Ent_QuickSpawnCmd(
                gs, ENTITY_TYPE9_ENEMY_CHARGER, ENTITY_CATAGORY_REPLICATED, &options);
        }
    }
	#endif
	#if 0
    if (Input_CheckActionToggledOn(actions, "Attack 2", time->frameNumber))
    {
        i32 shapeId = 10;
        Transform t = gs->cameraTransform;
        Phys_TeleportShape(10, t.pos.x, t.pos.y, t.pos.z);

		Vec3 forward = gs->cameraTransform.rotation.zAxis;
		f32 speed = 20;
		Phys_ChangeVelocity(shapeId, speed * (-forward.x), speed * (-forward.y), speed * (-forward.z));
    }
	#endif

    //////////////////////////////////////////////////////////////////
    // Read Commands
    //////////////////////////////////////////////////////////////////
    u8* ptrRead = input->ptrStart;
	if (verbose)
	{
		printf("GAME Reading commands from Buffer %s (%d bytes)\n",
            App_GetBufferName(ptrRead), input->Written());
        printf("GAME Writing commands to Buffer %s (%d bytes)\n",
            App_GetBufferName(output->ptrStart), output->Written());
	}
    Game_ReadCommandBuffer(game->session, gs, input, (u8)verbose);//(time->singleFrame != 0));
	// Read network stream inputs here...?
    
    g_debugTransform = gs->cameraTransform;
    
    //GameScene* gs = &game->scene;
    
    // Game state update
    // Update all inputs, entity components and colliders/physics
    // TODO: Bits of this are order sensitivity. eg projectiles etc hitting health.
    // health must be updated last, so that other events can all affect them!
    if (IS_SERVER)
    {
        Game_UpdateActorMotors(gs, time);
        Game_UpdateAIControllers(clients, gs, time);
        Game_UpdateColliders(gs, time);
        Game_UpdateProjectiles(gs, time);
        Game_UpdateVolumes(gs, time);
        Game_UpdateThinkers(clients, gs, time);
        Game_UpdateRenderObjects(gs, time);
        Game_UpdateSensors(gs, time);
        Game_UpdateHealth(clients, gs, time);
    }
    else
    {
        // TODO: Client specific update code
        //Game_UpdateActorMotors(gs, time);
        //Game_UpdateAIControllers(clients, gs, time);
        //Game_UpdateColliders(gs, time);
        Game_ClientUpdateProjectiles(gs, time);
        //Game_UpdateVolumes(gs, time);
        //Game_UpdateThinkers(clients, gs, time);
        Game_UpdateRenderObjects(gs, time);
        //Game_UpdateSensors(gs, time);
        //Game_UpdateHealth(clients, gs, time);
    }

    Game_TickLocalEntities(time->deltaTime, (time->singleFrame == 1));

    // step forward
    Game_StepPhysics(gs, time);
    Game_SetCameraPosition(game->session, gs);
}
