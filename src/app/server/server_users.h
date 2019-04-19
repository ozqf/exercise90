#pragma once

#include "../../common/com_module.h"
#include "server.cpp"


internal i32 SVU_CreateSalt()
{
    i32 result = 0;
    do
    {
        result = (i32)(COM_STDRandf32() * INT_MAX);
    } while (result == 0);
    return result;
}

internal i32 SVU_IsPrivateIdInUse(i32 id)
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

internal UserIds SVU_GenerateUserId()
{
    UserIds newId;
    do
    {
        newId.privateId = SVU_CreateSalt();
    } while (SVU_IsPrivateIdInUse(newId.privateId));
    newId.publicId = g_users.nextPublicId++;
    return newId;
}

internal void SVU_AllocateUserStream(
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

internal void SVU_EnqueueCommandForAllUsers(
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

internal void SVU_AddEntityLinkForAllUsers(
    UserList* users, i32 entSerial, i32 priority)
{
    for (i32 i = 0; i < users->max; ++i)
    {
        User* user = &users->items[i];
        if (user->state == USER_STATE_FREE) { continue; }
        SV_AddPriorityLink(&user->entSync, entSerial, 1);
    }
}

internal void SVU_AddBulkEntityLinksForAllUsers(
    UserList* users, i32 firstSerial, i32 numEntities)
{
    for (i32 i = 0; i < numEntities; ++i)
    {
        SVU_AddEntityLinkForAllUsers(users, firstSerial++, 1);
    }
}

internal void SVU_RemoveEntityLinkForAllUsers(
    UserList* users, i32 entSerial)
{
    for (i32 i = 0; i < g_users.max; ++i)
    {
        User* u = &g_users.items[i];
        if (u->state == USER_STATE_FREE) { continue; }
        SV_RemovePriorityLinkBySerial(&u->entSync, entSerial);
    }
}

internal void SVU_StartUserSync(User* user)
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

        // Add an entity link if required
        if (ent->flags & SIM_ENT_FLAG_POSITION_SYNC)
        {
            SV_AddPriorityLink(&user->entSync, ent->id.serial, 1);
        }

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

internal User* SVU_CreateUser(UserIds ids, ZNetAddress* addr)
{
    User* user = User_GetFree(&g_users);
    user->ids = ids;
    user->address = *addr;
    SVU_AllocateUserStream(&user->reliableStream, KiloBytes(64));
    SVU_AllocateUserStream(&user->unreliableStream, KiloBytes(64));

    user->entSync = {};
    user->entSync.links = (SVEntityLink*)COM_Malloc(
        &g_mallocs,
        SV_CalcEntityLinkArrayBytes(APP_MAX_ENTITIES),
        "EntLinks");
    user->entSync.maxLinks = APP_MAX_ENTITIES;
    user->entSync.numLinks = 0;
    //user->syncRateHertz = APP_CLIENT_SYNC_RATE_10HZ;
    //user->syncRateHertz = APP_CLIENT_SYNC_RATE_20HZ;
    //user->syncRateHertz = APP_CLIENT_SYNC_RATE_30HZ;
    user->syncRateHertz = APP_CLIENT_SYNC_RATE_60HZ;

    APP_LOG(64, "SV creating new user public %d private %d\n",
        ids.publicId, ids.privateId
    );
    return user;
}

internal void SVU_SpawnUserAvatar(User* u)
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

UserIds SVU_CreateLocalUser()
{
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;
    UserIds id = SVU_GenerateUserId();
    User* u = SVU_CreateUser(id, &addr);
    u->state = USER_STATE_SYNC;
	SVU_SpawnUserAvatar(u);
    SVU_StartUserSync(u);
    return id;
}
