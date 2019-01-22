#pragma once

#include <stdlib.h>
#include "../../common/com_module.h"
#include "server.h"
#include "../packet.h"
#include "../../interface/sys_events.h"
#include "../../sim/sim.h"

#define SV_MAX_MALLOCS 1024

internal MallocItem g_mallocItems[SV_MAX_MALLOCS];
internal MallocList g_mallocs;
internal UserList g_users;
internal SimScene g_sim;

internal i32 g_isRunning = 0;
internal i32 g_ticks = 0;
internal f32 g_ellapsed = 0;
i32 SV_IsRunning() { return g_isRunning; }

// this is the server's command buffer to execute next tick
internal ByteBuffer g_platformInput;

internal i32 SV_IsPrivateIdInUse(i32 id)
{
    for (i32 i = 0; i < g_users.max; ++i)
    {
        User* u = &g_users.items[i];

    }
    return 0;
}

internal i32 SV_CreateSalt()
{
    i32 result = 0;
    do
    {
        result = (i32)(COM_STDRandf32() * INT_MAX);
    } while (result == 0);
    return result;
}

internal UserIds SV_GenerateUserId()
{
    UserIds newId;
    do
    {
        newId.privateId = SV_CreateSalt();
    } while (SV_IsPrivateIdInUse(newId.privateId));
    newId.publicId = g_users.nextPublicId++;
    return newId;
}

            

ByteBuffer* SV_GetPlatformInput()
{
    return &g_platformInput;
}

internal void SV_AllocateUserStream(NetStream* stream, i32 capacityPerBuffer)
{
    if (stream->initialised) { return; }
    stream->initialised = 1;
    stream->inputBuffer = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, capacityPerBuffer, "User Input"),
        capacityPerBuffer
    );
    stream->outputBuffer = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, capacityPerBuffer, "User Input"),
        capacityPerBuffer
    );
}

// This is public so that a local client can be made easily
internal UserIds SV_CreateUser(UserIds ids, ZNetAddress* addr)
{
    User* user = User_GetFree(&g_users);
    user->ids = ids;
    user->address = *addr;
    SV_AllocateUserStream(&user->reliableStream, KiloBytes(64));
    SV_AllocateUserStream(&user->unreliableStream, KiloBytes(64));
    printf("SV creating new user public %d private %d\n",
        ids.publicId, ids.privateId
    );
    return ids;
}

UserIds SV_CreateLocalUser()
{
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;
    UserIds id = SV_GenerateUserId();
    User* u = SV_CreateUser(id, &addr);
    u->state = USER_STATE_SYNC;
    return id;
}

internal void SV_UserStartSync(User* user)
{
    
}

internal void SV_LoadTestScene()
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

internal void SV_ListAllocs()
{
    printf("-- SV ALLOCS --\n");
    i32 tally = 0;
    for (i32 i = 0; i < g_mallocs.max; ++i)
    {
        MallocItem* item = &g_mallocs.items[i];
        tally += item->capacity;
        if (item->ptr == NULL) { continue; }
        printf("%s: %d bytes\n", item->label, item->capacity);
    }
    printf("  Total: %lluKB, %lluMB\n",
        BytesAsKB(g_mallocs.totalBytes),
        BytesAsMB(g_mallocs.totalBytes)
        );
    printf("    Tally: %d bytes\n", tally);
}

void SV_Init()
{
    printf("SV Init scene\n");

    g_mallocs = COM_InitMallocList(g_mallocItems, SV_MAX_MALLOCS);

    g_users = {};
    g_users.max = APP_MAX_USERS;
    i32 userBytes = sizeof(User) * APP_MAX_USERS;
    User* users = (User*)COM_Malloc(&g_mallocs, userBytes, "SV Users");
    g_users.items = users;

    i32 size = KiloBytes(64);
    ByteBuffer a = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, size, "Sim Buf A"), size);
    ByteBuffer b = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, size, "Sim Buf B"), size);

    i32 maxEnts = 2048;
    size = Sim_CalcEntityArrayBytes(maxEnts);
    SimEntity* mem = (SimEntity*)COM_Malloc(&g_mallocs, size, "Sim Ents");
    Sim_InitScene(&g_sim, a, b, mem, maxEnts);
    SV_LoadTestScene();
    //SV_ListAllocs();
}

void SV_Shutdown()
{
    for (i32 i = 0; i < g_mallocs.max; ++i)
    {
        if (g_mallocs.items[i].ptr != NULL)
        {
            free(g_mallocs.items[i].ptr);
        }
    }
}

#if 0
internal void SV_EnqueueReliableOutput(User* user, Command* cmd)
{
    ByteBuffer* b = &user->reliableStream.outputBuffer;
    Assert(b->Space() >= cmd->size)
    b->ptrWrite += COM_CopyMemory((u8*)cmd, b->ptrWrite, cmd->size);
}

