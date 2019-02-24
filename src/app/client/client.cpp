#pragma once

#include <stdlib.h>
#include "../../common/com_module.h"
#include "client.h"
#include "../../interface/sys_events.h"
#include "../../interface/renderer_interface.h"
#include "../../sim/sim.h"
#include "client_input.h"

#define CLIENT_STATE_NONE 0
#define CLIENT_STATE_REQUESTING 1
#define CLIENT_STATE_HANDSHAKE 2
#define CLIENT_STATE_SYNC 3
#define CLIENT_STATE_PLAY 4

internal i32 g_clientState = CLIENT_STATE_NONE;

internal i32 g_isRunning = 0;
internal SimScene g_sim;
internal i32 g_ticks = 0;
internal f32 g_elapsed = 0;
internal i32 g_serverTick = 0;
internal f32 g_ping;
internal f32 g_jitter;

internal i32 g_avatarSerial = -2;

#define CL_MAX_ALLOCATIONS 256
internal void* g_allocations[CL_MAX_ALLOCATIONS];
internal i32 g_bytesAllocated = 0;
internal i32 g_numAllocations = 0;

internal NetStream g_reliableStream;
internal NetStream g_unreliableStream;
internal UserIds g_ids;
internal AckStream g_acks;
internal ZNetAddress g_serverAddress;

internal Vec3 g_testHitPos = { 0, 2, 0 };
internal M4x4 g_matrix;

// Menus
internal i32 g_mainMenuOn;

#define CL_MAX_INPUTS 256
internal InputAction g_inputActionItems[CL_MAX_INPUTS];
internal InputActionSet g_inputActions = {
    g_inputActionItems,
    0
};

#define CL_MAX_RENDER_COMMANDS 1024
internal RenderCommand* g_renderCommands;

internal SimActorInput g_actorInput = {};

#include "client_game.h"
#include "client_packets.h"

i32 CL_IsRunning() { return g_isRunning; }

internal void CL_WriteNetworkDebug(ZStringHeader* str)
{
	char* chars = str->chars;
	i32 written = 0;
    written += sprintf_s(chars, str->maxLength,
        "CLIENT:\nServer Tick: %d\nTick: %d\nElapsed: %.3f\nOutput Seq: %d\nAck Seq: %d\nDelay: %.3f\nJitter %.3f\n",
        g_serverTick, g_ticks, g_elapsed, g_acks.outputSequence,
		g_acks.remoteSequence, g_ping, g_jitter
    );


    written += sprintf_s(chars + written, str->maxLength,
			"%d Pending reliablebytes %d\n%d Pending unreliable bytes %d\n",
            Stream_CountCommands(&g_reliableStream.inputBuffer),
            g_reliableStream.inputBuffer.Written(),
            Stream_CountCommands(&g_unreliableStream.inputBuffer),
            g_unreliableStream.inputBuffer.Written()
            );

    #if 0
    SimEntity* ent =  Sim_GetEntityBySerial(&g_sim, -1);
    if (ent)
    {
        written += sprintf_s(chars + written, str->maxLength,
			"World vol pos Y: %.3f\n", ent->t.pos.y);
    }
    #endif
	#if 0
	// currently overflows debug text buffer:
	for (i32 i = 0; i < ACK_CAPACITY; ++i)
	{
		AckRecord* rec = &g_acks.awaitingAck[i];
		if (rec->acked)
		{
			f32 time = rec->receivedTime - rec->sentTime;
			written += sprintf_s(chars + written, str->maxLength,
				"%.3f Sent: %.3f Rec: %.3f\n",
				time, rec->sentTime, rec->receivedTime
            );
		}
	}
	#endif
	str->length = written;
}

internal void CL_WriteTransformDebug(ZStringHeader* str)
{
	char* chars = str->chars;
	i32 written = 0;
    f32* m = g_matrix.cells;
    written += sprintf_s(chars, str->maxLength,
        "MATRIX:\n%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n",
        m[0], m[4], m[8], m[12],
        m[1], m[5], m[9], m[13],
        m[2], m[6], m[10], m[14],
        m[3], m[7], m[11], m[15]
    );
}

internal void CL_WriteCameraDebug(ZStringHeader* str)
{
	
}

