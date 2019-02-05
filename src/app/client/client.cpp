#pragma once

#include <stdlib.h>
#include "../../common/com_module.h"
#include "client.h"
#include "../../interface/sys_events.h"
#include "../../sim/sim.h"

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

i32 CL_IsRunning() { return g_isRunning; }

#define CL_MAX_ALLOCATIONS 256
internal void* g_allocations[CL_MAX_ALLOCATIONS];
internal i32 g_bytesAllocated = 0;
internal i32 g_numAllocations = 0;

internal NetStream g_reliableStream;
internal NetStream g_unreliableStream;
internal UserIds g_ids;
internal AckStream g_acks;
internal ZNetAddress g_serverAddress;

#include "client_game.h"
#include "client_packets.h"

void CL_WriteDebugString(ZStringHeader* str)
{
	char* chars = str->chars;
	i32 written = 0;
    written += sprintf_s(chars, str->maxLength,
        "CLIENT:\nServer Tick: %d\nTick: %d\nElapsed: %.3f\nOutput Seq: %d\nAck Seq: %d\nDelay: %.3f\nJitter %.3f\n",
        g_serverTick, g_ticks, g_elapsed, g_acks.outputSequence,
		g_acks.remoteSequence, g_ping, g_jitter
    );
	#if 0
	// currently overflows debug text buffer:
	for (i32 i = 0; i < ACK_CAPACITY; ++i)
	{
		AckRecord* rec = &g_acks.awaitingAck[i];
		if (rec->acked)
		{
			f32 time = rec->receivedTime - rec->sentTime;
			written += sprintf_s(chars + written, str->maxLength,
				"%.3f Sent: %.3f Rec: %.3f\n", time, rec->sentTime, rec->receivedTime
            );
		}
	}
	#endif
	str->length = written;
}

internal void* CL_Malloc(i32 numBytes)
{
    Assert(g_numAllocations < CL_MAX_ALLOCATIONS)
    i32 index = g_numAllocations++;
    g_allocations[index] = malloc(numBytes);
    g_bytesAllocated += numBytes;
    return g_allocations[index];
}

void CL_LoadTestScene()
{
	Sim_LoadScene(&g_sim, 0);
	
    SimEntityDef def = {};
    #if 0
    for (i32 i = 0; i < 8; ++i)
    {
        f32 randX = (COM_STDRandf32() * 2) - 1;
        f32 randZ = (COM_STDRandf32() * 2) - 1;
        f32 x = 2 * randX;
        f32 y = 1;
        f32 z = 2 * randZ;
        def.isLocal = 1;
        def.pos[0] = x;
        def.pos[1] = y;
        def.pos[2] = z;
        def.velocity[0] = x;
        def.velocity[2] = z;
        Sim_AddEntity(&g_sim, &def);
    }
    #endif
    #if 0
    def = {};
    def.isLocal = 1;
	def.entType = SIM_ENT_TYPE_WORLD;
    def.pos[1] = 0;
    def.scale[0] = 12;
    def.scale[1] = 1;
    def.scale[2] = 12;
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

void CL_Init(ZNetAddress serverAddress)
{
    Assert(g_clientState == CLIENT_STATE_NONE)
    APP_PRINT(32, "CL Init scene\n");
    g_serverAddress = serverAddress;
	g_clientState = CLIENT_STATE_REQUESTING;
    i32 cmdBufferSize = MegaBytes(1);
    //ByteBuffer a = Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize);
    //ByteBuffer b = Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize);

    i32 maxEnts = 2048;
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
    APP_LOG(64, "CL init completed with %d allocations (%dKB)\n ",
        g_numAllocations, (u32)BytesAsKB(g_bytesAllocated));
}

void CL_Shutdown()
{
    // TODO: Free memory (:
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
internal void CL_ReadSystemEvents(ByteBuffer* sysEvents, f32 deltaTime)
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
                CL_ReadPacket(packet, &g_reliableStream, g_elapsed);
            } break;

            case SYS_EVENT_INPUT:
            {
                //printf("CL Input - skip\n");
            } break;
            case SYS_EVENT_SKIP: break;
        }
    }
}

internal void CL_RunReliableCommands(NetStream* stream, f32 deltaTime)
{
	ByteBuffer* b = &stream->inputBuffer;
	// -- Reliable --
	
	// TODO: Calculate the server tick the client is up to (due to smoothing out jitter etc)
	i32 serverTick = INT_MAX;
	for (;;)
	{
		i32 sequence = stream->inputSequence;
		Command* h = Stream_FindMessageBySequence(b->ptrStart, b->Written(), sequence);
		if (!h) { break; }
		// Execute only after jitter delay
		if (h->sequence > serverTick) { break; }
		
		// Step queue counter forward as we are now executing
		stream->inputSequence++;
		
		i32 err = Cmd_Validate(h);
		Assert(err == COM_ERROR_NONE)
		
		printf("CL exec input seq %d\n", h->sequence);
		
		switch (h->type)
		{
			case CMD_TYPE_S2C_SPAWN_ENTITY:
			{
				S2C_SpawnEntity* spawn = (S2C_SpawnEntity*)h;
				APP_LOG(64, "CL Spawn %d at %.3f, %.3f, %.3f\n",
					spawn->networkId, spawn->pos.x, spawn->pos.y, spawn->pos.z
				);
				
				SimEntityDef def = {};
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
				g_serverTick = sync->simTick;
                APP_PRINT(64, "CL Sync server sim tick %d\n", sync->simTick);
            } break;
			
			
			default:
			{
				APP_PRINT(64, "CL Unknown command type %d\n", h->type);
			} break;
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

internal void CL_CalcPings(f32 deltaTime)
{
	g_ping = Ack_CalculateAverageDelay(&g_acks);
	g_jitter = (g_acks.delayMax - g_acks.delayMin);
}

void CL_Tick(ByteBuffer* sysEvents, f32 deltaTime)
{
    APP_LOG(64, "*** CL TICK %d (T %.3f) ***\n", g_ticks, g_elapsed);
    CL_ReadSystemEvents(sysEvents, deltaTime);
    CL_CalcPings(deltaTime);
	CL_RunReliableCommands(&g_reliableStream, deltaTime);
    CLG_TickGame(&g_sim, deltaTime);
	g_ticks++;
	g_serverTick++;
    g_elapsed += deltaTime;
    CL_WritePacket(g_elapsed);
}

void CL_PopulateRenderScene(RenderScene* scene, i32 maxObjects, i32 texIndex, f32 interpolateTime)
{

    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }

        RendObj obj = {};
        MeshData* cube = COM_GetCubeMesh();
        RendObj_SetAsMesh(
            &obj, *cube, 0.3f, 0.3f, 1, texIndex);

        Transform t = ent->t;
        RendObj_InterpolatePosition(
                &t.pos,
                &ent->previousPos,
                &ent->t.pos,
                interpolateTime);
        RScene_AddRenderItem(scene, &t, &obj);
    }
}