/*
i32                 Stream_BufferMessage(
                        ByteBuffer* b, u32 msgId, u8* bytes, i32 numBytes)
internal void       Stream_Clear(NetStream* stream)
i32                 Stream_WriteStreamMsgHeader(
                        u8* ptr, u32 msgId, i32 numBytes, f32 resendRateSeconds);
u16                 Stream_WritePacketHeader(u8 sequenceOffset, u16 size)
void                Stream_ReadPacketHeader(
                        u16 header, u8* sequenceOffset, u16* size)
TransmissionRecord* Stream_AssignTransmissionRecord(
                        TransmissionRecord* records,
                        u32 sequence)
TransmissionRecord* Stream_FindTransmissionRecord(
                        TransmissionRecord* records,
                        u32 sequence)
void                Stream_PrintPacketManifest(u8* bytes, u16 numBytes)
StreamMsgHeader*    Stream_FindMessageById(u8* read, u8* end, u32 id)
u8*                 Stream_CollapseBlock(
                        u8* blockStart,
                        u32 blockSpace,
                        u8* bufferEnd)
u32                 Stream_ClearReceivedOutput(
                        NetStream* stream, u32 packetSequence)
void                Stream_OutputToPacket(
                        i32 connId,
                        NetStream* s,
                        ByteBuffer* packetBuf,
                        f32 deltaTime)
u8*                 Stream_PacketToInput(NetStream* s, u8* ptr)

*/

internal i32 SV_WriteUnreliableSection(User* user, ByteBuffer* packet)
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

internal void SV_WriteUserPacket(User* user)
{
    #if 1
	printf("SV Write packet for user %d\n", user->ids.privateId);
	//Stream_EnqueueOutput(&user->reliableStream, &ping.header);
	
	// enqueue
	//ByteBuffer* buf = App_GetLocalClientPacketForWrite();
	
	u8 buf[1400];
    ByteBuffer packet = Buf_FromBytes(buf, 1400);
    Packet_StartWrite(&packet, 0, 0, 0);
    packet.ptrWrite += COM_WriteI32(COM_SENTINEL_B, packet.ptrWrite);
    i32 unreliableWritten = SV_WriteUnreliableSection(user, &packet);
    Packet_FinishWrite(&packet, 0, unreliableWritten);
    i32 total = packet.Written();
    App_SV_SendTo(user->ids.privateId, buf, total);
    
	//Packet_WriteFromStream(
    //    &user->reliableStream, &user->unreliableStream, buf, 1400, g_ellapsed, g_ticks, 0);
    #endif
}
#endif

internal void SV_WriteTestPacket()
{
    // Make a packet, no messages just a header
    u8 buf[1400];
    ByteBuffer b = Buf_FromBytes(buf, 1400);
    Packet_StartWrite(&b, g_ticks, g_ellapsed, 0);
    b.ptrWrite += COM_WriteI32(COM_SENTINEL_B, b.ptrWrite);
    Packet_FinishWrite(&b, 0, 0);
    i32 written = b.Written();

    // loopback address
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;

    App_SendTo(0, &addr, buf, written);
}

internal void SV_ReadPacket(SysPacketEvent* ev)
{
	i32 headerSize = sizeof(SysPacketEvent);
    i32 dataSize = ev->header.size - headerSize;
    u8* data = (u8*)(ev) + headerSize;
    printf("SV %d Packet bytes from %d\n", dataSize, ev->sender.port);

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

internal void SV_ReadSystemEvents(ByteBuffer* sysEvents, f32 deltaTime)
{
	u8* read = sysEvents->ptrStart;
	u8* end = sysEvents->ptrWrite;
	while (read < end)
	{
		SysEvent* ev = (SysEvent*)read;
		i32 err = Sys_ValidateEvent(ev);
		if (err != COM_ERROR_NONE)
		{
			printf("SV Error %d reading system event header\n", err);
			return;
		}
		read += ev->size;
		switch (ev->type)
		{
			case SYS_EVENT_PACKET:
            {
				COM_PrintBytes((u8*)ev, ev->size, 16);
                SysPacketEvent* packet = (SysPacketEvent*)ev;
                SV_ReadPacket(packet);
            } break;

            case SYS_EVENT_INPUT:
            {
                printf("SV Input - skip\n");
            } break;
		}
	}
}

void SV_Tick(ByteBuffer* sysEvents, f32 deltaTime)
{
    #if 0
    u8* read = input->ptrStart;
    u8* end = input->ptrWrite;
    while(read < end)
    {
        Command* h = (Command*)read;
        i32 err = Cmd_Validate(h);
        Assert(err == COM_ERROR_NONE)
        read += h->size;
        switch (h->type)
        {
            case CMD_TYPE_USER_JOINED:
            {
                CmdUserJoined* cmd = (CmdUserJoined*)h;
                printf("SV User %d joined\n", cmd->privateId);
                SV_CreateUser(cmd->privateId);
            } break;
        }
    }
    #endif
    SV_WriteTestPacket();
    Sim_Tick(&g_sim, deltaTime);
    
	#if 0
	SV_WriteTestPacket();
	for (i32 i = 0; i < g_users.max; ++i)
	{
		User* user = &g_users.items[i];
		if (user->state == USER_STATE_FREE) { continue; }
		
		SV_WriteUserPacket(user);
	}
	#endif
	g_ellapsed += deltaTime;
    g_ticks++;
}

void SV_PopulateRenderScene(
    RenderScene* scene,
    i32 maxObjects,
    i32 texIndex,
    f32 interpolateTime)
{

    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }

        RendObj obj = {};
        MeshData* cube = COM_GetCubeMesh();
        RendObj_SetAsMesh(
            &obj, *cube, 1, 0, 0, texIndex);
        RScene_AddRenderItem(scene, &ent->t, &obj);
    }
}
