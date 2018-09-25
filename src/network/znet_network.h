#pragma once

#include "znet_module.cpp"

//////////////////////////////////////////////////////
// External
//////////////////////////////////////////////////////
void ZNet_StartSession(u8 netMode, ZNetAddress address)
{
    ZNet* net = &g_net;
    NET_ASSERT((net->state == 0), "Net session was not ended!");
    switch (netMode)
    {
        case NETMODE_SINGLE_PLAYER:
        {
            printf("ZNet - single player, no socket\n");
            net->state = ZNET_STATE_SERVER;
        } break;

        case NETMODE_DEDICATED_SERVER:
        {
            printf("ZNet - dedicated server\n");
            net->serverPort = address.port;
            net->socketIndex = g_netPlatform.OpenSocket(net->serverPort);
            net->state = ZNET_STATE_SERVER;
        } break;

        case NETMODE_CLIENT:
        {
            printf("ZNet - client\n");
            net->socketIndex = g_netPlatform.OpenSocket(address.port);
            ZNetConnection* conn = ZNet_GetFreeConnection(&g_net);
            net->state = ZNET_STATE_CONNECTING;
            conn->remoteAddress = address;
            g_net.client2ServerId = conn->id;

        } break;

        default:
        {
            NET_ASSERT(0, "Unsupported netmode\n");
        } break;
    }
}

void ZNet_EndSession()
{

}

internal void ZNet_Send(ZNetAddress* address, u8* bytes, i32 numBytes)
{
    // Platform read function:
    //i32  (*SendTo)
    //(i32 transmittingSocketIndex, char* address, u16 port, char* data, i32 dataSize);
    char asciAddress[32];
    sprintf_s(asciAddress, 32, "%d.%d.%d.%d",
        address->ip4Bytes[0],
        address->ip4Bytes[1],
        address->ip4Bytes[2],
        address->ip4Bytes[3]
    );
    printf("Sending %d bytes to %s\n", numBytes, asciAddress);
    g_netPlatform.SendTo(g_net.socketIndex, asciAddress, address->port, (char*)bytes, numBytes);
}

// Returns 0 if all is okay
internal i32 ZNet_CheckPacketForErrors(u8* bytes, i32 numBytes)
{
	u8* read = bytes;
	ZNetPacketHeader h = {};
	read += COM_COPY(read, &h.protocol, sizeof(i32));
	if (h.protocol != ZNET_PROTOCOL)
	{
		return 1;
	}
	return 0;
}

internal void ZNet_Read(ZNet* net)
{
    // Platform read function:
    // i32  (*Read)
    // (i32 socketIndex, ZNetAddress* sender,  MemoryBlock* dataPtr);
    ZNetAddress address = {};
    MemoryBlock mem = {};
    mem.ptrMemory = g_packetReadBuffer;
    mem.size = ZNET_PACKET_READ_SIZE;
	
    i32 bytesRead = g_netPlatform.Read(g_net.socketIndex, &address, &mem);
    
    if (bytesRead == 0)
    {
        printf("Nothing to read\n");
        return;
    }
	
	i32 packetError = ZNet_CheckPacketForErrors((u8*)mem.ptrMemory, mem.size);
	if (packetError)
	{
		switch (packetError)
		{
			case 1: printf("Packet Protocol mismatch\n"); break;
			default: printf("Unknown packet check failure code: %d\n", packetError); break;
		}
		return;
	}
	
	u8* read = (u8*)mem.ptrMemory;
	// advance past packet header
	read += sizeof(i32);
	
    switch(net->state)
    {
        case ZNET_STATE_SERVER:
        {
            // look for client messages
            i32 i;
            read += COM_COPY(read, &i, sizeof(i32));
            printf("Server read %d\n", i);
            switch (i)
            {
                case ZNET_MSG_TYPE_CONNECTION_REQUEST:
                {
                    // look for a client matching the given address
                    // > if not found, create
                    // > send challenge
                    ZNetConnection* conn = ZNet_GetConnectionByAddress(net, &address);
                    if (conn == NULL)
                    {

                    }
                } break;
            }
        } break;
        
        case ZNET_STATE_CONNECTED:
        {
            // look for challenge response
        } break;

        case ZNET_STATE_CONNECTING:
        {
            
        } break;

        case ZNET_STATE_RESPONDING:
        {
            // look for connection accepted
        } break;

        default:
        {
            printf("Unknown net state %d\n", net->state);
        } break;
    }
}

void ZNet_Tick()
{
    ZNet* net = &g_net;

    printf("Tick %d\n", net->tickCount);

    // input
    ZNet_Read(net);

    // output
    
    switch(net->state)
    {
        case ZNET_STATE_SERVER:
        {
            // transmit client messages ()
        } break;
        
        case ZNET_STATE_CONNECTED:
        {
            // Transmit server messages
        } break;

        case ZNET_STATE_CONNECTING:
        {
            // periodically send connection request to server
            ByteBuffer buf = Buf_FromBytes(g_dataWriteBuffer, ZNET_DATA_WRITE_SIZE);
			buf.ptrWrite += COM_WriteI32(ZNET_PROTOCOL, buf.ptrWrite);
            buf.ptrWrite += COM_WriteI32(ZNET_MSG_TYPE_CONNECTION_REQUEST, buf.ptrWrite);
            ZNetConnection* conn = ZNet_GetConnectionById(&g_net, g_net.client2ServerId);
            NET_ASSERT(conn, "Client 2 Server connection is null");
            ZNet_Send(&conn->remoteAddress, buf.ptrStart, buf.ptrWrite - buf.ptrStart);
        } break;

        case ZNET_STATE_RESPONDING:
        {
            // periodically send challenge response to server
        } break;

        default:
        {
            printf("Unknown net state %d\n", net->state);
        } break;
    }
    net->tickCount++;
}
