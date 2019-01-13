#pragma once

#include "znet_module.cpp"

//////////////////////////////////////////////////////
// External
//////////////////////////////////////////////////////

i32 ZNet_IsServer(ZNetHandle* handle)
{
    ZNET_HANDLE_2_NET(handle)
    return (net->state == ZNET_STATE_SERVER);
}

void ZNet_EnableQualitySimulation(i32 minMilliSeconds, i32 maxMilliSeconds, f32 packetLossPercentage)
{
    
}

i32 ZNet_StartSession(ZNetHandle* handle, u8 netMode, ZNetAddress* address, u16 selfPort)
{
    ZNET_HANDLE_2_NET(handle)
    // TODO: Make sure rand is somehow mixed up by this point
    // or server and client random numbers can exactly align!
    // Currently done by seeding it from the address of argv, but bleh.
    NET_ASSERT(net, (net->state == 0), "Cannot start a new network session until current is shutdown!");
    net->isListening = 0;
	u16 sockedOpened = 1;
    i32 result = COM_ERROR_NONE;
    switch (netMode)
    {
        case NETMODE_SINGLE_PLAYER:
        {
            printf("ZNet - SINGLE PLAYER, no socket\n");
            net->isListening = 1;
            net->socketIndex = net->platform.OpenSocket(ZNET_LOOPBACK_PORT, &sockedOpened);
            if (net->socketIndex < 0)
            {
                result = COM_ERROR_OPEN_SOCKET_FAILED;
                break;
            }
            net->state = ZNET_STATE_SERVER;
        } break;

        case NETMODE_DEDICATED_SERVER:
        {
            printf("ZNet - DEDICATED SERVER on port %d\n", selfPort);
            net->selfPort = selfPort;
            net->socketIndex = net->platform.OpenSocket(net->selfPort, &sockedOpened);
            if (net->socketIndex < 0)
            {
                result = COM_ERROR_OPEN_SOCKET_FAILED;
                break;
            }
			printf("Dedicated server requested port %d opened port %d\n", selfPort, sockedOpened);
            net->isListening = 1;
            net->state = ZNET_STATE_SERVER;
        } break;

        case NETMODE_LISTEN_SERVER:
        {
            printf("ZNet - LISTEN SERVER on port %d\n", selfPort);
            net->selfPort = selfPort;
            net->socketIndex = net->platform.OpenSocket(net->selfPort, &sockedOpened);
            printf("ZNet socket index: %d\n", net->socketIndex);
            if (net->socketIndex < 0)
            {
                result = COM_ERROR_OPEN_SOCKET_FAILED;
                printf("ZNET start socket failed with %d\n", result);
                break;
            }
			printf("  Listen server requested port %d opened port %d\n", selfPort, sockedOpened);
            net->isListening = 1;
            net->state = ZNET_STATE_SERVER;
        } break;

        case NETMODE_CLIENT:
        {
            printf("ZNet - CLIENT\n");
            
            net->socketIndex = net->platform.OpenSocket(0, &sockedOpened);
            printf("ZNet socket index: %d\n", net->socketIndex);
            if (net->socketIndex < 0)
            {
                result = COM_ERROR_OPEN_SOCKET_FAILED;
                break;
            }
			net->selfPort = sockedOpened;
            
            ZNetConnection* conn = ZNet_GetFreeConnection(net);
            net->state = ZNET_STATE_CONNECTING;
            conn->type = ZNET_CONN_TYPE_CLIENT2SERVER;
            conn->remoteAddress = *address;
            net->client2ServerId = conn->id;
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
            //NET_ASSERT(0, "Unsupported netmode\n");
            result = COM_ERROR_UNSUPPORTED_OPTION;
        } break;
    }
    printf("ZNET Start session result %d\n", result);
    return result;
}

void ZNet_EndSession(ZNetHandle* handle)
{
    ZNET_HANDLE_2_NET(handle)
    printf("Net session shutdown but not running\n");
    net->state = NETMODE_NONE;
    // TODO: Some actual shutting down would help
}

