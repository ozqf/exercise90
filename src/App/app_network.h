#pragma once

#include "app_module.cpp"

/*
App Network layer functionality:

> Packet Read/Write
> App -> platform communication (do this via platform input buffer?)
> Handle connection persistence

> System will open one unreliable UDP socket
> connections are managed via randomly assigned ids.
> connections store the 

*/

#define APPNET_DEFAULT_PORT 23232
#define APPNET_CONNECTION_FLAG_LOCAL (1 << 0)

// id of 0 == unassigned.
struct ZNetConnection
{
    u32 id;
    u32 sequence;
    ZNetAddress address;
    u32 flags;
};

#define MAX_CONNECTIONS 16
internal ZNetConnection g_connections[MAX_CONNECTIONS];
internal i32 g_socketIndex = -1;
internal i32 g_netMode = 0;

ZNetConnection* AppNet_GetConnectionById(u32 id)
{
    if (id == 0) { return NULL; }
    for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
    {
        if (g_connections[i].id == id) { return &g_connections[i]; }
    }
    return NULL;
}

void AppNet_CloseConnection(u32 id)
{
    ZNetConnection* conn = AppNet_GetConnectionById(id);
    if (conn == NULL)
    {
        APP_ASSERT(0, "App Net cannot close a non-existant connection\n");
    }
    conn->id = 0;
}

ZNetConnection* AppNet_GetFreeConnection()
{
    for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
    {
        ZNetConnection*
         conn = &g_connections[i];
        if (conn->id == 0)
        {
            u32 newId = 0;
            ZNetConnection* other = NULL;
            // make sure id is unique ;)
            do
            {
                newId = (u32)(COM_STDRandf32() * UINT_MAX);
                other = AppNet_GetConnectionById(newId);
            } while (other);
            conn->id = newId;
            return conn;
        }
    }
    return NULL;
}

u32 AppNet_CreateClientConnection(ZNetAddress address, u8 isLocal)
{
    ZNetConnection* conn = AppNet_GetFreeConnection();
    APP_ASSERT(conn, "AppNet failed to find free connection\n");
    if (isLocal)
    {
        conn->flags |= APPNET_CONNECTION_FLAG_LOCAL;
        printf("NET Creating local connection id %d\n", conn->id);
    }
    else
    {
        printf("NET Creating remote connection id %d\n", conn->id);
    }
    return conn->id;
}

void AppNet_Start(u8 netMode)
{
    APP_ASSERT((g_netMode == 0), "Net starting but not shutdown!");
    switch (netMode)
    {
        case NETMODE_SINGLE_PLAYER:
        {
            printf("AppNet - single player, no socket\n");
        } break;

        case NETMODE_DEDICATED_SERVER:
        {
            g_socketIndex = platform.Net_OpenSocket(23232);
        } break;

        default:
        {
            APP_ASSERT(0, "Unsupported netmode\n");
        }
    }
    g_netMode = netMode;
}

void AppNet_Shutdown()
{
    g_netMode = 0;
    if (g_socketIndex != -1)
    {
        platform.Net_CloseSocket(g_socketIndex);
        g_socketIndex = -1;
    }
}
