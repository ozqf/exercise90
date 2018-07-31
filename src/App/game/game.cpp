#pragma once

#include "game.h"

#include "game_intersection_test.cpp"

void Game_AddTestSolid(GameState* gs,
	f32 x, f32 y, f32 z,
	f32 halfSizeX, f32 halfSizeY, f32 halfSizeZ)
{
	u8* headerPos = App_StartCommandStream();
	u32 size = 0;
	Cmd_EntityStateHeader h = {};
	
	h.entId = Ent_ReserveFreeEntity(&gs->entList);
    h.componentBits |= EC_FLAG_ENTITY;
	h.componentBits |= EC_FLAG_TRANSFORM;
    h.componentBits |= EC_FLAG_RENDERER;
    h.componentBits |= EC_FLAG_COLLIDER;

	size += App_WriteCommandBytesToFrameOutput((u8*)&h, sizeof(Cmd_EntityStateHeader));

    Ent ent = {};
    ent.entId = h.entId;
    ent.inUse = ENTITY_STATUS_IN_USE;
    ent.factoryType = ENTITY_TYPE_WORLD_CUBE;
    size += App_WriteCommandBytesToFrameOutput((u8*)&ent, sizeof(Ent));

    // create transform state
    Transform t = {};
    Transform_SetToIdentity(&t);
	t.pos.x = x;
    t.pos.y = y;
	t.pos.z = z;
	t.scale.x = halfSizeX * 2;
	t.scale.y = halfSizeY * 2;
	t.scale.z = halfSizeZ * 2;

    size += App_WriteCommandBytesToFrameOutput((u8*)&t, sizeof(Transform));
    
    // Wall texture "textures\\COMP03_1.bmp"
    // Metal texture "textures\\W33_5.bmp"

    // create renderer state
    EC_RendererState r = {};
    COM_CopyStringLimited("Cube", r.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited(("textures\\COMP03_1.bmp"), r.textureName, EC_RENDERER_STRING_LENGTH);

    size += App_WriteCommandBytesToFrameOutput((u8*)&r, sizeof(EC_RendererState));

    // Create Collider
    EC_ColliderState col = {};
    col.def.SetAsBox(
        t.pos.x, t.pos.y, t.pos.z,
        halfSizeX, halfSizeY, halfSizeZ, 
        ZCOLLIDER_FLAG_STATIC,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEFAULT
    );

    size += App_WriteCommandBytesToFrameOutput((u8*)&col, sizeof(EC_ColliderState));

    // Close command
    App_FinishCommandStream(headerPos, CMD_TYPE_ENTITY_STATE_2, 0, (u16)size);
    printf("  Add test solid: cmd stream type %d, size %d bytes\n", CMD_TYPE_ENTITY_STATE_2, size);
}

void Game_BuildTestScene(GameState* gs)
{
	Game_AddTestSolid(gs, 0, -6, 0, 12, 1, 12);
	Game_AddTestSolid(gs, 0, 6, 0, 12, 1, 12);
}

void Game_BuildTestHud()
{
    UIEntity* ent;
    i32 bytesOfUI = sizeof(UIEntity) * UI_MAX_ENTITIES;
    COM_ZeroMemory((u8*)g_ui_entities, bytesOfUI);

    // 0 Crosshair
    ent = UI_GetFreeEntity(g_ui_entities, UI_MAX_ENTITIES);
    ent->transform.pos.z = 0;
    ent->transform.scale = { 0.1f, 0.1f, 0.1f };
    RendObj_SetAsSprite(&ent->rendObj, SPRITE_MODE_UI, AppGetTextureIndexByName("textures\\charset.bmp"), 1, 1);
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
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1, 1
    );


    // 2 Player Status
    ent = UI_GetFreeEntity(g_ui_entities, UI_MAX_ENTITIES);
    ent->transform.pos.x = -1;
    ent->transform.pos.y = -(1 - (0.075f * 3));
    char* placeholderChars2 = "HP\nENERGY\nRAGE\n";
    RendObj_SetAsAsciCharArray(
        &ent->rendObj, 
        placeholderChars2,
        numChars, 
        0.075f, 
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1, 1
    );
}

void Game_BuildTestMenu()
{
    
}

void Game_Init()
{
    GS_Init(&g_gameState);
    Game_BuildTestHud();
    Game_BuildTestMenu();
}

