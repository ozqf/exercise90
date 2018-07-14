#pragma once

#include "game.h"

#include "game_intersection_test.cpp"

inline Vec3 Game_RandomSpawnOffset(f32 rangeX, f32 rangeY, f32 rangeZ)
{
    Vec3 v;
    v.x = COM_Randf32() * (rangeX - -rangeX) + -rangeX;
    v.y = COM_Randf32() * (rangeY - -rangeY) + -rangeY;
    v.z = COM_Randf32() * (rangeZ - -rangeZ) + -rangeZ;
    return v;
}

void Game_InitGameState(GameState *gs)
{
    printf("GAME Init State\n");
    printf("GAME sizeof(Ent): %d\n", sizeof(Ent));
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
    
    Game_InitEntityFactory();
}

void Game_BuildTestHud(GameState *state)
{
	#if 1
    EntId entId;
    Ent* ent;
    EC_Renderer* renderer;
    EntList* ents = &state->entList;

    
    // Init gs and component lists
    *state = {};

    state->entList.items = g_uiEntities;
    state->entList.count = UI_MAX_ENTITIES;
    state->entList.max = UI_MAX_ENTITIES;

    state->rendererList.items = g_ui_renderers;
    state->rendererList.count = UI_MAX_ENTITIES;
    state->rendererList.max = UI_MAX_ENTITIES;

	Ent_ResetEntityIds(ents);
	#endif

/////////////////////////////////////////////////////////////
// A test HUD
/////////////////////////////////////////////////////////////
	#if 1
    // Crosshair (sprite rendering a single char from a character sheet)
    entId = Ent_ReserveFreeEntity(ents);
    ent = Ent_GetAndAssign(ents, &entId);
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 0.1f;
    ent->transform.scale.y = 0.1f;
    ent->transform.scale.z = 0.1f;

    renderer = EC_AddRenderer(state, ent);
    // RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);

    RendObj_SetAsSprite(&renderer->rendObj, SPRITE_MODE_UI, AppGetTextureIndexByName("textures\\charset.bmp"), 1, 1);
    //RendObj_SetSpriteUVs(&obj->obj.sprite, 0.0625, 0.125, 0.5625, 0.5625 + 0.0625);
    RendObj_CalculateSpriteAsciUVs(&renderer->rendObj.data.sprite, '+');
	#endif

	#if 0
    // Crosshair - position test (sprite rendering a single char from a character sheet)
    entId = Ent_ReserveFreeEntity(ents);
    ent = Ent_GetAndAssign(ents, &entId);
    ent->transform.pos.x = 2;
    ent->transform.scale.x = 0.5f;
    ent->transform.scale.y = 0.5f;
    ent->transform.scale.z = 0.5f;

    renderer = EC_AddRenderer(state, ent);
    // RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);

    RendObj_SetAsSprite(&renderer->rendObj, SPRITE_MODE_UI, AppGetTextureIndexByName("textures\\charset.bmp"), 1, 1);
    //RendObj_SetSpriteUVs(&obj->obj.sprite, 0.0625, 0.125, 0.5625, 0.5625 + 0.0625);
    RendObj_CalculateSpriteAsciUVs(&renderer->rendObj.data.sprite, 'X');
	#endif

    i32 numChars;
	#if 1
    // Game Messages
    entId = Ent_ReserveFreeEntity(ents);
    ent = Ent_GetAndAssign(ents, &entId);
    ent->transform.pos.x = 0;
    //ent->transform.pos.y -= -1 - (0.05f * 3);
    ent->transform.pos.y += 1;
    // ent->transform.scale.x = 1f;
    // ent->transform.scale.y = 1f;
    // ent->transform.scale.z = 1f;
    renderer = EC_AddRenderer(state, ent);

    //char* chars = g_debugStr.chars;
	//char* chars = "Message Line 1\nMessage Line 2\nMessage Line 3\nMessage Line 4\n";
    char* chars = "Info\n";
    numChars = COM_StrLen(chars);
	//i32 numChars = g_testString;

    RendObj_SetAsAsciCharArray(
        &renderer->rendObj,
        chars,
        numChars,
        0.05f,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1, 1
    );
    //printf("Draw asci array:\n%s\n", chars);
    //obj->transform.pos.x = -1;//-0.75f;
    //obj->transform.pos.y = 1;//0.75f;
	#endif

	#if 1
    // Player Status
    entId = Ent_ReserveFreeEntity(ents);
    ent = Ent_GetAndAssign(ents, &entId);
    ent->transform.pos.x = -1;
    //ent->transform.pos.y -= -1 - (0.05f * 3);
    ent->transform.pos.y -= (1 - (0.075f * 3));
    // ent->transform.scale.x = 1f;
    // ent->transform.scale.y = 1f;
    // ent->transform.scale.z = 1f;
    renderer = EC_AddRenderer(state, ent);

    //char* chars = g_debugStr.chars;
	char* placeholderChars2 = "HP\nENERGY\nRAGE\n";
    numChars = COM_StrLen(placeholderChars2);
	//i32 numChars = g_testString;

    RendObj_SetAsAsciCharArray(
        &renderer->rendObj, 
        placeholderChars2, 
        numChars, 
        0.075f, 
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1, 1
    );
    //obj->transform.pos.x = -1;//-0.75f;
    //obj->transform.pos.y = 1;//0.75f;
	#endif

	#if 0
    /////////////////////////////////////////////////////////////
    // Walls mesh
    /////////////////////////////////////////////////////////////
    ent = Ent_GetFreeEntity(&state->entList);

    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0;
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 12;
    ent->transform.scale.y = 4;
    ent->transform.scale.z = 12;

    renderer = EC_AddRenderer(ent, state);
    
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshInverseCube, 1, 1, 1, 5);
	#endif
}

