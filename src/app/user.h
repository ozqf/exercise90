#pragma once

#include "../common/com_module.h"

#define USER_STATE_FREE 0
#define USER_STATE_SYNC 1
#define USER_STATE_OBSERVING 2
//////////////////////////////////////////////////
// Clients and players
//////////////////////////////////////////////////
struct UserIds
{
    // public identity for client to client and local application
    i32 publicId;
    // private between client and server
    i32 privateId;
};

struct User
{
    UserIds ids;
    i32 state;
	u32 flags;
    i32 isLocal;

    i32 entSerial;

    f32 ping;
    f32 jitter;
    i32 smoothingTicks;
    i32 userInputSequence;
	
    ZNetAddress address;
	AckStream acks;
    NetStream reliableStream;
    NetStream unreliableStream;
    // Clients are considered in sync mode until their acknowledged
    // reliable message queue Id is >= to this.
    // Set to u32 max value when client first connects.
    // Set properly after all sync messages have been buffered for 
    // transmission.
    u32 syncCompleteMessageId;
};

struct UserList
{
    User* items;
    i32 count;
    i32 max;
    i32 nextPublicId;
    // id of the client this exe represents
    // 0 == no local client
    i32 localUserId = 0;
};

internal User* User_GetFree(UserList* users)
{
    for (i32 i = 0; i < users->max; ++i)
    {
        User* user = &users->items[i];
        if (user->state == USER_STATE_FREE)
        {
            user->state = USER_STATE_SYNC;
            return user;
        }
    }
    return NULL;
}

internal void User_Free(UserList* users, User* user)
{
    user->state = USER_STATE_FREE;
}

internal User* User_FindByPrivateId(UserList* users, i32 privateId)
{
    for (i32 i = 0; i < users->max; ++i)
    {
        User* user = &users->items[i];
        if (user->state == USER_STATE_FREE) { continue; }
        if (user->ids.privateId == privateId) { return user; }
    }
    return NULL;
}

internal User* User_FindByPublicId(UserList* users, i32 publicId)
{
    for (i32 i = 0; i < users->max; ++i)
    {
        User* user = &users->items[i];
        if (user->state == USER_STATE_FREE) { continue; }
        if (user->ids.publicId == publicId) { return user; }
    }
    return NULL;
}

internal User* User_FindByAddress(UserList* users, ZNetAddress* addr)
{
    for (i32 i = 0; i < users->max; ++i)
    {
        User* user = &users->items[i];
        if (user->state == USER_STATE_FREE) { continue; }
        if (COM_CompareMemory(
            (u8*)&user->address,
            (u8*)addr,
            sizeof(ZNetAddress)
            ))
        {
            return user;
        }
    }
    return NULL;
}
