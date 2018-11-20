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
    sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, "WAITING");
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
    GameSession_Init(&g_session);
    GS_Init(&g_gameScene);
    Game_CreateEntityTemplates();
    Game_BuildTestHud();
    Game_BuildTestMenu();
}

internal void Exec_UpdateClient(GameSession* session, GameScene* gs, Cmd_ClientUpdate* cmd)
{
    Client* cl = App_FindOrCreateClient(cmd->clientId, &session->clientList);
    printf("Client avatar was %d/%d now %d/%d\n",
        cl->entId.iteration, cl->entId.index,
        cmd->entId.iteration, cmd->entId.index
    );
    u8 spawn = (cl->entId.value == 0 && cmd->entId.value != 0);
    u8 death = (cl->entId.value != 0 && cmd->entId.value == 0);
    cl->state = cmd->state;
    cl->entId = cmd->entId;
    cl->connectionId = cmd->connectionId;
    printf("GAME EXEC Client %d conn %d, State: %d Avatar id %d/%d\n",
        cl->clientId, cl->connectionId, cl->state, cl->entId.iteration, cl->entId.index);
    
    // Changes that server should act on
    if (spawn)
    {
        SV_ProcessClientSpawn(session->netMode, gs, cl, cmd);
    }
    if (death)
    {
        // deth!
        SV_ProcessClientDeath(session->netMode, gs, &session->clientList, cl, cmd);
    }
    if (cl->flags != cmd->flags)
    {
        // Find changes
        if (COM_IsFlagDifferent(cmd->flags, cl->flags, CLIENT_FLAG_LOCAL))
        {
            // This won't work, remote clients will replicate this and break things!
            //g_localClientId = cmd->clientId;
            printf("GAME Client flag %d changed\n", CLIENT_FLAG_LOCAL);
        }
    }
    cl->flags = cmd->flags;

    if (IS_SERVER)
    {
        printf("SV TRANSMIT cl update to all clients\n");
        NET_MSG_TRANSMIT_TO_ALL_CLIENTS((&session->clientList), cmd);
    }
    
    //SV_OutputToAllClients(session->netMode, &session->clientList, gs, cmd);
}

internal void Exec_UpdateGameInstance(GameScene* gs, Cmd_GameSessionState* cmd)
{
    if (gs->state == cmd->state) { return; }
	printf("GAME Session state changing. %d to %d\n", gs->state, cmd->state);
	gs->state = cmd->state;
    if (cmd->state == GAME_SESSION_STATE_PLAYING)
    {
        sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, "");
    }
    else if (cmd->state == GAME_SESSION_STATE_FAILED)
    {
        sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, "GAME OVER");
    }
    else if (cmd->state == GAME_SESSION_STATE_SUCCESS)
    {
        sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, "SUCCESS");
    }
}

internal void Exec_SpawnViaTemplate(GameScene* gs, Cmd_SpawnViaTemplate* cmd)
{
	
}

internal u8 Game_ReadCmd(GameSession* session, GameScene* gs, CmdHeader* header, u8* ptr)
{
    ClientList* clients = &session->clientList;
    u8 type = *ptr;
    //switch (header->GetType())
    switch (type)
    {
        case CMD_TYPE_ENTITY_STATE_2:
        {
            if (gs->verbose)
            {
                printf("GAME reading Entity state stream cmd (%d bytes)\n", header->GetSize());
            }
            Ent_ReadStateData(gs, ptr, header->GetSize());
            return 1;
        } break;

        case CMD_TYPE_ENTITY_STATE:
        {
            printf("GAME: Defunct dynamic state call\n");
            #if 0
            Cmd_EntityState cmd = {};
            ptr += cmd.Read(header, ptr);
            if (gs->verbose)
            {
                printf("EXEC spawn dynamic ent %d/%d type %d\n",
                    cmd.entityId.iteration, cmd.entityId.index, cmd.factoryType
                );
            }
            Exec_DynamicEntityState(gs, &cmd);
            #endif
			return 1;
        } break;
		
		case CMD_TYPE_SPAWN_VIA_TEMPLATE:
		{
			Cmd_SpawnViaTemplate cmd = {};
			ptr += cmd.Read(ptr);
			Exec_SpawnViaTemplate(gs, &cmd);
			return 1;
		} break;

        case CMD_TYPE_STATIC_STATE:
        {
            printf("GAME: Defunct static state call\n");
            //Cmd_Spawn cmd = {};
            //ptr += cmd.Read(ptr);
            //Exec_StaticEntityState(gs, &cmd);
            return 1;
        } break;

        case CMD_TYPE_REMOVE_ENT:
        {
            Cmd_RemoveEntity cmd = {};
            ptr += cmd.Read(ptr);
            Game_RemoveEntity(session->netMode, clients, gs, &cmd);
            return 1;
        } break;
        
		case CMD_TYPE_PLAYER_INPUT:
		{
            Cmd_PlayerInput cmd;
            u16 measuredSize = cmd.MeasureForReading(ptr);
            APP_ASSERT(measuredSize == header->GetSize(), "Command read size mismatch");
            ptr += cmd.Read(ptr);
            Client* cl = App_FindClientByConnectionId(clients, cmd.connectionId);
            Assert(cl != NULL);
            //Ent* ent = Ent_GetEntityById(&gs->entList, (EntId*)&cl->entIdArr);
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &cl->entId);
			if (motor == NULL)
			{
				printf("!GAME No motor for Entity %d/%d\n",
                    cl->entId.iteration,
                    cl->entId.index);
				return 1;
			}
            motor->state.input = cmd.input;
			return 1;
		} break;
        
        case CMD_TYPE_IMPULSE:
        {
            Cmd_ServerImpulse cmd = {};
            ptr += cmd.Read(ptr);
			return SV_ReadImpulse(session->netMode, clients, gs, &cmd);
        } break;

        case CMD_TYPE_CLIENT_UPDATE:
        {
            Cmd_ClientUpdate cmd = {};
            ptr += cmd.Read(ptr);
            Exec_UpdateClient(session, gs, &cmd);
            return 1;
        } break;
		
		case CMD_TYPE_GAME_SESSION_STATE:
		{
			Cmd_GameSessionState cmd = {};
			ptr += cmd.Read(ptr);
			Exec_UpdateGameInstance(gs, &cmd);
			return 1;
		} break;

        case CMD_TYPE_SPAWN_HUD_ITEM:
        {
            Cmd_SpawnHudItem cmd = {};
            ptr += cmd.Read(ptr);
            Exec_SpawnHudItem(gs, &cmd);
            return 1;
        } break;
    }
    return 0;
}