void Game_BuildTestMenu()
{
    
}

void Game_Init()
{
    Game_InitGameState(&g_gameState);
    Game_BuildTestHud(&g_uiState);
    Game_BuildTestMenu();
}

void Game_Reset()
{
    // Need to be different yet?
    Game_Init();
}

void Game_Shutdown(GameState* gs)
{
	printf("GAME Shutdown\n");
    // Reset all entities and game state here
	for (u32 i = 0; i < gs->entList.max; ++i)
	{
		gs->entList.items[i].inUse = 0;
	}
    gs->localPlayerHasEnt = 0;
	// Components
	/*
	EC_AIControllerList aiControllerList;
	EC_RendererList rendererList;
	EC_ColliderList colliderList;
	EC_ActorMotorList actorMotorList;
	EC_ProjectileList projectileList;
	EC_LabelList labelList;
	*/
    Game_Reset();
    COM_ZeroMemory((u8*)gs->rendererList.items, sizeof(EC_Renderer) * gs->rendererList.max);
	COM_ZeroMemory((u8*)gs->colliderList.items, sizeof(EC_Collider) * gs->colliderList.max);
	COM_ZeroMemory((u8*)gs->actorMotorList.items, sizeof(EC_ActorMotor) * gs->actorMotorList.max);
	COM_ZeroMemory((u8*)gs->projectileList.items, sizeof(EC_Projectile) * gs->projectileList.max);
	COM_ZeroMemory((u8*)gs->labelList.items, sizeof(EC_Label) * gs->labelList.max);
}

