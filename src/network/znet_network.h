#pragma once

#include "znet_module.cpp"

//internal ZNetConnection g_connections[MAX_CONNECTIONS];
//internal i32 g_socketIndex = -1;
//internal i32 g_netMode = 0;

internal ZNetConnection* ZNet_GetConnectionById(ZNet* net, u32 id)
{
    if (id == 0) { return NULL; }
    for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
    {
        if (net->connections[i].id == id) { return &net->connections[i]; }
    }
    return NULL;
}

 void ZNet_CloseConnection(ZNet* net, u32 id)
{
    ZNetConnection* conn = ZNet_GetConnectionById(net, id);
    if (conn == NULL)
    {
        NET_ASSERT(0, "App Net cannot close a non-existant connection\n");
    }
    conn->id = 0;
}

 ZNetConnection* ZNet_GetFreeConnection(ZNet* net)
{
    for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
    {
        ZNetConnection*
         conn = &net->connections[i];
        if (conn->id == 0)
        {
            u32 newId = 0;
            ZNetConnection* other = NULL;
            // make sure id is unique ;)
            do
            {
                newId = (u32)(COM_STDRandf32() * UINT_MAX);
                other = ZNet_GetConnectionById(net, newId);
            } while (other);
            conn->id = newId;
            return conn;
        }
    }
    return NULL;
}

 u32 ZNet_CreateClientConnection(ZNetAddress address, u8 isLocal)
{
    ZNetConnection* conn = ZNet_GetFreeConnection(&g_net);
    NET_ASSERT(conn, "ZNet failed to find free connection\n");
    // if client is local, ignore challenge/response stuff
    if (isLocal)
    {
        conn->flags |= ZNET_CONNECTION_FLAG_LOCAL;
        printf("NET Creating local connection id %d\n", conn->id);
    }
    else
    {
        printf("NET Creating remote connection id %d\n", conn->id);
    }
    return conn->id;
}

//////////////////////////////////////////////////////
// External
//////////////////////////////////////////////////////
void ZNet_StartSession(u8 netMode, ZNetAddress* address)
{
    ZNet* net = &g_net;
    NET_ASSERT((net->netMode == 0), "Net starting but not shutdown!");
    switch (netMode)
    {
        case NETMODE_SINGLE_PLAYER:
        {
            printf("ZNet - single player, no socket\n");
        } break;

        case NETMODE_DEDICATED_SERVER:
        {
            net->socketIndex = g_netPlatform.OpenSocket(23232);
        } break;

        case NETMODE_CLIENT:
        {
            net->socketIndex = g_netPlatform.OpenSocket(23232);
        } break;

        default:
        {
            NET_ASSERT(0, "Unsupported netmode\n");
        }
    }
    net->netMode = netMode;
}

void ZNet_SetErrorHandler(ZNet_ErrorHandler handler)
{
    g_errorHandler = handler;
}

void ZNet_EndSession()
{

}

void ZNet_Tick()
{
    ZNet* net = &g_net;
    switch(net->netMode)
    {
        case NETMODE_CLIENT:
        {

        } break;
    }
}