void CL_WriteDebugString(ZStringHeader* str)
{
	CL_WriteNetworkDebug(str);
	//CL_WriteTransformDebug(str);
	//CL_WriteCameraDebug(str);
}

internal void* CL_Malloc(i32 numBytes)
{
    Assert(g_numAllocations < CL_MAX_ALLOCATIONS)
    i32 index = g_numAllocations++;
    g_allocations[index] = malloc(numBytes);
    g_bytesAllocated += numBytes;
    return g_allocations[index];
}

u8 CL_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
    return 0;
}

void CL_LoadTestScene()
{
	Sim_LoadScene(&g_sim, 0);
	
    SimEntityDef def = {};
    #if 1
	
    def = {};
    def.isLocal = 1;
	g_avatarSerial = Sim_ReserveEntitySerial(&g_sim, def.isLocal);
    def.serial = g_avatarSerial;
	def.entType = SIM_ENT_TYPE_ACTOR;
    def.pos[1] = 0;
    def.scale[0] = 1;
    def.scale[1] = 1;
    def.scale[2] = 1;
    Sim_AddEntity(&g_sim, &def);

    g_sim.boundaryMin = { -6, -6, -6 };
    g_sim.boundaryMax = { 6, 6, 6 };

    #endif
}

// Public so that local user can be instantly set from outside
void CL_SetLocalUser(UserIds ids)
{
    Assert(g_clientState == CLIENT_STATE_REQUESTING)
    APP_LOG(64, "CL Set local user public %d private %d\n",
        ids.publicId, ids.privateId
    );
    g_ids = ids;
    g_clientState = CLIENT_STATE_SYNC;
}

////////////////////////////////////////////////////////////////////
// Init
////////////////////////////////////////////////////////////////////
void CL_Init(ZNetAddress serverAddress)
{
    Assert(g_clientState == CLIENT_STATE_NONE)
    APP_PRINT(32, "CL Init scene\n");
    g_serverAddress = serverAddress;
	g_clientState = CLIENT_STATE_REQUESTING;
    i32 cmdBufferSize = MegaBytes(1);
    //ByteBuffer a = Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize);
    //ByteBuffer b = Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize);

    i32 maxEnts = APP_MAX_ENTITIES;
    i32 numEntityBytes = Sim_CalcEntityArrayBytes(maxEnts);
    SimEntity* mem = (SimEntity*)CL_Malloc(numEntityBytes);
    Sim_InitScene(&g_sim, mem, maxEnts);
	Sim_Reset(&g_sim);
    CL_LoadTestScene();

    COM_InitStream(&g_reliableStream,
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize),
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize)
    );
    COM_InitStream(&g_unreliableStream,
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize),
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize)
    );

    i32 numRenderCommandBytes = sizeof(RenderCommand) *
		CL_MAX_RENDER_COMMANDS;
    u8* bytes = (u8*)CL_Malloc(numRenderCommandBytes);
    COM_SET_ZERO(bytes, numRenderCommandBytes);

    g_renderCommands = (RenderCommand*)bytes;

    CL_InitInputs(&g_inputActions);

    APP_LOG(64, "CL init completed with %d allocations (%dKB)\n ",
        g_numAllocations, (u32)BytesAsKB(g_bytesAllocated));
}

