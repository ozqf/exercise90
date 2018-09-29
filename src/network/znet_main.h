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
            // Random salt to tell apart different connections made
            // on the same address and port
            conn->salt = ZNet_CreateSalt();
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
    ZNet* net = &g_net;
    NET_ASSERT((net->state != 0), "Net session shutdown but not running\n");
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
    printf("Sending %d bytes to %s:%d\n", numBytes, asciAddress, address->port);
    g_netPlatform.SendTo(g_net.socketIndex, asciAddress, address->port, (char*)bytes, numBytes);
}

internal void ZNet_ServerReadPacket(ZNet* net, ZNetPacket* packet)
{
    u8* read = packet->bytes;
    i32 i;
    read += COM_COPY(read, &i, sizeof(i));
    printf("Server read packet type %d\n", i);
    switch (i)
    {
        case ZNET_MSG_TYPE_CONNECTION_REQUEST:
        {
            i32 clientSalt;
            read += COM_COPY(read, &clientSalt, sizeof(clientSalt));
            printf("Client Salt %d\n", clientSalt);
            // look for a client matching the given address
            // > if not found, create
            // > send challenge
            ZNetPending* pending = ZNet_AddPendingConnection(net, &packet->address, clientSalt);
			
			ByteBuffer data = ZNet_GetDataWriteBuffer();
			data.ptrWrite += COM_WriteI32(clientSalt, data.ptrWrite);
			data.ptrWrite += COM_WriteI32(pending->challenge, data.ptrWrite);
			
			ByteBuffer output = ZNet_GetPacketWriteBuffer();
			ZNet_BuildPacket(&output, data.ptrStart, Buf_BytesWritten(&data), &pending->address);
            ZNet_Send(&pending->address, output.ptrStart, Buf_BytesWritten(&output));
        } break;

        case ZNET_MSG_TYPE_DATA:
        {
            ZNetConnection* conn = ZNet_GetConnectionByAddress(net, &packet->address);
        } break;
    }
}

internal void ZNet_ClientReadPacket(ZNet* net, ZNetPacket* packet)
{
    u8* read = packet->bytes;
    i32 i;
    read += COM_COPY(read, &i, sizeof(i));
	switch (net->state)
    {
        case ZNET_STATE_CONNECTING:
        {
            if (i == net->client2ServerId)
            {
                printf("CL: Received response\n");
            }
            else
            {
                printf("CL: Unknown connection response %d\n", i);
            }
        } break;

        default:
        {
            printf("CL No read mode for state %d\n", net->state);
        } break;
    }
}

internal void ZNet_ReadPacket(ZNet* net, ZNetPacket* packet)
{
    switch(net->state)
    {
        case ZNET_STATE_SERVER:
        {
            ZNet_ServerReadPacket(net, packet);
        } break;
        
        case ZNET_STATE_CONNECTED:
        {
            // look for challenge response
            ZNet_ClientReadPacket(net, packet);
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

internal void ZNet_ReadSocket(ZNet* net)
{
    // Platform read function:
    // i32  (*Read)
    // (i32 socketIndex, ZNetAddress* sender,  MemoryBlock* dataPtr);
    i32 bytesRead;
    i32 packetsRead = 0;
    for(;;)
    {
        ZNetAddress address = {};
        MemoryBlock mem = {};
        mem.ptrMemory = g_packetReadBuffer;
        mem.size = ZNET_PACKET_READ_SIZE;
	
        bytesRead = g_netPlatform.Read(g_net.socketIndex, &address, &mem);
        if (bytesRead == 0)
        {
            //printf("Read %d packets\n", packetsRead);
            return;
        }
        packetsRead++;

        ZNetPacket packet;
        // careful to send bytes read NOT full buffer size here you ninny.
	    i32 packetError = ZNet_ParsePacketHeader((u8*)mem.ptrMemory, bytesRead, &address, &packet);
	    if (packetError)
	    {
	    	switch (packetError)
	    	{
	    		case 1: printf("Packet Protocol mismatch\n"); break;
                case 2: printf("Packet data checksum mismatch\n"); break;
	    		default:
	    		printf("Unknown packet check failure code: %d\n", packetError);
	    		break;
	    	}
	    	return;
	    }
	    printf("Reading %d bytes from \"%d.%d.%d.%d:%d\"\n",
	    	bytesRead,
	    	address.ip4Bytes[0], address.ip4Bytes[1],
	    	address.ip4Bytes[2], address.ip4Bytes[3],
	    	address.port
	    );

        ZNet_ReadPacket(net, &packet);
    }
}

void ZNet_Tick()
{
    ZNet* net = &g_net;

    printf("***** Tick %d *****\n", net->tickCount);

    // input
    ZNet_ReadSocket(net);

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
            // Get conn for address and salt
            ZNetConnection* conn = ZNet_GetConnectionById(&g_net, g_net.client2ServerId);
            NET_ASSERT(conn, "Client 2 Server connection is null");

            // Prepare data buffer
            ByteBuffer data = Buf_FromBytes(g_dataWriteBuffer, ZNET_DATA_WRITE_SIZE);
            data.ptrWrite += COM_WriteI32(ZNET_MSG_TYPE_CONNECTION_REQUEST, data.ptrWrite);
            data.ptrWrite += COM_WriteI32(conn->salt, data.ptrWrite);
            i32 numBytes = data.ptrWrite - data.ptrStart;

            // g_packetWriteBuffer
            // ZNET_PACKET_WRITE_SIZE
            ByteBuffer packetBuffer = Buf_FromBytes(g_packetWriteBuffer, ZNET_PACKET_WRITE_SIZE);
            i32 packetSize = ZNet_BuildPacket(&packetBuffer, data.ptrStart, numBytes, &conn->remoteAddress);

            // send!
            ZNet_Send(&conn->remoteAddress, packetBuffer.ptrStart, packetSize);
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
