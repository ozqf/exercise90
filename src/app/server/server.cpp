#ifndef SERVER_CPP
#define SERVER_CPP

#include <stdlib.h>
#include "../../common/common.h"
#include "server.h"
#include "../packet.h"
#include "../../interface/sys_events.h"
#include "../../sim/sim.h"
#include "server_priority.h"

struct SVEntityFrame
{
    i32 latestFrame;
    Vec3 pos;
};

#define SV_DEBUG_TIMING (1 << 0)
#define SV_DEBUG_USER_BANDWIDTH (1 << 1)

#define SV_MAX_MALLOCS 1024

#define SV_PACKET_MAX_BYTES 1400
#define SV_PACKET_RELIABLE_MAX_BYTES 700

//internal void SVU_EnqueueCommandForAllUsers(
//    UserList* users, Command* cmd);

internal MallocItem g_mallocItems[SV_MAX_MALLOCS];
internal MallocList g_mallocs;
internal UserList g_users;
internal SimScene g_sim;

internal i32 g_isRunning = 0;
internal i32 g_ticks = 0;
internal f32 g_elapsed = 0;
internal i32 g_lagCompensateProjectiles = 1;
internal i32 g_unreliableProjectileDeaths = 1;

internal i32 g_maxSyncRate = APP_CLIENT_SYNC_RATE_20HZ;

internal i32 g_debugFlags = SV_DEBUG_TIMING | SV_DEBUG_USER_BANDWIDTH;

/*
Record entity states for lag compensation rewind
Local entities are not compensated.
Access by Frame number, then entity slot.
*/
#define SV_NUM_POSITION_FRAMES_RECORDED 60
internal SVEntityFrame* g_entityRecords = NULL;

i32 SV_IsRunning() { return g_isRunning; }