internal void ZNet_SendActual(ZNet* net, ZNetAddress* address, u8* bytes, i32 numBytes)
{
    char asciAddress[32];
    sprintf_s(asciAddress, 32, "%d.%d.%d.%d",
        address->ip4Bytes[0],
        address->ip4Bytes[1],
        address->ip4Bytes[2],
        address->ip4Bytes[3]
    );
    //printf("Sending %d bytes to %s:%d\n", numBytes, asciAddress, address->port);
    //printf("SEND: ");
    //COM_PrintBytes(bytes, (u16)numBytes, 16);
    //printf(">");
    net->platform.SendTo(net->socketIndex, asciAddress, address->port, (char*)bytes, numBytes);
}

internal void ZNet_Send(ZNet* net, ZNetAddress* address, u8* bytes, i32 numBytes)
{
    net->store.SendPacket(net, address, bytes, (u16)numBytes);
}

/**
 * Important: Must make sure that the reader is in the correct state for the given message type
 */
internal void ZNet_ReadPacket(ZNet* net, ZNetPacket* packet)
{
    u8* read = packet->bytes;
    u8* end = packet->bytes + packet->numBytes;
    u8 msgType;
    msgType = COM_ReadByte(&read);
    ZNetConnection* conn = NULL;
    //printf(">> Read packet type %d from remote port %d\n", msgType, packet->address.port);
    switch (msgType)
    {
        case ZNET_MSG_TYPE_DATA:
        {
            //printf("<");
			if (net->state == ZNET_STATE_SERVER || net->state == ZNET_STATE_CONNECTED)
			{
                // read payload header and local connection Id
                i32 xor = COM_ReadI32(&read);

                //ZNetConnection* conn = ZNet_GetConnectionByAddress(net, &packet->address);
				conn = ZNet_GetConnectionById(net, xor);
            
                // May occur in the packet after a client is disconnected...
                // TODO: Cleaner way to handle this situation?
                if (conn == NULL)
                {
                    printf("ZNET: No connection found for data packet\n");
                    return;
                }
				
                ZNetPacketInfo info = {};
				info.sender.address = conn->remoteAddress;
                
                if (xor != conn->id)
                {
                    printf("ZNET data packet connId mismatch: got %d expected %d\n",
                        xor, conn->id);
                    return;
                }
                info.sender.id = xor;
                
                // sequence
                info.remoteSequence = COM_ReadU32(&read);
                
                if (info.remoteSequence > conn->remoteSequence)
                {
                    conn->remoteSequence = info.remoteSequence;
                }
                u32 ack = COM_ReadU32(&read);
                u32 ackBits = COM_ReadU32(&read);
                //printf("ZNET Received seq %u. Ack %u, ackBits %u\n",
                //    info.remoteSequence, ack, ackBits);
                ZNet_CheckAcks(net, conn, ack, ackBits);
                ZNet_RecordPacketForAck(conn, info.remoteSequence);
				// TODO: Nicer way to calculate remaining bytes:
				u16 dataSize = (u16)(end - read);
				net->output.DataPacketReceived(&info, read, dataSize);
			}
			else
			{
				printf("ZNET Received data packet but not connected!\n");
			}
            
        } break;

        case ZNET_MSG_TYPE_CONNECTION_REQUEST:
        {
            printf("^");
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
			ZNet_BuildPacket(&output, data.ptrStart, Buf_BytesWritten(&data), &pending->address, 0);
            ZNet_Send(net, &pending->address, output.ptrStart, Buf_BytesWritten(&output));

            printf("ZNET SV Challenging client %d with %d\n", clientSalt, pending->challenge);
        } break;

        case ZNET_MSG_TYPE_CHALLENGE:
        {
            printf("$");
            if (net->state != ZNET_STATE_CONNECTING) { return; }

            net->state = ZNET_STATE_RESPONDING;
            i32 clientSalt = COM_ReadI32(&read);
            conn = ZNet_GetConnectionById(net, net->client2ServerId);
            if (!conn)
            {
                printf("ZNET CL No conn for client2Server id %d\n", net->client2ServerId);
                return;
            }
            if (conn->id != clientSalt)
            {
                printf("ZNET CL Client salt mismatch got %d expected %d\n", clientSalt, conn->id);
                return;
            }

            i32 challenge = COM_ReadI32(&read);
            printf("ZNET CL: Challenged: %d\n", challenge);
            
            // the conn->id is actually (clientSalt ^ challenge)
            // the conn id must therefore be changed to that here or 
            // the client will loose it's own connection
            i32 response = (clientSalt ^ challenge);
            conn->id = response;
			net->client2ServerId = response;
        } break;

        case ZNET_MSG_TYPE_CHALLENGE_RESPONSE:
        {
            printf("^");
            if (!net->isListening) { return; }

            i32 response = COM_ReadI32(&read);
			
			conn = ZNet_GetConnectionById(net, response);
			if (!conn)
			{
                // close pending connection
                ZNetPending* p = ZNet_FindPendingConnection(net, response);
                if (!p)
                {
                    printf("ZNET SV Found no pending connection for %d\n", response);
                    return;
                }
                conn = ZNet_CreateClientConnection(net, p->address, 0);
                // TODO: Is id response assigned here or a new one generated
                // when a free connection is assigned...?
                conn->id = response;
                ZNet_ClosePendingConnection(net, p);

                ZNet_SendConnectionApproved(net, conn);
			
			    ZNetConnectionInfo info = {};
			    info.address = conn->remoteAddress;
			    info.id = conn->id;
			    net->output.ConnectionAccepted(&info);
			}
            else
            {
                // resend join confirmation
                ZNet_SendConnectionApproved(net, conn);
            }
            
            printf("ZNET SV Response received: %d Accepted connection\n", response);
        } break;

        case ZNET_MSG_TYPE_CONNECTION_APPROVED:
        {
            printf("^");
            if (net->state != ZNET_STATE_RESPONDING) { return; }

			//i32 response = 
			// TODO: Not validating XOR!
			
            printf("ZNET CL Conn approval received!\n");
            net->state = ZNET_STATE_CONNECTED;
			
			conn = ZNet_GetConnectionById(net, net->client2ServerId);
			ZNetConnectionInfo info = {};
			info.address = conn->remoteAddress;
			info.id = conn->id;
			net->output.ConnectionAccepted(&info);
        } break;

        case ZNET_MSG_TYPE_CONNECTION_DENIED:
        {
            printf("^");
            i32 xor = COM_ReadI32(&read);
            conn = ZNet_GetConnectionById(net, xor);
            if (!conn)
            {
                printf("ZNET No conn with xor %d!\n", xor);
                return;
            }
            ZNetConnectionInfo info = {};
            info.address = conn->remoteAddress;
            info.id = conn->id;
            // TODO: Currently not copying this string to a safe buffer but trusting
            // to find a zero terminator!
            i32 numChars = COM_ReadI32(&read);
            char* msg = (char*)read;
            printf("ZNET Connection denied, msg: %s\n", msg);
            ZNet_CloseConnection(net, conn);
            if (net->state != ZNET_STATE_SERVER)
            {
                net->state = ZNET_STATE_DISCONNECTED;
                // TODO: See if this ever happens...
                if (conn->id != net->client2ServerId)
                {
                    printf("ZNET ERROR: Unknown source for client disconnection\n");
                }
                net->state = ZNET_STATE_DISCONNECTED;
                net->output.ConnectionDropped(&info);
                return;
            }
        } break;

        #if 1
        case ZNET_MSG_TYPE_KEEP_ALIVE:
        {
            printf("^");
            i32 xor = COM_ReadI32(&read);
            conn = ZNet_GetConnectionById(net, xor);
            if (conn)
            {
                conn->ticksSinceLastMessage = 0;
                //printf("NET type %d received Keep alive from %d\n", net->state, xor);
            }
            else
            {
                printf("ZNET No conn %d found for keep alive\n", xor);
            }
        } break;

        case ZNET_MSG_TYPE_KEEP_ALIVE_RESPONSE:
        {
            printf("^");
            i32 xor = COM_ReadI32(&read);
            conn = ZNet_GetConnectionById(net, xor);
        } break;
        #endif

        default:
        {
            printf("ZNET Unknown msg type %d\n", msgType);
            #if 0
            ZNetAddress* addr = &packet->address;
            conn = ZNet_GetConnectionByAddress(net, &packet->address);
            printf("ZNET Unknown msg type %d from \"%d.%d.%d.%d:%d\"\n",
                msgType,
                addr->ip4Bytes[0], addr->ip4Bytes[1], addr->ip4Bytes[2], addr->ip4Bytes[3],
                addr->port
            );
            #endif
        } break;
    }

    if (conn != NULL)
    {
        conn->timeSinceLastMessage = 0;
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
        mem.size = ZNET_MAX_UDP_PACKET_SIZE;
	
        bytesRead = (u16)net->platform.Read(net->socketIndex, &address, &mem);
        if (bytesRead == 0)
        {
            return;
        }
        packetsRead++;

        ZNetPacket packet;
        //printf("RECV: ");
        //COM_PrintBytes((u8*)mem.ptrMemory, bytesRead, 16);
        // careful to send bytes read NOT full buffer size here you ninny.
	    i32 packetError = ZNet_ParsePacketHeader((u8*)mem.ptrMemory, bytesRead, &address, &packet);
	    if (packetError)
	    {
	    	switch (packetError)
	    	{
	    		case 1: printf("ZNET Packet Protocol mismatch\n"); break;
                case 2: printf("ZNET Packet data checksum mismatch\n"); break;
	    		default:
	    		printf("ZNET Unknown packet check failure code: %d\n", packetError);
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
i32 ZNet_Tick(ZNetHandle* handle, f32 deltaTime)
{
    ZNET_HANDLE_2_NET(handle);
    printf("|");
	
	//system("cls");
    /*
    if (net->state == ZNET_STATE_SERVER)
    {
        //printf("\n***** Server Tick %d *****\n", net->tickCount);
        printf(".");
    }
    */
    // else
    // {
    //     printf("\n***** Client Tick %d *****\n", net->tickCount);
    // }
    net->store.Tick(net, deltaTime);

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
                if(conn->flags & ZNET_CONNECTION_FLAG_LOCAL) { continue; }

                conn->keepAliveSendTicks++;
				conn->timeSinceLastMessage += deltaTime;
                if (conn->timeSinceLastMessage > ZNET_CONNECTION_TIMEOUT_SECONDS)
                {
                    printf("  Conn to port %d lost\n", conn->remoteAddress.port);
                    ZNet_DisconnectPeer(net, conn, "Connection lost");
                    continue;
                }

                //ZNet_SendKeepAlive(net, conn);
            }
        } break;
        
        case ZNET_STATE_CONNECTED:
        {
            // Transmit server messages
            for (i32 i = 0; i < MAX_CONNECTIONS; ++i)
            {
                ZNetConnection* conn = &net->connections[i];
                if (conn->id == 0) { continue; }

                // TODO: Timeout server connection

                //ZNet_SendKeepAlive(net, conn);
            }
        } break;

        case ZNET_STATE_CONNECTING:
        {
            // Get conn for address and salt
            ZNetConnection* conn =
                ZNet_GetConnectionById(net, net->client2ServerId);
            NET_ASSERT(net, conn, "Client 2 Server connection is null");
            //printf("CL SENDING CONN REQUEST\n");
            
            // Prepare data buffer
            ByteBuffer data = ZNet_GetDataWriteBuffer();
            ZNet_WriteConnRequest(&data, conn->id);
            i32 numBytes = data.Written();

            // g_packetWriteBuffer
            // ZNET_PACKET_WRITE_SIZE
            ByteBuffer packetBuffer = 
                Buf_FromBytes(g_packetWriteBuffer, ZNET_MAX_PAYLOAD_SIZE);
            i32 packetSize = ZNet_BuildPacket(
                &packetBuffer, data.ptrStart, numBytes, &conn->remoteAddress, 0);

            // send!
            ZNet_Send(net, &conn->remoteAddress, packetBuffer.ptrStart, packetSize);
        } break;

        case ZNET_STATE_RESPONDING:
        {
            // periodically send challenge response to server

            ZNetConnection* conn = ZNet_GetConnectionById(net, net->client2ServerId);
            if (conn == NULL)
            {
                printf("  In respond state but no connection!\n");
                net->state = ZNET_STATE_DISCONNECTED;
                break;
            }
            printf("$");
            ByteBuffer b = ZNet_GetDataWriteBuffer();
            i32 numBytes = ZNet_WriteChallengeResponse(&b, conn->id);
            ByteBuffer p = ZNet_GetPacketWriteBuffer();
            ZNet_BuildPacket(&p, b.ptrStart, b.Written(), &conn->remoteAddress, 0);
            ZNet_Send(net, &conn->remoteAddress, p.ptrStart, p.Written());
        } break;

        case ZNET_STATE_DISCONNECTED:
        {
            return 1;
        } break;

        default:
        {
            printf("ZNET Unknown net state %d\n", net->state);
        } break;
    }
    net->tickCount++;
    if (net->tickCount > 18 && net->state != ZNET_STATE_SERVER)
    {
        return 2;
    }
    return 0;
}