void CL_Shutdown()
{
	for (i32 i = 0; i < g_numAllocations; ++i)
	{
		free(g_allocations[i]);
	}
	g_numAllocations = 0;
}
#if 0
internal void CL_ReadReliableCommands(NetStream* stream)
{
    ByteBuffer* b = &stream->inputBuffer;
    u8* read = b->ptrStart;
    u8* end = b->ptrEnd;
    while (read < end)
    {
        Command* header = (Command*)read;
        Assert(header->sentinel == CMD_SENTINEL)
        Assert(header->size > 0)
        read += header->size;
        switch (header->type)
        {
            case CMD_TYPE_IMPULSE:
            {
                CmdImpulse* cmd = (CmdImpulse*)header;
            } break;
            default:
            {
                printf("CL Unknown command type %d\n", header->type);
                ILLEGAL_CODE_PATH
            } break;
        }
    }
}
#endif
internal void CL_ReadSystemEvents(
	ByteBuffer* sysEvents, f32 deltaTime, u32 platformFrame)
{
    //printf("CL Reading platform events (%d bytes)\n", sysEvents->Written());
    u8* read = sysEvents->ptrStart;
    u8* end = sysEvents->ptrWrite;
    while (read < end)
    {
        SysEvent* ev = (SysEvent*)read;
        i32 err = Sys_ValidateEvent(ev);
        if (err != COM_ERROR_NONE)
        {
            printf("CL Error %d reading system event header\n", err);
            return;
        }
        read += ev->size;
        switch(ev->type)
        {
            case SYS_EVENT_PACKET:
            {
				//COM_PrintBytes((u8*)ev, ev->size, 16);
                SysPacketEvent* packet = (SysPacketEvent*)ev;
                CL_ReadPacket(packet, &g_reliableStream, &g_unreliableStream, g_elapsed);
            } break;

            case SYS_EVENT_INPUT:
            {
                SysInputEvent* inputEv = (SysInputEvent*)ev;
                Input_TestForAction(
					&g_inputActions,
					inputEv->value,
					inputEv->inputID,
					platformFrame);
				
				if (Input_CheckActionToggledOn(
					&g_inputActions,
					"Menu",
					platformFrame))
				{
					g_mainMenuOn = !g_mainMenuOn;
				}

            } break;
            case SYS_EVENT_SKIP: break;
        }
    }
}

internal i32 CL_IsCommandTickSensitive(i32 cmdType)
{
	switch (cmdType)
	{
		case CMD_TYPE_S2C_SYNC: { return false; }
	}
	return true;
}

internal void CL_SetServerTick(i32 value)
{
	g_serverTick = value;
}

// Can be changed during command execute so always retreive from here:
internal i32 CL_GetServerTick()
{
	return g_serverTick;
}

internal i32 CL_ExecReliableCommand(Command* h, f32 deltaTime, i32 tickDiff)
{
    //APP_LOG(64, "CL exec input seq %d\n", h->sequence);

	switch (h->type)
	{
        case CMD_TYPE_S2C_SPAWN_PROJECTILE:
        {
            S2C_SpawnProjectile* prj = (S2C_SpawnProjectile*)h;
            APP_LOG(256, "CL Spawn Prj %d on SV tick %d (local sv tick diff %d. Cmd tick %d)\n",
                prj->def.projType,
				prj->def.tick,
				prj->def.tick - CL_GetServerTick(),
				prj->header.tick
            );
            // flip diff to specify fast forwarding
            Sim_ExecuteProjectileSpawn(&g_sim, &prj->def, -tickDiff);
        } break;
		case CMD_TYPE_S2C_SPAWN_ENTITY:
		{
			S2C_SpawnEntity* spawn = (S2C_SpawnEntity*)h;
			APP_LOG(64, "CL Spawn %d at %.3f, %.3f, %.3f\n",
				spawn->networkId, spawn->pos.x, spawn->pos.y, spawn->pos.z
			);
			
			SimEntityDef def = {};
			def.serial = spawn->networkId;
            def.birthTick = h->tick;
            def.entType = spawn->entType;
			def.pos[0] = spawn->pos.x;
			def.pos[1] = spawn->pos.y;
			def.pos[2] = spawn->pos.z;
			def.velocity[0] = spawn->vel.x;
			def.velocity[1] = spawn->vel.y;
			def.velocity[2] = spawn->vel.z;
			Sim_AddEntity(&g_sim, &def);
		} break;
        case CMD_TYPE_S2C_SYNC:
        {
            S2C_Sync* sync = (S2C_Sync*)h;
			CL_SetServerTick(sync->simTick - APP_DEFAULT_JITTER_TICKS);
			
            // Lets not do what the server tells us!
			//g_serverTick = sync->simTick - sync->jitterTickCount;
			APP_LOG(64, "/////////////////////////////////////////\n");
            APP_LOG(64, "CL Sync server sim tick %d\n", CL_GetServerTick());
			APP_LOG(64, "/////////////////////////////////////////\n");
        } break;
		
		
		default:
		{
			APP_PRINT(64, "CL Unknown command type %d\n", h->type);
		} break;
	}
    return 1;
}

