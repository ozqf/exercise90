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
internal f32 g_elapsed = 0;
i32 SV_IsRunning() { return g_isRunning; }

#include "server_game.h"
#include "server_packets.h"

void SV_WriteDebugString(ZStringHeader* str)
{
    char* chars = str->chars;
    i32 written = 0;
    written += sprintf_s(chars, str->maxLength,
        "SERVER:\nTick: %d\nElapsed: %.3f\n",
        g_ticks, g_elapsed
    );
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;
    User* user = User_FindByAddress(&g_users, &addr);
    if (user)
    {
        AckStream* acks = &user->acks;
        written += sprintf_s(chars + written, str->maxLength - written,
            "-- Local Client %d --\nOutput seq: %d\nAck Seq: %d\nDelay: %.3f\nJitter: %.3f\n",
            user->ids.privateId,
            acks->outputSequence,
            acks->remoteSequence,
            Ack_CalculateAverageDelay(acks),
			(acks->delayMax - acks->delayMin)
		);
        #if 0
		// currently overflows debug text buffer:
        for (i32 j = 0; j < ACK_CAPACITY; ++j)
        {
            AckRecord* rec = &acks->awaitingAck[j];
            if (rec->acked)
            {
                f32 time = rec->receivedTime - rec->sentTime;
                written += sprintf_s(chars + written, str->maxLength - written,
                    "%.3f Sent: %.3f Rec: %.3f\n", time, rec->sentTime, rec->receivedTime
                );
            }
        }
		#endif
    }
    else
    {
        written += sprintf_s(chars + written, str->maxLength - written,
            "No local client found\n");
    }
    str->length = written;
}

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

            

/*ByteBuffer* SV_GetPlatformInput()
{
    return &g_platformInput;
}*/

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

internal void SV_EnqueueCommandForAllUsers(UserList* users, Command* cmd)
{
	for (i32 i = 0; i < users->max; ++i)
	{
		User* user = &users->items[i];
		if (user->state == USER_STATE_FREE)
		{ continue; }
	
		//
		Stream_EnqueueOutput(&user->reliableStream, cmd);
	}
}

internal void SV_UserStartSync(User* user)
{
    printf("SV - Begin sync for user %d\n", user->ids.privateId);
    // start user command queue
    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        if (ent->isLocal) { continue; }
        S2C_SpawnEntity cmd = {};
        // TODO: Passing in sequence 0 as it is set by the stream when enqueued anyway
        // is manually setting it ever required?
        Cmd_InitSpawnEntity(&cmd, g_ticks, 0);
        cmd.entType = (u8)ent->entType;
        cmd.networkId = ent->id.serial;
        cmd.pos = ent->t.pos;
        cmd.vel = ent->velocity;
        cmd.pitch = ent->pitch;
        cmd.yaw = ent->yaw;
        ByteBuffer* b = &user->reliableStream.outputBuffer;
        Stream_EnqueueOutput(&user->reliableStream, (Command*)&cmd);
        printf("  Write Entity %d\n", ent->id.serial);
    }
}

// This is public so that a local client can be made easily
internal User* SV_CreateUser(UserIds ids, ZNetAddress* addr)
{
    User* user = User_GetFree(&g_users);
    user->ids = ids;
    user->address = *addr;
    SV_AllocateUserStream(&user->reliableStream, KiloBytes(64));
    SV_AllocateUserStream(&user->unreliableStream, KiloBytes(64));
    printf("SV creating new user public %d private %d\n",
        ids.publicId, ids.privateId
    );
    return user;
}

UserIds SV_CreateLocalUser()
{
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;
    UserIds id = SV_GenerateUserId();
    User* u = SV_CreateUser(id, &addr);
    u->state = USER_STATE_SYNC;
    SV_UserStartSync(u);
    return id;
}

internal void SV_LoadTestScene()
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
        def.isLocal = 0;
        def.entType = SIM_ENT_TYPE_WANDERER;
        def.pos[0] = x;
        def.pos[1] = y;
        def.pos[2] = z;
        def.velocity[0] = x;
        
        def.velocity[2] = z;
        Sim_AddEntity(&g_sim, &def);
    }
    #endif

    def = {};
    def.isLocal = 1;
    def.serial = Sim_ReserveEntitySerial(&g_sim, 1);
    def.pos[1] = 1;
    def.entType = SIM_ENT_TYPE_TURRET;
    def.scale[0] = 1;
    def.scale[1] = 1;
    def.scale[2] = 1;
    Sim_AddEntity(&g_sim, &def);
	
	// World geometry
    #if 0
    def = {};
    def.isLocal = 1;
    def.pos[1] = 0;
    def.entType = SIM_ENT_TYPE_WORLD;
    def.scale[0] = 12;
    def.scale[1] = 1;
    def.scale[2] = 12;
    // Test: this should be ignored by the sim as world entities cannot move
    def.velocity[0] = 3;
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
    APP_PRINT(64, "SV Init scene\n");

    g_mallocs = COM_InitMallocList(g_mallocItems, SV_MAX_MALLOCS);

    g_users = {};
    g_users.max = APP_MAX_USERS;
    i32 userBytes = sizeof(User) * APP_MAX_USERS;
    User* users = (User*)COM_Malloc(&g_mallocs, userBytes, "SV Users");
    g_users.items = users;

    i32 size = KiloBytes(64);

    i32 maxEnts = 2048;
    size = Sim_CalcEntityArrayBytes(maxEnts);
    SimEntity* mem = (SimEntity*)COM_Malloc(&g_mallocs, size, "Sim Ents");
    Sim_InitScene(&g_sim, mem, maxEnts);
	Sim_Reset(&g_sim);
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
				//COM_PrintBytes((u8*)ev, ev->size, 16);
                SysPacketEvent* packet = (SysPacketEvent*)ev;
				// Don't read packets intended for the client
				if (packet->sender.port == APP_CLIENT_LOOPBACK_PORT)
				{
					continue;
				}
                SV_ReadPacket(packet, g_elapsed);
            } break;

            case SYS_EVENT_INPUT:
            {
                printf("SV Input - skip\n");
            } break;

            case SYS_EVENT_SKIP: break;
		}
	}
}

internal void SV_SendUserPackets(f32 deltaTime)
{
	for (i32 i = 0; i < g_users.max; ++i)
	{
		User* user = &g_users.items[i];
		if (user->state == USER_STATE_FREE) { continue; }
		
		SV_WriteUserPacket(user, g_elapsed);
	}
}

void SV_Tick(ByteBuffer* sysEvents, f32 deltaTime)
{
    SV_ReadSystemEvents(sysEvents, deltaTime);
    SVG_TickSim(&g_sim, deltaTime);
	g_elapsed += deltaTime;
    g_ticks++;
    SV_SendUserPackets(deltaTime);
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
