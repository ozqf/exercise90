#pragma once

#include <stdlib.h>
#include "../../common/com_module.h"
#include "client.h"
#include "../packet.h"
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
internal f32 g_ellapsed = 0;
i32 CL_IsRunning() { return g_isRunning; }

#define CL_MAX_ALLOCATIONS 256
internal void* g_allocations[CL_MAX_ALLOCATIONS];
internal i32 g_bytesAllocated = 0;
internal i32 g_numAllocations = 0;

internal NetStream g_reliableStream;
internal NetStream g_unreliableStream;
internal UserIds g_ids;

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
    SimEntityDef def = {};
    #if 1
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
    #if 1
    def = {};
    def.isLocal = 1;
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

void CL_Init()
{
    Assert(g_clientState == CLIENT_STATE_NONE)
    printf("CL Init scene\n");
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

internal i32 CL_WriteUnreliableSection(ByteBuffer* packet)
{
    u8* start = packet->ptrWrite;
    // Send ping
	CmdPing ping = {};
	// TODO: Stream enqueue will set the sequence for us
	// so remove sending 0 here.
	Cmd_Prepare(&ping.header, g_ticks, 0);
	ping.sendTime = g_ellapsed;
    packet->ptrWrite += COM_COPY(&ping, packet->ptrWrite, ping.header.size);
    return (packet->ptrWrite - start);
}

internal void CL_WritePacket()
{
    #if 0
	printf("CL Write packet for user %d\n", g_ids.privateId);
	//Stream_EnqueueOutput(&user->reliableStream, &ping.header);
	
	// enqueue
	//ByteBuffer* buf = App_GetLocalClientPacketForWrite();
	
	u8 buf[1400];
    ByteBuffer packet = Buf_FromBytes(buf, 1400);
    Packet_StartWrite(&packet, 0, 0, 0);
    packet.ptrWrite += COM_WriteI32(COM_SENTINEL_B, packet.ptrWrite);
    i32 unreliableWritten = CL_WriteUnreliableSection(&packet);
    Packet_FinishWrite(&packet, 0, unreliableWritten);
    i32 total = packet.Written();
    App_CL_SendTo(g_ids.privateId, buf, total);
    
	//Packet_WriteFromStream(
    //    &user->reliableStream, &user->unreliableStream, buf, 1400, g_ellapsed, g_ticks, 0);
    #endif
}

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

internal void CL_ReadPacket(SysPacketEvent* ev)
{
    i32 headerSize = sizeof(SysPacketEvent);
    i32 dataSize = ev->header.size - headerSize;
    u8* data = (u8*)(ev) + headerSize;
    printf("CL %d Packet bytes from %d\n", dataSize, ev->sender.port);

    PacketDescriptor p;
    i32 err = Packet_InitDescriptor(
        &p, data, dataSize);
	if (err != COM_ERROR_NONE)
	{
		printf("  Error %d deserialising packet\n", err);
		return;
	}
    printf("  Tick %d Time %.3f\n",
        p.transmissionSimFrameNumber,
        p.transmissionSimTime);
}

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
                CL_ReadPacket(packet);
            } break;

            case SYS_EVENT_INPUT:
            {
                printf("CL Input - skip\n");
            } break;
        }
    }
}

void CL_Tick(ByteBuffer* sysEvents, f32 deltaTime)
{
    CL_ReadSystemEvents(sysEvents, deltaTime);
    Sim_Tick(&g_sim, deltaTime);
    CL_WritePacket();
    g_ticks++;
    g_ellapsed += deltaTime;
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
