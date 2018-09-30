#pragma once


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
        ZNetConnection* conn = &net->connections[i];
        if (conn->id == 0)
        {
            u32 newId = 0;
            ZNetConnection* other = NULL;
            newId = ZNet_CreateSalt();
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