internal void CL_RunReliableCommands(NetStream* stream, f32 deltaTime)
{
	ByteBuffer* b = &stream->inputBuffer;
	
	for (;;)
	{
		i32 sequence = stream->inputSequence;
		Command* h = Stream_FindMessageBySequence(
			b->ptrStart, b->Written(), sequence);
        // No commands to run
		if (!h) { break; }

		i32 diff = h->tick - CL_GetServerTick();
		//APP_LOG(128, "CL Exec Cmd %d: Cmd Tick %d, Sync Tick %d (diff %d), CL Tick %d\n",
		//	h->sequence, h->tick, CL_GetServerTick(), diff, g_ticks
		//);
		
		if (CL_IsCommandTickSensitive(h->type))
		{
			if (diff > 0)
			{
				APP_LOG(128, "\tCL Delaying execution of cmd %d until tick %d (diff %d)\n",
					h->sequence, h->tick, diff);
				// Drop out - next reliable command cannot be executed until we reach this frame
				break;
			}
			
			if (diff < 0)
			{
				APP_LOG(128,  "\tCL Fast forward cmd %d by %d frames\n",
					h->sequence, -(diff)
				);
			}
		}
		
		// Step queue counter forward as we are now executing
		stream->inputSequence++;
		
		i32 err = Cmd_Validate(h);
		Assert(err == COM_ERROR_NONE)
		if (CL_ExecReliableCommand(h, deltaTime, diff))
        {
            Stream_DeleteCommand(b, h);
        }
	}
	/*
	// -- Unreliable commands --
	u8* read = buf->ptrStart;
	u8* end = buf->ptrWrite;
	while (read < end)
	{
		Command* h = (Command*)read;
		i32 err = Cmd_Validate(h);
		if (err != COM_ERROR_NONE)
		{
			printf("CL Error %d running input commands\n", err);
			return;
		}
		read += h->size;
		
		switch(h->type)
		{
			case CMD_TYPE_S2C_SPAWN_ENTITY:
			{
				S2C_SpawnEntity* spawn = (S2C_SpawnEntity*)h;
				printf("CL Spawn %d at %.3f, %.3f, %.3f\n",
					spawn->networkId, spawn->pos.x, spawn->pos.y, spawn->pos.z
				);
			} break;
			
			default:
			{
				printf("CL Unknown command type %d\n", h->type);
			} break;
		}
	}
	*/
}

internal void CL_RunUnreliableCommands(NetStream* stream, f32 deltaTime)
{
	ByteBuffer* b = &stream->inputBuffer;
    u8* read = b->ptrStart;
	//APP_LOG(128, "CL Run %d bytes of unreliable msgs\n",
    //    b->Written());
    //CL_LogCommandBuffer(b, "unreliable");

	while (read < b->ptrWrite)
	{
		i32 sequence = stream->inputSequence;
		Command* h = (Command*)read;
        i32 err = Cmd_Validate(h);
		if (err != COM_ERROR_NONE)
		{
			APP_PRINT(128, "CL Run unreliable - unvalid cmd code %d\n", err);
			Buf_Clear(b);
			return;
		}
        
		i32 diff = h->tick - CL_GetServerTick();
		//APP_LOG(128, "CL Exec Cmd %d: Cmd Tick %d, Sync Tick %d (diff %d), CL Tick %d\n",
		//	h->sequence, h->tick, CL_GetServerTick(), diff, g_ticks
		//);
        i32 executed = 0;
        if (diff <= 0)
        {
            switch (h->type)
            {
                case CMD_TYPE_S2C_SYNC_ENTITY:
                {
                    S2C_EntitySync* cmd = (S2C_EntitySync*)h;
                    SimEntity* ent = Sim_GetEntityBySerial(&g_sim, cmd->networkId);
                    if (!ent)
                    {
                        APP_PRINT(128, "CL No ent %d for sync\n", cmd->networkId);
                    }
                    else
                    {
                        //APP_LOG(64, "CL Sync ent %d\n", cmd->networkId);
                        ent->t.pos = cmd->pos;
                        executed = 1;
                    }
                } break;

                case CMD_TYPE_PING:
                {
                    CmdPing* cmd = (CmdPing*)h;
                    executed = 1;
                } break;

                default:
                {
                    APP_PRINT(64, "CL Unknown unreliable type %d\n", h->type);
                } break;
            }
        }
        
        if (executed)
        {
            Stream_DeleteCommand(b, h);
        }
        else
        {
            read += h->size;
        }
    }
}

