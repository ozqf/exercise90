#pragma once

#include <stdlib.h>
#include "../../common/com_module.h"
#include "server.h"
#include "../packet.h"
#include "../../interface/sys_events.h"
#include "../../sim/sim.h"

struct SVEntityFrame
{
    i32 latestFrame;
    Vec3 pos;
};

#define SV_MAX_MALLOCS 1024

internal void SV_EnqueueCommandForAllUsers(
    UserList* users, Command* cmd);

internal MallocItem g_mallocItems[SV_MAX_MALLOCS];
internal MallocList g_mallocs;
internal UserList g_users;
internal SimScene g_sim;

internal i32 g_isRunning = 0;
internal i32 g_ticks = 0;
internal f32 g_elapsed = 0;
internal i32 g_lagCompensateProjectiles = 1;

/*
Record entity states for lag compensation rewind
Local entities are not compensated.
Access by Frame number, then entity slot.
*/
#define SV_NUM_POSITION_FRAMES_RECORDED 60
internal SVEntityFrame* g_entityRecords = NULL;

i32 SV_IsRunning() { return g_isRunning; }

#include "server_game.h"
#include "server_priority.h"
#include "server_packets.h"

internal void SV_PrintMsgSizes()
{
    APP_LOG(64, "SV sizeof ping: %d\n",
        sizeof(CmdPing))
    APP_LOG(64, "SV sizeof S2C_EntitySync: %d\n",
        sizeof(S2C_EntitySync))
}

void SV_WriteDebugString(ZStringHeader* str)
{
    char* chars = str->chars;
    i32 written = 0;
    written += sprintf_s(chars, str->maxLength,
        "SERVER:\nTick: %d\nElapsed: %.3f\n",
        g_ticks, g_elapsed
    );
    
    // TODO: Just showing local user for now
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;
    User* user = User_FindByAddress(&g_users, &addr);
    if (user)
    {
        AckStream* acks = &user->acks;
        written += sprintf_s(
            chars + written,
            str->maxLength - written,
            "-- Local Client %d --\nOutput seq: %d\nAck Seq: %d\nDelay: %.3f\nJitter: %.3f\n",
            user->ids.privateId,
            acks->outputSequence,
            acks->remoteSequence,
            user->ping,
			user->jitter
		);
        written += sprintf_s(
            chars + written,
            str->maxLength - written,
            "\tOutbytes: %d\n",
            user->reliableStream.outputBuffer.Written()
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
        written += sprintf_s(
            chars + written, str->maxLength - written,
            "No local client found\n");
    }
    str->length = written;
}

u8 SV_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
	if (COM_CompareStrings(tokens[0], "SPAWN"))
	{
		if (numTokens == 1)
		{
			APP_PRINT(64, "SV === Spawn entity options: ===\n");
			APP_PRINT(64, "\tWANDERER\n");
			return 1;
		}
		if (numTokens == 2)
		{
			if (COM_CompareStrings(tokens[1], "WANDERER"))
			{
				APP_PRINT(128, "\tSV Spawn Wanderer\n");
			}
			else
			{
				APP_PRINT(256, "\tUnknown entity type %s\n",
                    tokens[1]);
			}
			return 1;
		}
		APP_PRINT(64, "SV incorrect token count for spawn\n");
		return 1;
	}
    return 0;
}

internal i32 SV_IsPrivateIdInUse(i32 id)
{
    for (i32 i = 0; i < g_users.max; ++i)
    {
        User* u = &g_users.items[i];
        if (u->ids.privateId == id)
        {
            return 1;
        }
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

internal void SV_AllocateUserStream(
    NetStream* stream, i32 capacityPerBuffer)
{
    if (stream->initialised) { return; }
    stream->initialised = 1;
    stream->inputBuffer = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, capacityPerBuffer, "User Input"),
        capacityPerBuffer
    );
    stream->outputBuffer = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, capacityPerBuffer, "User Output"),
        capacityPerBuffer
    );
}

