#pragma once

#include <stdlib.h>
#include "../../common/com_module.h"
#include "server.h"
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

internal void SV_AllocateUserStreams(NetStream* stream, i32 capacityPerBuffer)
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

UserIds SV_CreateLocalUser()
{
    User* user = User_GetFree(&g_users);
    user->ids.privateId = 0xDEADDEAD;
    user->ids.publicId = g_users.nextPublicId++;
    SV_AllocateUserStreams(&user->reliableStream, KiloBytes(64));
    UserIds ids = user->ids;
    printf("SV creating local user public %d private %d",
        ids.publicId, ids.privateId
    );
    return ids;
}

void SV_LoadTestScene()
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

void SV_EnqueueReliableOutput(User* user, Command* cmd)
{
    ByteBuffer* b = &user->reliableStream.outputBuffer;
    Assert(b->Space() >= cmd->size)
    b->ptrWrite += COM_CopyMemory((u8*)cmd, b->ptrWrite, cmd->size);
}

void SV_BuildUserPacket(User* user, u8* buf, i32 capacity)
{

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

void SV_WriteUserPacket(User* user)
{
	// Send ping
	CmdPing ping = {};
	// TODO: Stream enqueue will set the sequence for us
	// so remove sending 0 here.
	Cmd_Prepare(&ping.header, g_ticks, 0);
	ping.sendTime = g_ellapsed;
	Stream_EnqueueReliableOutput(&user->reliableStream, &ping.header);
	
	// enqueue
	//ByteBuffer* buf = App_GetLocalClientPacketForWrite();
	
	u8 buf[1400];
	Packet_WriteFromStream(
        &user->reliableStream, &user->unreliableStream, buf, 1400, g_ellapsed, g_ticks, 0);
}

void SV_Tick(f32 deltaTime)
{
    Sim_Tick(&g_sim, deltaTime);
	
	
	for (i32 i = 0; i < g_users.max; ++i)
	{
		User* user = &g_users.items[i];
		if (user->state == USER_STATE_FREE) { continue; }
		
		
	}
	
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