internal void CL_CalcPings(f32 deltaTime)
{
	g_ping = Ack_CalculateAverageDelay(&g_acks);
	g_jitter = (g_acks.delayMax - g_acks.delayMin);
}

void CL_Tick(ByteBuffer* sysEvents, f32 deltaTime, u32 platformFrame)
{
    APP_LOG(64, "*** CL TICK %d (Server Sync Tick %d. T %.3f) ***\n",
        g_ticks, g_serverTick, g_elapsed);
    CL_ReadSystemEvents(sysEvents, deltaTime, platformFrame);
    CL_CalcPings(deltaTime);
	CL_RunReliableCommands(&g_reliableStream, deltaTime);
    //CL_LogCommandBuffer(&g_unreliableStream.inputBuffer, "Unreliable input");
    CL_RunUnreliableCommands(&g_unreliableStream, deltaTime);
    CL_UpdateActorInput(&g_inputActions, &g_actorInput);
    //CLG_HandlePlayerInput(NULL, &g_actorInput);
	SimEntity* plyr = Sim_GetEntityBySerial(&g_sim, g_avatarSerial);
	if (plyr)
	{
		plyr->input = g_actorInput;
	}
	else
	{
		printf("No player!\n");
	}
	
    CLG_TickGame(&g_sim, deltaTime);
	
	C2S_Input cmd;
	Cmd_InitClientInput(&cmd, &g_actorInput, NULL, g_ticks);
    
	g_ticks++;
	g_serverTick++;
    g_elapsed += deltaTime;
    CL_WritePacket(g_elapsed, &cmd);
}

void CL_PopulateRenderScene(
	Transform* cam,
	RenderScene* scene,
	i32 maxObjects,
	i32 texIndex,
	f32 interpolateTime)
{
    Transform t;
    Transform_SetToIdentity(&t);
    RendObj obj = {};
    MeshData* cube = COM_GetCubeMesh();
	
	
    // Hit test
	i32 mouseScrX = Input_GetActionValue(
		&g_inputActions, "Mouse Pos X");
	i32 mouseScrY = Input_GetActionValue(
		&g_inputActions, "Mouse Pos Y");
	ScreenInfo scr = App_GetScreenInfo();
	
	// screen pos in 0 to 1 range
	f32 percentX = (f32)mouseScrX / (f32)scr.width;
	f32 percentY = (f32)mouseScrY / (f32)scr.height;
	
	// flip y axis
	percentY = (1 - percentY);
	
	// convert to -1 to +1 range
	percentX = (percentX * 2.0f) - 1.0f;
	percentY = (percentY * 2.0f) - 1.0f;
	
	//printf("Mouse scr Percent %.3f, %.3f\n",
	//	percentX, percentY);
	
    Vec3 forward = Transform_GetForward(cam);
    //forward.x *= 0.5f;
    //forward.y *= 0.5f;
    //forward.z *= 0.5f;
	
    Vec3 left = Transform_GetLeft(cam);
    left.x *= (percentX * scr.aspectRatio);
    left.y *= (percentX * scr.aspectRatio);
    left.z *= (percentX * scr.aspectRatio);

    Vec3 up = Transform_GetUp(cam);
    up.x *= percentY;
    up.y *= percentY;
    up.z *= percentY;

    t.pos = 
    {
        cam->pos.x + forward.x,
        cam->pos.y + forward.y,
        cam->pos.z + forward.z
    };
	t.scale = { 0.2f, 0.2f, 0.2f };
	t.pos.x += left.x;
	t.pos.y += left.y;
	t.pos.z += left.z;
	
	t.pos.x += up.x;
	t.pos.y += up.y;
	t.pos.z += up.z;
    
    Transform t2 = t;
    t2.pos.x += forward.x;
    t2.pos.y += forward.y;
    t2.pos.z += forward.z;

    Transform t3 = t2;
    t3.pos.x += forward.x;
    t3.pos.y += forward.y;
    t3.pos.z += forward.z;
    f32 m[16];
    M4x4_SetToIdentity(m);
    COM_SetupDefault3DProjection(m, scr.aspectRatio);
    t3.pos = Vec3_MultiplyByM4x4(&t3.pos, m);
    g_matrix = *(M4x4*)m;
	
    RendObj_SetAsMesh(
		&obj, *cube, 1, 0, 1, texIndex);
    //t.pos = g_testHitPos;

    RScene_AddRenderItem(scene, &t, &obj);
    RScene_AddRenderItem(scene, &t2, &obj);
    RScene_AddRenderItem(scene, &t3, &obj);

    //////////////////////////////////////////////////////////////////////
    // Sim Entities
    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        if (scene->numObjects >= scene->maxObjects) { break; }

        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        
		switch (ent->entType)
		{
			case SIM_ENT_TYPE_WORLD:
			{
				RendObj_SetAsMesh(
					&obj, *cube, 0.2f, 0.2f, 0.2f, texIndex);
			} break;
			
			case SIM_ENT_TYPE_ACTOR:
			{
				RendObj_SetAsMesh(
					&obj, *cube, 0.2f, 1, 0.2f, texIndex);
			} break;
			
			default:
			{
				RendObj_SetAsMesh(
					&obj, *cube, 0.3f, 0.3f, 1, texIndex);
			} break;
		}
        
        t = ent->t;
		RendObj_InterpolatePosition(
                &t.pos,
                &ent->previousPos,
                &ent->t.pos,
                interpolateTime);
        RScene_AddRenderItem(scene, &t, &obj);
    }
}

