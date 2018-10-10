#pragma once

#include "znet_module.cpp"

internal ZNetConnection* ZNet_GetConnectionById(ZNet* net, i32 id)
{
    if (id == 0) { return NULL; }
    for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
    {
        if (net->connections[i].id == id) { return &net->connections[i]; }
    }
    return NULL;
}

internal ZNetConnection* ZNet_GetConnectionByAddress(ZNet* net, ZNetAddress* address)
{
    if (!address) { return NULL; }
    for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
    {
        ZNetAddress* connAddr = &net->connections[i].remoteAddress;
        if (COM_COMPARE(address, connAddr, sizeof(ZNetAddress)))
        {
            return &net->connections[i];
        }
    }
    return NULL;
}

internal void ZNet_CloseConnection(ZNet* net, ZNetConnection* conn)
{
    NET_ASSERT(net, "Close connection: Net is null\n");
    NET_ASSERT(conn, "Close conncetion: conn is null\n");
    conn->id = 0;
}

internal void ZNet_DisconnectPeer(ZNet* net, ZNetConnection* conn, char* msg)
{
    NET_ASSERT(net, "Disconnect peer: Net is null\n");
    NET_ASSERT(conn, "Disconnect peer: conn is null\n");
    ZNet_SendDisconnectCommand(net, conn, msg);
    ZNet_CloseConnection(net, conn);
}

internal i32 ZNet_ConnectionIdIsInUse(ZNet* net, i32 queryId)
{
    for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
    {
        ZNetConnection* conn = &net->connections[i];
        if (conn->id == queryId) { return 1; }
    }
    return 0;
}

internal ZNetConnection* ZNet_GetFreeConnection(ZNet* net)
{
    for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
    {
        ZNetConnection* conn = &net->connections[i];
        if (conn->id == 0)
        {
            *conn = {};
            u32 newId = 0;
            ZNetConnection* other = NULL;
            do
            {
                newId = ZNet_CreateSalt();
            } while (ZNet_ConnectionIdIsInUse(net, newId));
            
            conn->id = newId;
            conn->publicId = net->nextPublicClientId;
            net->nextPublicClientId++;
            printf("ZNet got free conn for public Id %d\n", conn->publicId);
            return conn;
        }
    }
    return NULL;
}

// TODO: check server connection response... is conn id xor used here or is a new one generated
// by the server...?
internal ZNetConnection* ZNet_CreateClientConnection(ZNetAddress address, u8 isLocal)
{
    ZNetConnection* conn = ZNet_GetFreeConnection(&g_net);
    NET_ASSERT(conn, "ZNet failed to find free connection\n");
    // if client is local, ignore challenge/response stuff
    if (isLocal)
    {
        conn->flags |= ZNET_CONNECTION_FLAG_LOCAL;
        printf("ZNET Creating local connection id %d\n", conn->id);
    }
    else
    {
        conn->remoteAddress = address;
        printf("ZNET Creating remote connection id %d\n", conn->id);
    }
    return conn;
}