internal i32 Game_ReadCommandBuffer(GameSession* session, GameScene* gs, ByteBuffer* commands, u8 verbose)
{
    ClientList* clients = &session->clientList;
    i32 numExecuted = 0;
    u8* ptrRead = commands->ptrStart;
    if (verbose)
    {
        u32 size = commands->ptrEnd - commands->ptrStart;
        printf("GAME Reading %d bytes from %s\n", size, App_GetBufferName(commands->ptrStart));
    }
    u32 totalRead = 0;
    while(ptrRead < commands->ptrEnd)
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
                (u32)(commands->ptrEnd - (ptrRead + h.GetSize()))
            );
        }
        
        if (h.GetType() == NULL)
        {
            ptrRead = commands->ptrEnd;
        }
        else
        {
            if (Game_ReadCmd(session, gs, &h, ptrRead) == 0)
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
    GameScene* gs = game->scene;
    ClientList* clients = &game->session->clientList;
    gs->verbose = (u8)time->singleFrame;
    if (time->singleFrame)
    {
        printf("GAME Frame %d\n", time->gameFrameNumber);
    }
	#if 1
    f32 testRadius = 23;
    if (Input_CheckActionToggledOn(actions, "Spawn Test", time->platformFrameNumber))
    {
        
        EntitySpawnOptions options = {};
        options.pos = Game_RandomSpawnOffset(testRadius, 0, testRadius);
        Ent_QuickSpawnCmd(gs, ENTITY_TYPE6_ENEMY, &options);
    }
    if (Input_CheckActionToggledOn(actions, "Spawn Test 2", time->platformFrameNumber))
    {
        EntitySpawnOptions options = {};
        options.pos = Game_RandomSpawnOffset(testRadius, 0, testRadius);
        Ent_QuickSpawnCmd(gs, ENTITY_TYPE8_ENEMY_BRUTE, &options);
    }
    if (Input_CheckActionToggledOn(actions, "Spawn Test 3", time->platformFrameNumber))
    {
        EntitySpawnOptions options = {};
        options.pos = Game_RandomSpawnOffset(testRadius, 0, testRadius);
        Ent_QuickSpawnCmd(gs, ENTITY_TYPE9_ENEMY_CHARGER, &options);
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
	if (time->singleFrame)
	{
		printf("GAME Reading commands from Buffer %s\n", App_GetBufferName(ptrRead));
        printf("GAME Writing commands to Buffer %s\n", App_GetBufferName(output->ptrStart));
	}
    Game_ReadCommandBuffer(game->session, gs, input, (time->singleFrame != 0));
    
    g_debugTransform = gs->cameraTransform;

    //GameScene* gs = &game->scene;
    
    // Game state update
    // Update all inputs, entity components and colliders/physics
    // TODO: Bits of this are order sensitivity. eg projectiles etc hitting health.
    // health must be updated last, so that other events can all affect them!
    Game_UpdateActorMotors(gs, time);
    Game_UpdateAIControllers(clients, gs, time);
    Game_UpdateColliders(gs, time);
    Game_UpdateProjectiles(gs, time);
    Game_UpdateVolumes(gs, time);
    Game_UpdateThinkers(clients, gs, time);
    Game_UpdateRenderObjects(gs, time);
    Game_UpdateSensors(gs, time);
    Game_UpdateHealth(clients, gs, time);

    Game_TickLocalEntities(time->deltaTime, (time->singleFrame == 1));

    // step forward
    Game_StepPhysics(gs, time);

    Client* localClient = App_FindLocalClient(clients, 1);
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
			else if (gs->verbose)
			{
				printf("GAME: Could not find transform for local avatar %d/%d\n", id.iteration, id.index);
			}
            if (hp != NULL)
            {
                sprintf_s(g_playerStatusText, PLAYER_STATUS_TEXT_LENGTH, "HP: %d", hp->state.hp);
            }
        }
        else if (gs->verbose)
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
