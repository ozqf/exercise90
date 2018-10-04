#pragma once

#include "znet_module.cpp"

//////////////////////////////////////////////////////
// External
//////////////////////////////////////////////////////

i32 ZNet_IsServer()
{
    return (g_net.state == ZNET_STATE_SERVER);
}

void ZNet_StartSession(u8 netMode, ZNetAddress* address, u16 selfPort)
{
    // TODO: Make sure rand is somehow mixed up by this point
    // or server and client random numbers can exactly align!
    // Currently done by seeding it from the address of argv, but bleh.
    ZNet* net = &g_net;
    NET_ASSERT((net->state == 0), "Net session was not ended!");
    net->isListening = 0;
	u16 sockedOpened = 1;
    switch (netMode)
    {
        case NETMODE_SINGLE_PLAYER:
        {
            printf("ZNet - single player, no socket\n");
            net->isListening = 1;
            net->state = ZNET_STATE_SERVER;
        } break;

        case NETMODE_DEDICATED_SERVER:
        {
            printf("ZNet - dedicated server on port %d\n", selfPort);
            net->selfPort = selfPort;
            net->socketIndex = g_netPlatform.OpenSocket(net->selfPort, &sockedOpened);
			printf("Dedicated server requested port %d opened port %d\n", selfPort, sockedOpened);
            net->isListening = 1;
            net->state = ZNET_STATE_SERVER;
        } break;

        case NETMODE_CLIENT:
        {
            printf("ZNet - client\n");
            
            net->socketIndex = g_netPlatform.OpenSocket(0, &sockedOpened);
			net->selfPort = sockedOpened;
			
            ZNetConnection* conn = ZNet_GetFreeConnection(&g_net);
            net->state = ZNET_STATE_CONNECTING;
            conn->type = ZNET_CONN_TYPE_CLIENT2SERVER;
            conn->remoteAddress = *address;
            g_net.client2ServerId = conn->id;
            printf("CL on port %d connecting to \"%d.%d.%d.%d:%d\"\n",
                selfPort,
                conn->remoteAddress.ip4Bytes[0],
                conn->remoteAddress.ip4Bytes[1],
                conn->remoteAddress.ip4Bytes[2],
                conn->remoteAddress.ip4Bytes[3],
                conn->remoteAddress.port
            );
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
    //printf("Sending %d bytes to %s:%d\n", numBytes, asciAddress, address->port);
    g_netPlatform.SendTo(g_net.socketIndex, asciAddress, address->port, (char*)bytes, numBytes);
}

internal void ZNet_ReadPacket(ZNet* net, ZNetPacket* packet)
{
    u8* read = packet->bytes;
    u8* end = packet->bytes + packet->numBytes;
    u8 msgType;
    msgType = COM_ReadByte(&read);
    //printf(">> Read packet type %d from remote port %d\n", msgType, packet->address.port);
    switch (msgType)
    {
        case ZNET_MSG_TYPE_DATA:
        {
            ZNetConnection* conn = ZNet_GetConnectionByAddress(net, &packet->address);
            NET_ASSERT(conn, "No connection found for data packet\n");
            ZNetPacketInfo info = {};
            info.sender.address = conn->remoteAddress;
            info.sender.id = conn->id;
            info.remoteSequence = COM_ReadI32(&read);

            // TODO: Nicer way to calculate remaining bytes:
            u16 dataSize = (u16)(end - read);
            g_output.DataPacketReceived(&info, read, dataSize);
        } break;

        case ZNET_MSG_TYPE_CONNECTION_REQUEST:
        {
            if (!net->isListening) { return; }

            i32 clientSalt;
            read += COM_COPY(read, &clientSalt, sizeof(clientSalt));
            //printf("Client Salt %d\n", clientSalt);
            // look for a client matching the given address
            // > if not found, create
            // > send challenge
            ZNetPending* pending = ZNet_AddPendingConnection(net, &packet->address, clientSalt);
			
			ByteBuffer data = ZNet_GetDataWriteBuffer();
            data.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_CHALLENGE, data.ptrWrite);
			data.ptrWrite += COM_WriteI32(clientSalt, data.ptrWrite);
			data.ptrWrite += COM_WriteI32(pending->challenge, data.ptrWrite);
			
			ByteBuffer output = ZNet_GetPacketWriteBuffer();
			ZNet_BuildPacket(&output, data.ptrStart, Buf_BytesWritten(&data), &pending->address);
            ZNet_Send(&pending->address, output.ptrStart, Buf_BytesWritten(&output));

            printf("SV Challenging client %d with %d\n", clientSalt, pending->challenge);
        } break;

        case ZNET_MSG_TYPE_CHALLENGE:
        {
            if (net->state != ZNET_STATE_CONNECTING) { return; }
            net->state = ZNET_STATE_RESPONDING;
            i32 clientSalt = COM_ReadI32(&read);
            ZNetConnection* conn = ZNet_GetConnectionById(net, net->client2ServerId);
            if (!conn)
            {
                printf("CL No conn for client2Server id %d\n", net->client2ServerId);
                return;
            }
            if (conn->id != clientSalt)
            {
                printf("CL Client salt mismatch got %d expected %d\n", clientSalt, conn->id);
            }

            i32 challenge = COM_ReadI32(&read);
            printf("CL: Challenged: %d\n", challenge);
            
            // the conn->id is actually (clientSalt ^ challenge)
            // the conn id must therefore be changed to that here or 
            // the client will loose it's own connection
            i32 response = (clientSalt ^ challenge);
            conn->id = response;
			net->client2ServerId = response;

            ByteBuffer b = ZNet_GetDataWriteBuffer();
            i32 numBytes = ZNet_WriteChallengeResponse(&b, response);
            ByteBuffer p = ZNet_GetPacketWriteBuffer();
            ZNet_BuildPacket(&p, b.ptrStart, b.Written(), &conn->remoteAddress);
            ZNet_Send(&conn->remoteAddress, p.ptrStart, p.Written());
        } break;

        case ZNET_MSG_TYPE_CHALLENGE_RESPONSE:
        {
            if (!net->isListening) { return; }

            i32 response = COM_ReadI32(&read);
            
            ZNetPending* p = ZNet_FindPendingConnection(net, response);
            if (!p)
            {
                printf("SV Found no pending connection for %d\n", response);
                return;
            }
            ZNetConnection* conn = ZNet_CreateClientConnection(p->address, 0);
            conn->id = response;
            ZNet_ClosePendingConnection(net, p);
            
            ZNet_SendConnectionApproved(net, conn);
			
			ZNetConnectionInfo info = {};
			info.address = conn->remoteAddress;
			info.id = conn->id;
			g_output.ConnectionAccepted(&info);
			
            printf("SV Response received: %d Accepted connection\n", response);
        } break;

        case ZNET_MSG_TYPE_CONNECTION_APPROVED:
        {
            if (net->state != ZNET_STATE_RESPONDING) { return; }

			i32 response = 
			// TODO: Not validating XOR!
			
            printf("CL Conn approval received!\n");
            net->state = ZNET_STATE_CONNECTED;
			
			ZNetConnection* conn = ZNet_GetConnectionById(net, net->client2ServerId);
			ZNetConnectionInfo info = {};
			info.address = conn->remoteAddress;
			info.id = conn->id;
			g_output.ConnectionAccepted(&info);
        } break;

        case ZNET_MSG_TYPE_CONNECTION_DENIED:
        {
            i32 xor = COM_ReadI32(&read);
            ZNetConnection* conn = ZNet_GetConnectionById(net, xor);
            if (!conn)
            {
                printf("No conn with xor %d!\n", xor);
                return;
            }
            // TODO: Currently not copying this string to a safe buffer but trusting
            // to find a zero terminator!
            i32 numChars = COM_ReadI32(&read);
            char* msg = (char*)read;
            printf("Connection denied, msg: %s\n", msg);
            ZNet_CloseConnection(net, conn);
            if (net->state != ZNET_STATE_SERVER)
            {
                net->state = ZNET_STATE_DISCONNECTED;
                return;
            }
        } break;

        #if 1
        case ZNET_MSG_TYPE_KEEP_ALIVE:
        {
            i32 xor = COM_ReadI32(&read);
            ZNetConnection* conn = ZNet_GetConnectionById(net, xor);
            if (conn)
            {
                conn->ticksSinceLastMessage = 0;
                printf("NET type %d received Keep alive from %d\n", net->state, xor);
            }
        } break;

        case ZNET_MSG_TYPE_KEEP_ALIVE_RESPONSE:
        {
            i32 xor = COM_ReadI32(&read);
            ZNetConnection* conn = ZNet_GetConnectionById(net, xor);
        } break;
        #endif

        default:
        {
            ZNetAddress* addr = &packet->address;
            ZNetConnection* conn = ZNet_GetConnectionByAddress(net, &packet->address);
            printf("Unknown msg type %d from \"%d.%d.%d.%d:%d\"\n",
                msgType,
                addr->ip4Bytes[0], addr->ip4Bytes[1], addr->ip4Bytes[2], addr->ip4Bytes[3],
                addr->port
            );
        } break;
    }
}

internal void ZNet_ReadSocket(ZNet* net)
{
    // Platform read function:
    // i32  (*Read)
    // (i32 socketIndex, ZNetAddress* sender,  MemoryBlock* dataPtr);
    u16 bytesRead;
    i32 packetsRead = 0;
    for(;;)
    {
        ZNetAddress address = {};
        MemoryBlock mem = {};
        mem.ptrMemory = g_packetReadBuffer;
        mem.size = ZNET_PACKET_READ_SIZE;
	
        bytesRead = (u16)g_netPlatform.Read(g_net.socketIndex, &address, &mem);
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
		#if 0
	    printf("Reading %d bytes from \"%d.%d.%d.%d:%d\"\n",
	    	bytesRead,
	    	address.ip4Bytes[0], address.ip4Bytes[1],
	    	address.ip4Bytes[2], address.ip4Bytes[3],
	    	address.port
	    );
		#endif
		
        ZNet_ReadPacket(net, &packet);
    }
}

// returns 0 if all is well
i32 ZNet_Tick(f32 deltaTime)
{
    ZNet* net = &g_net;
	
	//system("cls");
    if (net->state == ZNET_STATE_SERVER)
    {
        printf("\n***** Server Tick %d *****\n", net->tickCount);
    }
    else
    {
        printf("\n***** Client Tick %d *****\n", net->tickCount);
    }
    

    // input
    ZNet_ReadSocket(net);

    // output
    
    switch(net->state)
    {
        case ZNET_STATE_SERVER:
        {
            // transmit client messages ()
            for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
            {
                ZNetConnection* conn = &net->connections[i];
                if (conn->id == 0) { continue; }

                conn->keepAliveSendTicks++;
				conn->ticksSinceLastMessage++;
                if (conn->ticksSinceLastMessage > ZNET_CONNECTION_TIMEOUT_TICKS)
                {
                    printf("  Conn to port %d lost\n", conn->remoteAddress.port);
                    ZNet_DisconnectPeer(net, conn, "Connection lost");
                    continue;
                }

                if (conn->keepAliveSendTicks > 8)
                {
                    printf("  Disconnecting conn to port %d\n", conn->remoteAddress.port);
                    ZNet_DisconnectPeer(net, conn, "Kicked");
                    continue;
                }

                ZNet_SendKeepAlive(net, conn);
            }
        } break;
        
        case ZNET_STATE_CONNECTED:
        {
            // Transmit server messages
            for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
            {
                ZNetConnection* conn = &net->connections[i];
                if (conn->id == 0) { continue; }

                ZNet_SendKeepAlive(net, conn);
            }
        } break;

        case ZNET_STATE_CONNECTING:
        {
            // Get conn for address and salt
            ZNetConnection* conn = ZNet_GetConnectionById(&g_net, g_net.client2ServerId);
            NET_ASSERT(conn, "Client 2 Server connection is null");
            printf("CL SENDING CONN REQUEST\n");
            #if 0
            // Prepare data buffer
            ByteBuffer data = Buf_FromBytes(g_dataWriteBuffer, ZNET_DATA_WRITE_SIZE);
            //data.ptrWrite += COM_WriteI32(ZNET_MSG_TYPE_CONNECTION_REQUEST, data.ptrWrite);
            data.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_CONNECTION_REQUEST, data.ptrWrite);
            data.ptrWrite += COM_WriteI32(conn->id, data.ptrWrite);
            printf("CL Sending type %d\n", ZNET_MSG_TYPE_CONNECTION_REQUEST);
            i32 numBytes = data.ptrWrite - data.ptrStart;
            #endif

            // Prepare data buffer
            ByteBuffer data = ZNet_GetDataWriteBuffer();
            ZNet_WriteConnRequest(&data, conn->id);
            i32 numBytes = data.Written();

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

        case ZNET_STATE_DISCONNECTED:
        {
            return 1;
        } break;

        default:
        {
            printf("Unknown net state %d\n", net->state);
        } break;
    }
    net->tickCount++;
    if (net->tickCount > 30)
    {
        return 2;
    }
    return 0;
}