void CL_CopyCameraTransform(Transform* target)
{
	Transform_SetToIdentity(target);
    target->pos.z = 18;
    target->pos.y = 12;
    Transform_SetRotation(
        target,
        -(45 * DEG2RAD),
        0,
        0
    );
}

void CL_GetRenderCommands(
	RenderCommand** cmds,
	i32* numCommands,
	i32 texIndex,
	f32 interpolateTime)
{
    *cmds = g_renderCommands;
    *numCommands = 0;

    i32 nextCommand = 0;

    RenderCommand* cmd;
    ScreenInfo info = App_GetScreenInfo();

    // setup scene
    cmd = &g_renderCommands[nextCommand++];
    cmd->type = REND_CMD_TYPE_SETTINGS;
    RenderSceneSettings* s = &cmd->settings;
    s->fov = 90;
    s->projectionMode = RENDER_PROJECTION_MODE_3D;
    s->orthographicHalfHeight = 8;
    
	CL_CopyCameraTransform(&cmd->settings.cameraTransform);

    // setup projection - must be done after settings
    cmd = &g_renderCommands[nextCommand++];
    cmd->type = REND_CMD_TYPE_PROJECTION;
    COM_SetupDefault3DProjection(cmd->projection.cells, 1);//info.aspectRatio);
	
    /*Transform_SetToIdentity(&cmd->settings.cameraTransform);
    s->cameraTransform.pos.z = 18;
    s->cameraTransform.pos.y = 12;
    Transform_SetRotation(
        &cmd->settings.cameraTransform,
        -(45 * DEG2RAD),
        0,
        0
    );*/
	
	// Set viewport
	cmd = &g_renderCommands[nextCommand++];
	cmd->type = REND_CMD_TYPE_SET_VIEWPORT;
	cmd->viewPort.viewPortX = 0;//info.width / 2;
	cmd->viewPort.viewPortY = 0;
	cmd->viewPort.viewPortWidth = info.width;// / 2;
	cmd->viewPort.viewPortHeight = info.height;
	
    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        if (nextCommand == CL_MAX_RENDER_COMMANDS)
        {
            break;
        }
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        
        RendObj obj = {};
        //MeshData* cube = COM_GetCubeMesh();
        //RendObj_SetAsMesh(
        //    &obj, *cube, 0.3f, 0.3f, 1, texIndex);
		
		RendObj_SetAsAABB(
			&obj, 1, 1, 1, 0, 1, 0);

        Transform t = ent->t;
        RendObj_InterpolatePosition(
                &t.pos,
                &ent->previousPos,
                &ent->t.pos,
                interpolateTime);
        
        // Add to command list
        cmd = &g_renderCommands[nextCommand++];
        cmd->type = REND_CMD_TYPE_DRAW;
        cmd->drawItem.obj = obj;
        cmd->drawItem.transform = t;
    }

    *numCommands = nextCommand;
}