inline void Game_HandleEntityUpdate(GameState *gs, PhysEV_TransformUpdate *ev)
{
    EntId entId = {};
    entId.index = ev->ownerId;
    entId.iteration = ev->ownerIteration;
    Ent *ent = Ent_GetEntity(&gs->entList, &entId);
    if (ent == NULL)
    {
        return;
    }
    M4x4* updateM = (M4x4*)&ev->matrix;
	f32 magX = Vec3_Magnitudef(updateM->x0, updateM->x1, updateM->x2);
	f32 magY = Vec3_Magnitudef(updateM->y0, updateM->y1, updateM->y2);
	f32 magZ = Vec3_Magnitudef(updateM->z0, updateM->z1, updateM->z2);
    // Debugging trap
	// if (ZABS(magX) > 1.1f)
	// {
	// 	int foo = 1;
	// }
	// if (ZABS(magY) > 1.1f)
	// {
	// 	int foo = 1;
	// }
	// if (ZABS(magZ) > 1.1f)
	// {
	// 	int foo = 1;
	// }

    EC_Collider* col = EC_FindCollider(gs, &entId);
    Assert(col != NULL);
    col->velocity.x = ev->vel[0]; 
    col->velocity.y = ev->vel[1];
    col->velocity.z = ev->vel[2];
    col->isGrounded = (ev->flags & PHYS_EV_FLAG_GROUNDED);
#if 0
	//ent->transform.scale = { 1, 1, 1 };
	ent->transform.pos.x = updateM->posX;
	ent->transform.pos.y = updateM->posY;
	ent->transform.pos.z = updateM->posZ;
	Transform_ClearRotation(&ent->transform);
	Transform_RotateX(&ent->transform, ev->rot[0]);
	Transform_RotateY(&ent->transform, ev->rot[1]);
	Transform_RotateZ(&ent->transform, ev->rot[2]);
	/*Transform_RotateX(&ent->transform, updateM->xAxisW);
	Transform_RotateY(&ent->transform, updateM->yAxisW);
	Transform_RotateZ(&ent->transform, updateM->zAxisW);*/
#endif
#if 1
    Transform_FromM4x4(&ent->transform, updateM);
#endif
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
            Cmd_Spawn cmd = {};
            ptr += cmd.Read(header, ptr);
            Exec_StaticEntityState(gs, &cmd);
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
                Vec3 p = ent->transform.pos;
                Vec3 normal = COM_UnpackVec3Normal(cmd.gfxNormal);
                Game_SpawnLocalEntity(p.x, p.y, p.z, &normal, 10);
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
            motor->input = cmd.input;
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
// STEP PHYSICS
/////////////////////////////////////////////////////////////////////////////
#define MAX_ALLOWED_PHYSICS_STEP 0.0334f
void Game_StepPhysics(GameState* gs, GameTime* time)
{
    
    // Force physics step to always be no lower than 30fps

    /////////////////////////////////////////////////////////////////////////////
    // STEP PHYSICS
    f32 dt = time->deltaTime;
    if (dt > MAX_ALLOWED_PHYSICS_STEP)
    {
        dt = MAX_ALLOWED_PHYSICS_STEP;
    }

    MemoryBlock eventBuffer = Phys_Step(dt);
    
    //i32 ptrOffset = 0;
    u8 reading = 1;
	i32 eventsProcessed = 0;

    u8* readPos = (u8*)eventBuffer.ptrMemory;
    u8* end = (u8*)((u8*)eventBuffer.ptrMemory + eventBuffer.size);
    while (readPos < end)
    {
        //u8 *mem = (u8 *)((u8 *)eventBuffer.ptrMemory + ptrOffset);
        //i32 type = *(i32 *)mem;
        PhysDataItemHeader h;
        readPos += COM_COPY_STRUCT(readPos, &h, PhysDataItemHeader);
        switch (h.type)
        {
            case TransformUpdate:
            {
                PhysEV_TransformUpdate tUpdate = {};
                readPos += COM_COPY_STRUCT(readPos, &tUpdate, PhysEV_TransformUpdate);
                //COM_CopyMemory(mem, (u8 *)&tUpdate, sizeof(PhysEV_TransformUpdate));
                //ptrOffset += sizeof(PhysEV_TransformUpdate);
                Game_HandleEntityUpdate(gs, &tUpdate);
		    	eventsProcessed++;
            } break;

            case RaycastDebug:
            {
                PhysEv_RaycastDebug ray = {};
                readPos += COM_COPY_STRUCT(readPos, (u8*)&ray, PhysEv_RaycastDebug);
                RendObj_SetAsLine(
                    &g_debugLine,
                    ray.a[0], ray.a[1], ray.a[2],
                    ray.b[0], ray.b[1], ray.b[2],
                    ray.colour[0], ray.colour[1], ray.colour[2],
                    ray.colour[0], ray.colour[1], ray.colour[2]
                
                );
                // printf("Draw Raycast %.2f %.2f %.2f to %.2f %.2f %.2f\n",
                //     ray.a[0], ray.a[1], ray.a[2],
                //     ray.b[0], ray.b[1], ray.b[2]
                // );
                eventsProcessed++;
            } break;

            default:
            {
                readPos = end;
            } break;
        }
    }
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

    if (gs->localPlayerHasEnt)
    {
        Ent* ent = Ent_GetEntityById(&gs->entList, &gs->localPlayerEntId);
        Assert(ent != NULL);
        EC_ActorMotor* motor = EC_FindActorMotor(gs, &gs->localPlayerEntId);
        Transform_SetByPosAndDegrees(&gs->cameraTransform, &ent->transform.pos, &motor->input.degrees);
		// raise camera to eye height
        gs->cameraTransform.pos.y += (1.85f / 2) * 0.8f;
    }
    
}
