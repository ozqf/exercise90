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
    printf("CL Set local user public %d private %d\n",
        ids.publicId, ids.privateId
    );
    g_ids = ids;
    g_clientState = CLIENT_STATE_SYNC;
}

void CL_Init(ZNetAddress serverAddress)
{
    Assert(g_clientState == CLIENT_STATE_NONE)
    printf("CL Init scene\n");
    g_serverAddress = serverAddress;
	g_clientState = CLIENT_STATE_REQUESTING;
    i32 cmdBufferSize = MegaBytes(1);
    ByteBuffer a = Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize);
    ByteBuffer b = Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize);

    i32 maxEnts = 2048;
    i32 numEntityBytes = Sim_CalcEntityArrayBytes(maxEnts);
    SimEntity* mem = (SimEntity*)CL_Malloc(numEntityBytes);
    Sim_InitScene(&g_sim, a, b, mem, maxEnts);
    CL_LoadTestScene();

    COM_InitStream(&g_reliableStream,
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize),
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize)
    );
    COM_InitStream(&g_unreliableStream,
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize),
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize)
    );
    printf("CL init completed with %d allocations (%dKB)\n ",
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
    printf("CL Reading platform events (%d bytes)\n", sysEvents->Written());
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
                CL_ReadPacket(packet, &g_reliableStream);
            } break;

            case SYS_EVENT_INPUT:
            {
                printf("CL Input - skip\n");
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
				printf("CL Spawn %d at %.3f, %.3f, %.3f\n",
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
			
			default:
			{
				printf("CL Unknown command type %d\n", h->type);
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

void CL_Tick(ByteBuffer* sysEvents, f32 deltaTime)
{
    CL_ReadSystemEvents(sysEvents, deltaTime);
	CL_RunReliableCommands(&g_reliableStream, deltaTime);
    //Sim_Tick(&g_sim, deltaTime);
    CLG_TickGame(&g_sim, deltaTime);
    CL_WritePacket();
	
    g_ticks++;
    g_elapsed += deltaTime;
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