internal void SV_EnqueueCommandForAllUsers(
    UserList* users, Command* cmd)
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
    APP_PRINT(128, "SV - Begin sync for user %d\n",
        user->ids.privateId);
    NetStream* stream = &user->reliableStream;

    S2C_Sync sync;
    Cmd_InitSync(&sync, g_ticks, 0, g_ticks, 8, user->entSerial);
    Stream_EnqueueOutput(stream, &sync.header);
    // start user command queue
    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        if (ent->isLocal) { continue; }
        S2C_RestoreEntity cmd = {};

        // TODO: Passing in sequence 0 as it is set by the stream when enqueued anyway
        // is manually setting it ever required?
        Cmd_InitRestoreEntity(&cmd, g_ticks, 0);
        
        // TODO: Any entity specific spawning stuff here
        cmd.factoryType = (u8)ent->factoryType;
        cmd.networkId = ent->id.serial;
        cmd.pos = ent->body.t.pos;
        cmd.vel = ent->body.velocity;
        cmd.pitch = ent->body.pitch;
        cmd.yaw = ent->body.yaw;

        ByteBuffer* b = &user->reliableStream.outputBuffer;
        Stream_EnqueueOutput(&user->reliableStream, (Command*)&cmd);
        APP_LOG(64, "  Write Entity %d\n", ent->id.serial);
    }
    APP_LOG(64, "SV User %d has %d sync bytes\n",
        user->ids.privateId, stream->outputBuffer.Written());
}

// This is public so that a local client can be made easily
internal User* SV_CreateUser(UserIds ids, ZNetAddress* addr)
{
    User* user = User_GetFree(&g_users);
    user->ids = ids;
    user->address = *addr;
    SV_AllocateUserStream(&user->reliableStream, KiloBytes(64));
    SV_AllocateUserStream(&user->unreliableStream, KiloBytes(64));
    APP_LOG(64, "SV creating new user public %d private %d\n",
        ids.publicId, ids.privateId
    );
    return user;
}

internal void SV_SpawnUserAvatar(User* u)
{
	SimEntityDef def = {};
    def = {};
    //def.isLocal = 1;
	i32 avatarSerial = Sim_ReserveEntitySerial(&g_sim, 0);
	u->entSerial = avatarSerial;
    def.serial = avatarSerial;
	def.factoryType = SIM_FACTORY_TYPE_ACTOR;
    def.pos = { -6, 0, 6 };
    def.scale = { 1, 1, 1 };
    Sim_RestoreEntity(&g_sim, &def);
}

UserIds SV_CreateLocalUser()
{
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;
    UserIds id = SV_GenerateUserId();
    User* u = SV_CreateUser(id, &addr);
    u->state = USER_STATE_SYNC;
	SV_SpawnUserAvatar(u);
    SV_UserStartSync(u);
    return id;
}

internal void SV_AddWanderer()
{
    SimEntityDef def = {};
    def = {};
    def.isLocal = 0;
    def.serial = Sim_ReserveEntitySerial(&g_sim, def.isLocal);
    printf("SV Reserver serial %d for Wanderer\n", def.serial);
    def.pos.x = COM_STDRandomInRange(-8, 8);
    def.pos.y = 0;
    def.pos.z = COM_STDRandomInRange(-8, 8);
    def.factoryType = SIM_FACTORY_TYPE_WANDERER;
    def.scale = { 1, 1, 1 };
    Sim_RestoreEntity(&g_sim, &def);   
}

internal void SV_AddSpawner(SimScene* sim, Vec3 pos, simFactoryType factoryType)
{
    SimEntityDef def = {};
    def = {};
    def.isLocal = 1;
    def.serial = Sim_ReserveEntitySerial(&g_sim, 1);
    def.pos = pos;
    def.childFactoryType = factoryType;
    def.factoryType = SIM_FACTORY_TYPE_TURRET;
    def.scale = { 1, 1, 1 };
    Sim_RestoreEntity(&g_sim, &def);
}