void Exec_UpdateClient(GameState* gs, Cmd_ClientUpdate* cmd)
{
    Client* cl = App_FindOrCreateClient(cmd->clientId, &gs->clientList);
    cl->state = cmd->state;
    cl->entId = cmd->entId;

    // char buf[256];
    // sprintf_s(buf, 256, "APP: Client %d State: %d Avatar: iteration %d - id %d\n", cl->clientId, cl->state, cl->entIdArr[0], cl->entIdArr[1]);
    printf("GAME EXEC Client %d State: %d Avatar id %d/%d\n", cl->clientId, cl->state, cl->entId.iteration, cl->entId.index);

}

u8 Game_ReadCmd(GameState* gs, CmdHeader* header, u8* ptr)
{
    switch (header->GetType())
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
            Cmd_EntityState cmd = {};
            ptr += cmd.Read(header, ptr);
            if (gs->verbose)
            {
                printf("EXEC spawn dynamic ent %d/%d type %d\n",
                    cmd.entityId.iteration, cmd.entityId.index, cmd.factoryType
                );
            }
            Exec_DynamicEntityState(gs, &cmd);
			return 1;
        } break;

        case CMD_TYPE_STATIC_STATE:
        {
            printf("GAME: Defunct static state call\n");
            //Cmd_Spawn cmd = {};
            //ptr += cmd.Read(header, ptr);
            //Exec_StaticEntityState(gs, &cmd);
            return 1;
        } break;

        case CMD_TYPE_REMOVE_ENT:
        {
            Cmd_RemoveEntity cmd = {};
            ptr += cmd.Read(header, ptr);
            if (g_verbose)
            {
                printf("GAME Removing Ent %d/%d\n", cmd.entId.iteration, cmd.entId.index);
            }
            Ent* ent = Ent_GetEntityToRemoveById(&gs->entList, &cmd.entId);
			if (ent != NULL)
			{
                //Vec3 p = ent->transform.pos;
				if (Ent_HasTransform(ent))
				{
					EC_Transform* ecT = EC_FindTransform(gs, ent);
					Vec3 p = ecT->t.pos;
					Vec3 normal = COM_UnpackVec3Normal(cmd.gfxNormal);
					Game_SpawnLocalEntity(p.x, p.y, p.z, &normal, 10);
				}
				else
				{
					printf("GAME Ent %d/%d type %d has no transform...?\n",
						ent->entId.iteration, ent->entId.index,
						ent->factoryType
					);
				} 
				Ent_Free(gs, ent);
			}
			else
			{
				printf("GAME Ent %d/%d not found to remove!\n", cmd.entId.iteration, cmd.entId.index);
			}
            return 1;
        } break;
        #if 1
		case CMD_TYPE_PLAYER_INPUT:
		{
            Cmd_PlayerInput cmd;
            ptr += cmd.Read(header, ptr);
            Client* cl = App_FindClientById(cmd.clientId, &gs->clientList);
            Assert(cl != NULL);
            //Ent* ent = Ent_GetEntityById(&gs->entList, (EntId*)&cl->entIdArr);
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &cl->entId);
			if (motor == NULL)
			{
				printf("!GAME No motor for Entity %d/%d\n", cl->entId.iteration, cl->entId.index);
				return 1;
			}
            motor->state.input = cmd.input;
			return 1;
		} break;
        #endif
        #if 0
        case CMD_TYPE_PLAYER_STATE:
        {
            //
            Cmd_PlayerState cmd = {};

            Assert(header->size == sizeof(Cmd_PlayerState));
            COM_COPY_STRUCT(ptr, &cmd, Cmd_PlayerState);
            Exec_PlayerState(gs, &cmd);
            return 1;
        } break;
        #endif
        case CMD_TYPE_IMPULSE:
        {
            Cmd_ServerImpulse cmd = {};
            ptr += cmd.Read(header, ptr);
			return SV_ReadImpulse(gs, &cmd);
        } break;

        case CMD_TYPE_CLIENT_UPDATE:
        {
            Cmd_ClientUpdate cmd = {};
            ptr += cmd.Read(header, ptr);
            Exec_UpdateClient(gs, &cmd);
            return 1;
        } break;
    }
    return 0;
}

