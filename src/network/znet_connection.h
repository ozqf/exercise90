#pragma once

#include "znet_module.cpp"

internal inline ZNetConnectionInfo ZNet_CreateConnInfo(ZNetConnection* conn)
{
    ZNetConnectionInfo info;
    info.address = conn->remoteAddress;
    info.id = conn->id;
    info.publicId = conn->publicId;
    return info;
}

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
            conn->sequence = 1;
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

ZNetConnectionInfo* ZNet_CreateLocalClient()
{
    return NULL;
}

internal void ZNet_RecordPacketTransmission(ZNetConnection* conn, u32 sequence)
{
    u32 index = sequence % ZNET_AWAITING_ACK_CAPACITY;
    //printf("Ack sequence: %d. index: %d\n", sequence, index);
    conn->awaitingAck[index].sequence = sequence;
    conn->awaitingAck[index].acked = 0;
}

internal void ZNet_CheckAcks(ZNetConnection* conn, u32 ack, u32 ackBits)
{
    //printf("Checking ack %u and ackBits %u\n", ack, ackBits);
    u32 index = ack % ZNET_AWAITING_ACK_CAPACITY;
    if (conn->awaitingAck[index].sequence == ack
        && conn->awaitingAck[index].acked == 0)
    {
        conn->awaitingAck[index].acked = 1;
        // boardcast ack received!
        //printf("  %d acked!\n", ack);
        ZNetConnectionInfo info = ZNet_CreateConnInfo(conn);
        g_output.DeliveryConfirmed(&info, ack);
    }
    // Run through awaiting acks. If ack is on matching sequence, check ack'd is true
    u32 bit = 0;
    ack -= 1;
    while (bit < 32)
    {
        if (ackBits & (1 << bit))
        {
            index = ack % ZNET_RECEIVED_CAPACITY;
            //printf("Checking bit %d at index %d\n", bit, index);
            //printf("  seq %u acked %d",
            //    conn->awaitingAck[index].sequence, conn->awaitingAck[index].acked);
            if (conn->awaitingAck[index].sequence == ack
                && conn->awaitingAck[index].acked == 0)
            {
                conn->awaitingAck[index].acked = 1;
                // boardcast ack received!
                //printf("  %d acked!\n", ack);
                ZNetConnectionInfo info = ZNet_CreateConnInfo(conn);
                g_output.DeliveryConfirmed(&info, ack);
            }
        }
        ack--;
        bit++;

    }
}

internal void ZNet_RecordPacketForAck(ZNetConnection* conn, u32 remoteSequence)
{
    u32 index = remoteSequence % ZNET_RECEIVED_CAPACITY;
    conn->received[index] = remoteSequence;
}

internal u32 ZNet_BuildAckBits(ZNetConnection* conn, u32 remoteSequence)
{
    #if 1
    u32 ackBits = 0;
    i32 bit = 0;
    u32 expectedSequence = remoteSequence - 1;
    while (bit < 32)
    {
        i32 index = expectedSequence % ZNET_RECEIVED_CAPACITY;
        
        if (conn->received[index] == expectedSequence
            && expectedSequence != 0)
        {
            ackBits |= (1 << bit);
            //printf("Sequence %u Index: %d bit %d\n", expectedSequence, index, bit);
        }
        bit++;
        expectedSequence--;
    }
    return ackBits;
    #endif
}

internal void ZNet_PrintAwaitingAcks(ZNetConnection* conn)
{
    for (i32 i = 0; i < ZNET_AWAITING_ACK_CAPACITY; ++i)
    {
        printf("%u, ", conn->awaitingAck[i].sequence);
    }
    #if 0
    i32 count = ZNET_AWAITING_ACK_CAPACITY;
    i32 sequence = conn->remoteSequence;
    do
    {
        i32 index = sequence % ZNET_AWAITING_ACK_CAPACITY;
        if (!conn->awaitingAck[index].acked)
        {
            printf("%d, ", conn->awaitingAck[index].sequence);
        }
        
        --sequence;
        --count;
    } while (count);
    #endif
}

internal void ZNet_PrintReceived(ZNetConnection* conn)
{
    for (i32 i = 0; i < ZNET_RECEIVED_CAPACITY; ++i)
    {
        printf("%u, ", conn->received[i]);
    }
}

void DeleteMe()
{
    ILLEGAL_CODE_PATH
    ZNet_PrintAwaitingAcks(NULL);
}