internal void SV_LoadTestScene()
{
    SimScene* sim = &g_sim;
    Sim_LoadScene(sim, 0);
	
    // Place a test spawner
    SV_AddSpawner(sim, { -6, 0, 0 }, SIM_FACTORY_TYPE_BOUNCER);
    SV_AddSpawner(sim, { 6, 0, 0 }, SIM_FACTORY_TYPE_SEEKER);
    
    i32 numWanderers = 6;
    for (i32 i = 0; i < numWanderers; ++i)
    {
        SV_AddWanderer();
    }
    printf("SV spawned %d wanderers\n", numWanderers);
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

internal void SV_ResetEntityPositionRecords()
{
    i32 bytesPerFrame = sizeof(SVEntityFrame) * APP_MAX_ENTITIES;
    i32 bytesTotal = bytesPerFrame * SV_NUM_POSITION_FRAMES_RECORDED;
    if (g_entityRecords == NULL)
    {
        g_entityRecords = (SVEntityFrame*)COM_Malloc(
            &g_mallocs, bytesTotal, "Entity Frames");
    }
    COM_ZeroMemory((u8*)g_entityRecords, bytesTotal);
}

void SV_Init()
{
    APP_PRINT(64, "SV Init scene\n");

    SV_PrintMsgSizes();

    // force time and ticks forward for debugging
    g_ticks = 1000;
    g_elapsed = g_ticks * (1.0f / 60.0f);

    g_mallocs = COM_InitMallocList(g_mallocItems, SV_MAX_MALLOCS);

    g_users = {};
    g_users.max = APP_MAX_USERS;
    i32 userBytes = sizeof(User) * APP_MAX_USERS;
    User* users = (User*)COM_Malloc(&g_mallocs, userBytes, "SV Users");
    g_users.items = users;

    SV_ResetEntityPositionRecords();

    i32 size;// = KiloBytes(64);

    i32 maxEnts = APP_MAX_ENTITIES;
    size = Sim_CalcEntityArrayBytes(maxEnts);
    SimEntity* mem = (SimEntity*)COM_Malloc(&g_mallocs, size, "Sim Ents");
    Sim_Init(&g_sim, mem, maxEnts);
	Sim_Reset(&g_sim);
    SV_LoadTestScene();

    SV_ListAllocs();
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

internal void SV_CalcPings(f32 deltaTime)
{
    for (i32 i = 0; i < g_users.max; ++i)
    {
        User* u = &g_users.items[i];
        if (u->state == USER_STATE_FREE) { continue; }
        AckStream* acks = &u->acks;
        u->ping = Ack_CalculateAverageDelay(acks);
        u->jitter = (acks->delayMax - acks->delayMin);
    }
}

void SV_Tick(ByteBuffer* sysEvents, f32 deltaTime)
{
    APP_LOG(64, "*** SV TICK %d (T %.3f) ***\n", g_ticks, g_elapsed);
    SV_ReadSystemEvents(sysEvents, deltaTime);
    SV_CalcPings(deltaTime);
    SVG_TickSim(&g_sim, deltaTime);
	g_elapsed += deltaTime;
    g_ticks++;
    SV_SendUserPackets(deltaTime);
}

void SV_PopulateRenderScene(
    RenderScene* scene,
    i32 maxObjects,
    i32 texIndex,
    f32 interpolateTime,
    i32 g_debugDrawServerScene,
    i32 g_debugDrawServerTests)
{
    if (!g_debugDrawServerScene && !g_debugDrawServerTests) { return; }
    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }

        RendObj obj = {};
        //MeshData* cube = COM_GetCubeMesh();
        //RendObj_SetAsMesh(
        //    &obj, *cube, 1, 0, 0, texIndex);

        switch(ent->tickType)
        {
            case SIM_TICK_TYPE_LINE_TRACE:
            {
                if (!g_debugDrawServerTests) { break; }
                Vec3* a = &ent->body.t.pos;
                Vec3* b = &ent->destination;
                // move ray up slightly out of the floor
                f32 offsetY = 0.2f;
                RendObj_SetAsLine(&obj,
                    { a->x, a->y + offsetY, a->z },
                    { b->x, b->y + offsetY, b->z },
                    COL_U32_PURPLE,
                    COL_U32_PURPLE
                );
                TRANSFORM_CREATE(t);
                RScene_AddRenderItem(scene, &t, &obj);
            } break;

            default:
            {
                if (!g_debugDrawServerScene) { break; }
                RendObj_SetAsAABB(
			        &obj, 1, 1, 1, 0, 1, 0);
                RScene_AddRenderItem(scene, &ent->body.t, &obj);
            } break;
        }
    }
}