i32 Game_ReadCommandBuffer(GameState* gs, ByteBuffer* commands, u8 verbose)
{
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
            if (Game_ReadCmd(gs, &h, ptrRead) == 0)
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

/////////////////////////////////////////////////////////////////////////////
// Update Clients
/////////////////////////////////////////////////////////////////////////////
void Game_UpdatePlayers(ByteBuffer* output, Client* cl, InputActionSet* actions)
{

}

/////////////////////////////////////////////////////////////////////////////
// Game Tick
/////////////////////////////////////////////////////////////////////////////
void Game_Tick(
    GameState *gs,
    ByteBuffer* input,
    ByteBuffer* output,
    GameTime *time,
    InputActionSet* actions)
{
    //Game_ApplyInputToTransform(actions, &g_localClientTick.degrees, &gs->cameraTransform, time);
    gs->verbose = (u8)time->singleFrame;
    if (time->singleFrame)
    {
        printf("GAME Frame %d\n", time->gameFrameNumber);
    }
	#if 1
    if (Input_CheckActionToggledOn(actions, "Spawn Test", time->platformFrameNumber))
    {
        EntitySpawnOptions options = {};
        //options.scale = { 1, 1, 1 };
        options.pos = Game_RandomSpawnOffset(10, 0, 10);
        Game_WriteSpawnCmd(gs, ENTITY_TYPE_RIGIDBODY_CUBE, &options);
        #if 0
        // CmdHeader header = {};
        // header.type = CMD_TYPE_SPAWN;
        // header.size = sizeof(Cmd_Spawn);
        Cmd_EntityState cmd = {};
		cmd.entityId = Ent_ReserveFreeEntity(&gs->entList);
        cmd.factoryType = ENTITY_TYPE_RIGIDBODY_CUBE;
        cmd.pos = Game_RandomSpawnOffset(10, 0, 10);
        printf("Spawn test: %d/%d\n", cmd.entityId.iteration, cmd.entityId.index);
        //cmd.pos.y += 10;

        //App_WriteGameCmd((u8*)&cmd, CMD_TYPE_ENTITY_STATE, sizeof(Cmd_EntityState));
        APP_WRITE_CMD(0, CMD_TYPE_ENTITY_STATE, 0, cmd);

        //output->ptrWrite += COM_COPY_STRUCT(&header, output->ptrWrite, CmdHeader);
        //output->ptrWrite += COM_COPY_STRUCT(&cmd, output->ptrWrite, Cmd_Spawn);
        //output->count++;
        #endif
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
    Game_ReadCommandBuffer(gs, input, (time->singleFrame != 0));
    
    g_debugTransform = gs->cameraTransform;
    
    // Game state update
    // Update all inputs, entity components and colliders/physics
    Game_UpdateActorMotors(gs, time);
    Game_UpdateAIControllers(gs, time);
    Game_UpdateColliders(gs, time);
    Game_UpdateProjectiles(gs, time);

    Game_TickLocalEntities(time->deltaTime, (time->singleFrame == 1));

    // step forward
    Game_StepPhysics(gs, time);

    Client* localClient = App_FindLocalClient(&gs->clientList);
    if (localClient && localClient->state == CLIENT_STATE_PLAYING)
    {
        EntId id = localClient->entId;
        Ent* ent = Ent_GetEntityById(&gs->entList, &id);
        if (ent != NULL)
        {
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &id);
            EC_Transform* ecT = EC_FindTransform(gs, ent);
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
        }
        else if (gs->verbose)
        {
            printf("GAME: Could not find local avatar %d/%d\n", id.iteration, id.index);
        }
    }

    #if 0
    if (gs->local.localPlayerHasEnt)
    {
		EntId id = gs->local.localPlayerEntId;
        Ent* ent = Ent_GetEntityById(&gs->entList, &id);
		if (ent == NULL)
		{
			printf("GAME: Could not find local avatar %d/%d\n", id.iteration, id.index);

		}
		else
		{
			EC_ActorMotor* motor = EC_FindActorMotor(gs, &gs->local.localPlayerEntId);
            EC_Transform* ecT = EC_FindTransform(gs, ent);
			if (ecT == NULL)
			{
				printf("GAME: Could not find transform for local avatar %d/%d\n", id.iteration, id.index);
			}
			else
			{
				Transform_SetByPosAndDegrees(&gs->cameraTransform, &ecT->t.pos, &motor->state.input.degrees);
				// raise camera to eye height
				gs->cameraTransform.pos.y += (1.85f / 2) * 0.8f;
			}
		}
    }
    #endif
    
}