#include "server_users.h"
#include "server_game.h"
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
    if (g_debugFlags & SV_DEBUG_TIMING)
    {
        written += sprintf_s(chars, str->maxLength,
            "SERVER:\nTick: %d\nElapsed: %.3f\nMax Rate %d\nNext remote ent: %d\n",
            g_ticks, g_elapsed, g_maxSyncRate, g_sim.remoteEntitySequence
        );
    }
    
    
    // TODO: Just showing local user for now
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;
    User* user = User_FindByAddress(&g_users, &addr);
    if (user)
    {
        AckStream* acks = &user->acks;
        
        // title
        written += sprintf_s(
            chars + written,
            str->maxLength - written,
            "-- Local Client %d --\n",
            user->ids.privateId
		);
        // Bandwidth
        if (g_debugFlags & SV_DEBUG_USER_BANDWIDTH)
        {
            #if 1
            StreamStats stats;
            User_SumPacketStats(user, &stats);
            if (stats.numPackets > 0)
            {
                i32 kbpsTotal = (stats.totalBytes * 8) / 1024;
                i32 reliableKbps = (stats.reliableBytes * 8) / 1024;
                i32 unreliableKbps = (stats.unreliableBytes * 8) / 1024;
                f32 lossEstimate = Ack_EstimateLoss(&user->acks);
                i32 numEnqueued = Stream_CountCommands(
                    &user->reliableStream.outputBuffer);
                written += sprintf_s(
                    chars + written,
                    str->maxLength - written,
                    "-Bandwidth -\nRate: %d\nPer Second: %dkbps (%.3f KB)\nReliable: %d kbps\nUnreliable: %d kbps\nLoss %.1f%%\nEnqueued: %d (%d Bytes)\n",
                    user->syncRateHertz,
                    kbpsTotal,
                    (f32)stats.totalBytes / 1024.0f,
                    reliableKbps,
                    unreliableKbps,
                    lossEstimate,
                    numEnqueued,
                    user->reliableStream.outputBuffer.Written()
		        );
                #if 1
                // Sequencing/jitter
                written += sprintf_s(
                    chars + written,
                    str->maxLength - written,
                    "- Latency -\nOutput seq: %d\nAck Seq: %d\nDelay: %.3f\nJitter: %.3f\n",
                    acks->outputSequence,
                    acks->remoteSequence,
                    user->ping,
		        	user->jitter
                );
                #endif
                i32 numLinks = user->entSync.numLinks;
                i32 numDeadLinks = Priority_TallyDeadLinks(
                    user->entSync.links, user->entSync.numLinks);
                written += sprintf_s(
                    chars + written,
                    str->maxLength - written,
                    "- Cmds/Sync Links -\nLinks %d\nDead Links %d\nLifetime max %.3f\nCurrent max %.3f\n--Per packet averages--\nReliable Cmds %d\nUnreliable Cmds %d\nPacket Size %d\n",
                    numLinks,
                    numDeadLinks,
                    user->entSync.highestMeasuredPriority,
                    user->entSync.currentHighest,
                    stats.numReliableMessages / stats.numPackets,
                    stats.numUnreliableMessages / stats.numPackets,
                    stats.totalBytes / stats.numPackets
		        );
                
                written += sprintf_s(
                    chars + written,
                    str->maxLength - written,
                    "--Last second totals --\nReliable Cmds %d\nUnreliable Cmds %d\n",
                    stats.numReliableMessages,
                    stats.numUnreliableMessages
		        );
            }
            for (i32 i = 0; i < 256; ++i)
            {
                i32 count = stats.commandCounts[i];
                if (count == 0) { continue; }
                written += sprintf_s(
                    chars + written,
                    str->maxLength - written,
                    "\tType %d: %d\n",
                    i, count);
            }
            #endif
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
	if (!COM_CompareStrings(tokens[0], "SPAWN"))
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
    else if (numTokens == 2 && !COM_CompareStrings(tokens[0], "RATE"))
    {
        i32 value = COM_AsciToInt32(tokens[1]);
        printf("SV Set Sync rate %d\n", value);
        switch (value)
        {
            case 10:
            g_maxSyncRate = APP_CLIENT_SYNC_RATE_10HZ;
            return 1;
            case 20:
            g_maxSyncRate = APP_CLIENT_SYNC_RATE_20HZ;
            return 1;
            case 30:
            g_maxSyncRate = APP_CLIENT_SYNC_RATE_30HZ;
            return 1;
            case 60:
            g_maxSyncRate = APP_CLIENT_SYNC_RATE_60HZ;
            return 1;

            default:
            printf("Invalid sync rate %d\n", value);
            break;
        }
        return 1;
    }
    return 0;
}

internal void SV_AddWanderer()
{
    SimEntSpawnData def = {};
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
    //SimEntSpawnData def = {};
    //def = {};
    //def.isLocal = 1;
    //def.serial = Sim_ReserveEntitySerial(&g_sim, 1);
    //def.pos = pos;
    //def.childFactoryType = factoryType;
    //def.factoryType = SIM_FACTORY_TYPE_SPAWNER;
    SimEntSpawnData data = {};
    Sim_PrepareSpawnData(sim, &data, 1, SIM_FACTORY_TYPE_SPAWNER, pos);
    data.childFactoryType = factoryType;
    Sim_RestoreEntity(&g_sim, &data);
}

internal void SV_AddBot(SimScene* sim, Vec3 pos)
{
    SimEntSpawnData data = {};
    Sim_PrepareSpawnData(sim, &data, 1, SIM_FACTORY_TYPE_BOT, pos);
    SimEntity* ent = Sim_RestoreEntity(&g_sim, &data);
    S2C_RestoreEntity cmd = {};
    Cmd_InitRestoreEntity(&cmd, g_ticks, ent);
}

internal void SV_LoadTestScene()
{
    SimScene* sim = &g_sim;
    Sim_LoadScene(sim, 0);
    const i32 stage = 3;
	switch (stage)
    {
        case 1:
        SV_AddSpawner(sim, { 10, 0, 10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { -10, 0, 10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { 10, 0, -10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { -10, 0, -10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { 0, 0, 0 }, SIM_FACTORY_TYPE_SEEKER);
        break;
        case 2:
        SV_AddSpawner(sim, { -10, 0, 10 }, SIM_FACTORY_TYPE_BOUNCER);
        SV_AddSpawner(sim, { 10, 0, -10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { 0, 0, 0 }, SIM_FACTORY_TYPE_DART);
        break;
        case 3:
        SV_AddSpawner(sim, { 10, 0, 10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { -10, 0, 10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { 10, 0, -10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { -10, 0, -10 }, SIM_FACTORY_TYPE_SEEKER);
        SV_AddSpawner(sim, { 0, 0, 0 }, SIM_FACTORY_TYPE_DART);
        break;
        default:
        SV_AddSpawner(sim, { 0, 0, 0 }, SIM_FACTORY_TYPE_SEEKER);
        break;
    }

    SV_AddBot(sim, { -10, 0, 0 });

    // Place a test spawner
    // -10 Z == further away
    //SV_AddSpawner(sim, { -10, 0, -10 }, SIM_FACTORY_TYPE_BOUNCER);

    //SV_AddSpawner(sim, { -10, 0, 10 }, SIM_FACTORY_TYPE_WANDERER);
    //SV_AddSpawner(sim, { 10, 0, -10 }, SIM_FACTORY_TYPE_DART);
    
    //SV_AddSpawner(sim, { 0, 0, 0 }, SIM_FACTORY_TYPE_RUBBLE);
    //SV_AddSpawner(sim, { 0, 0, 0 }, SIM_FACTORY_TYPE_SEEKER);



    i32 numWanderers = 0;
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
    printf("SV sizeof(Command): %d\n", sizeof(Command)); 
    printf("SV sizeof(S2C_EntitySync): %d\n", sizeof(S2C_EntitySync));
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
                SVP_ReadPacket(packet, g_elapsed);
            } break;

            case SYS_EVENT_INPUT:
            {
                printf("SV Input - skip\n");
            } break;

            case SYS_EVENT_SKIP: break;
		}
	}
}

internal void SV_SendUserPackets(SimScene* sim, f32 deltaTime)
{
    for (i32 i = 0; i < g_users.max; ++i)
	{
		User* user = &g_users.items[i];
		if (user->state == USER_STATE_FREE) { continue; }

        // Clean out reliable queue if possible
        SVU_ClearStaleOutput(sim, &user->reliableStream.outputBuffer);

        // Update priorities
        SimEntity* avatar = Sim_GetEntityBySerial(sim, user->entSerial);
        if (avatar != NULL)
        {
            SVP_CalculatePriorities(
                sim, avatar, user->entSync.links, user->entSync.numLinks);
        }
        user->packetStats[g_ticks % USER_NUM_PACKET_STATS] = {};
        // force sending rate
        i32 rate;
        if (user->syncRateHertz < g_maxSyncRate)
        {
            rate = user->syncRateHertz;
        }
        else
        {
            rate = g_maxSyncRate;
        }
        
        switch (rate)
        {
            case APP_CLIENT_SYNC_RATE_30HZ:
            if (g_ticks % 2 != 0) { continue; }
            break;
            case APP_CLIENT_SYNC_RATE_20HZ:
            if (g_ticks % 3 != 0) { continue; }
            break;
            case APP_CLIENT_SYNC_RATE_10HZ:
            if (g_ticks % 6 != 0) { continue; }
            break;
            case APP_CLIENT_SYNC_RATE_60HZ:
            default:
            break;
        }

		Priority_TickQueue(&user->entSync);
        
        PacketStats stats = SVP_WriteUserPacket(sim, user, g_elapsed);
        user->packetStats[g_ticks % USER_NUM_PACKET_STATS] = stats;

        // add to totals
        user->streamStats.numPackets += 1;
        user->streamStats.totalBytes += stats.totalBytes;
        user->streamStats.reliableBytes += stats.reliableBytes;
        user->streamStats.unreliableBytes += stats.unreliableBytes;
        user->streamStats.numReliableMessages += stats.numReliableMessages;
        user->streamStats.numReliableSkipped += stats.numReliableSkipped;
        user->streamStats.numUnreliableMessages += stats.numUnreliableMessages;

        //printf(".");
        // Add up command stats here
        for (i32 j = 0; j < 256; ++j)
        {
            user->streamStats.commandCounts[j] += stats.commandCounts[j];
        }
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
    SV_SendUserPackets(&g_sim, deltaTime);
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

#endif // SERVER_CPP
