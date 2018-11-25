#pragma once

/**
 * Notes:
 * Handles callbacks from ZNet connection layer and 
 * the client/server command streams that go into/out of the
 * game state
 */
#include "app_network_types.h"
#include "app_network_stream.h"

#include "app_module.cpp" 

/////////////////////////////////////////////////////////////////
// Network callbacks
/////////////////////////////////////////////////////////////////
void Net_ConnectionAccepted(ZNetConnectionInfo* conn)
{
    if (IsRunningServer(g_session.netMode))
    {
        printf("SV Accepted connection %d\n", conn->id);
        // Create client
        Cmd_ClientUpdate spawnClient = {};
		spawnClient.connectionId = conn->id;
        spawnClient.clientId = App_GetNextClientId(&g_session.clientList);
        spawnClient.state = CLIENT_STATE_SYNC;

        // Exec local client update - will be recreated to clients when executed
        APP_WRITE_CMD(0, spawnClient);

        // Prepare sync...?
    }
    else
    {
        printf("CL Connected to server with conn id %d\n", conn->id);
        g_session.remoteConnectionId = conn->id;
    }
}

void Net_ConnectionDropped(ZNetConnectionInfo* conn)
{
    printf("APP Connection %d dropped\n", conn->id);
	if (IsRunningServer(g_session.netMode))
    {
		Client* cl = App_FindClientByConnectionId(&g_session.clientList, conn->id);
        // Delete client
        Cmd_ClientUpdate spawnClient = {};
        spawnClient.clientId = cl->connectionId;
        spawnClient.state = CLIENT_STATE_FREE;
        APP_WRITE_CMD(0, spawnClient);
    }
}

void Net_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes)
{
    //printf("APP Received %d bytes from %d\n", numBytes, info->sender.id);
    switch (g_session.netMode)
    {
        case NETMODE_LISTEN_SERVER:
        {
            ClientList* cls = &g_session.clientList;

            Client* cl = App_FindClientByConnectionId(&g_session.clientList, info->sender.id);
            APP_ASSERT(cl, "SV Unknown client for packet received\n");
            APP_ASSERT((cl->state != CLIENT_STATE_FREE), "SV Client is in state FREE for packet received\n");

            u8* read = bytes;
            //u16 reliableBytes = COM_ReadU16(&read);
            //u16 unreliableBytes = COM_ReadU16(&read);
            //if (reliableBytes > 0)
            //{
                //printf("SV writing %d reliable input bytes to conn %d\n", reliableBytes, cl->connectionId);
                read = Stream_PacketToInput(&cl->stream, read);
            //}
			// Deserialise sync check
			u32 positionCheck = COM_ReadU32(&read);
			if (positionCheck != NET_DESERIALISE_CHECK)
			{
				printf("Deserialise expected %X got %X\n", NET_DESERIALISE_CHECK, positionCheck);
			}
            u16 numUnreliableBytes = COM_ReadU16(&read);
            if (numUnreliableBytes > 0)
            {
                printf("Read %d unreliable bytes\n", numUnreliableBytes);
            }
			//APP_ASSERT(positionCheck == NET_DESERIALISE_CHECK, "Deserialise failed sync check at unreliable section");
        } break;

        case NETMODE_CLIENT:
        {
            //printf("Received %d bytes from %d\n", numBytes, info->sender.id);
            APP_ASSERT(
                (info->sender.id == g_session.remoteConnectionId),
                "Received packet from unknown source");

            u8* read = bytes;
			// Read reliable section
            read = Stream_PacketToInput(&g_serverStream, read);
			// Deserialise sync check
			u32 positionCheck = COM_ReadU32(&read);
			if (positionCheck != NET_DESERIALISE_CHECK)
			{
				printf("Deserialise expected %X got %X\n", NET_DESERIALISE_CHECK, positionCheck);
                return;
			}
            u16 numUnreliableBytes = COM_ReadU16(&read);
            if (numUnreliableBytes > 0)
            {
                printf("Read %d unreliable bytes\n", numUnreliableBytes);
            }
            //printf("Read %d unreliable bytes\n", numUnreliableBytes);
			//APP_ASSERT(positionCheck == NET_DESERIALISE_CHECK, "Deserialise failed sync check at unreliable section");
        } break;
    }
}

void Net_DeliveryConfirmed(ZNetConnectionInfo* conn, u32 packetNumber)
{
    //printf("APP Confirmed delivery of conn %d packet %d\n", conn->id, packetNumber);
    switch (g_session.netMode)
    {
        case NETMODE_LISTEN_SERVER:
        {
            Client* cl = App_FindClientByConnectionId(&g_session.clientList, conn->id);
            APP_ASSERT(cl, "SV Unknown client for packet delivery confirmation\n");
            APP_ASSERT((cl->state != CLIENT_STATE_FREE), "SV Client is in state FREE for delivery confirmation\n");

            //printf("SV - Clearing output to CL %d\n", cl->connectionId);
            Stream_ClearReceivedOutput(&cl->stream, packetNumber);
        } break;

        case NETMODE_CLIENT:
        {
            Stream_ClearReceivedOutput(&g_serverStream, packetNumber);
        } break;
		
		default:
		{
			APP_ASSERT(0, "Delivery Confirmed: Unsupported netmode");
		} break;
    }
}

/////////////////////////////////////////////////////////////////
// Network init
/////////////////////////////////////////////////////////////////
ZNetPlatformFunctions Net_CreateNetworkPlatformFunctions()
{
    ZNetPlatformFunctions netFuncs = {};
	netFuncs.Init = platform.Net_Init;
    netFuncs.Shutdown = platform.Net_Shutdown;
    netFuncs.OpenSocket = platform.Net_OpenSocket;
    netFuncs.CloseSocket = platform.Net_CloseSocket;
    netFuncs.Read = platform.Net_Read;
    netFuncs.SendTo = platform.Net_SendTo;
    netFuncs.FatalError = App_FatalError;
    return netFuncs;
}

ZNetOutputInterface Net_CreateOutputInterface()
{
	ZNetOutputInterface x = {};
	x.ConnectionAccepted = Net_ConnectionAccepted;
	x.ConnectionDropped = Net_ConnectionDropped;
	x.DataPacketReceived = Net_DataPacketReceived;
	x.DeliveryConfirmed = Net_DeliveryConfirmed;
	return x;
}

/////////////////////////////////////////////////////////////////
// Network Read
/////////////////////////////////////////////////////////////////
void Net_ClientExecuteServerMessage(u8* bytes, u16 numBytes)
{
    //printf("CL Exec msg type %d size %d\n", *bytes, numBytes);
    u8 type = *bytes;
    if (type == CMD_TYPE_TEST)
    {
        Cmd_Test cmd;
        u16 bytesRead = cmd.Read(bytes);
        APP_ASSERT((bytesRead == numBytes), "CL Exec SV msg - Read() size mismatch")
        printf(".");
        //printf("CL Keep alive %d\n", cmd.data);
    }
    else
    {
        //printf("CL Exec unknown msg type: %d\n", type);
        APP_COPY_COMMAND_BYTES(0, bytes, numBytes);
    }
}

void Net_ServerExecuteClientMessage(Client* cl, u8* bytes, u16 numBytes)
{
    //printf("SV Exec msg from %d: type %d size %d\n", cl->connectionId, *bytes, numBytes);
    u8 type = *bytes;
    if (type == CMD_TYPE_TEST)
    {
        Cmd_Test cmd;
        u16 bytesRead = cmd.Read(bytes);
        APP_ASSERT((bytesRead == numBytes), "SV Exec CL msg - Read() size mismatch")
        //printf("SV Keep alive %d\n", cmd.data);
        printf(".");
    }
}

void Net_ReadInput(NetStream* stream, Client* nullable_cl)
{
    ByteBuffer* b = &stream->inputBuffer;
    for(;;)
    {
        u32 nextMsg = stream->inputSequence + 1;
        APP_ASSERT(nextMsg != 0, "CL Read input: nextMsg ID is 0");

        StreamMsgHeader* h = Stream_FindMessageById(b->ptrStart, b->ptrWrite, nextMsg);
        if (!h) { return; }

        stream->inputSequence = nextMsg;

        u8* msg = (u8*)h + sizeof(StreamMsgHeader);
        if (nullable_cl)
        {
			Net_ServerExecuteClientMessage(nullable_cl, msg, (u16)h->size);
        }
        else
        {
			Net_ClientExecuteServerMessage(msg, (u16)h->size);
        }
        

        // Clear
        u8* blockStart = (u8*)h;
        u32 blockSize = sizeof(StreamMsgHeader) + h->size;
        b->ptrWrite = Stream_CollapseBlock(blockStart, blockSize, b->ptrWrite);
    }
}

/////////////////////////////////////////////////////////////////
// Network Send
/////////////////////////////////////////////////////////////////
internal void Net_TransmitSay(GameSession* session, char* str, char** tokens, i32 numTokens)
{
    char buf[256];
    COM_ConcatonateTokens(buf, 256, tokens, numTokens, 1);
    printf("NET SAY: \"%s\"\n", buf);
    switch(session->netMode)
    {
        case NETMODE_CLIENT:
        {
            Cmd_Test cmd;
            cmd.data = 999999;
            NET_MSG_TO_OUTPUT((&g_serverStream), (&cmd));
        } break;

        case NETMODE_LISTEN_SERVER:
        {
            Cmd_Test cmd;
            cmd.data = 111111;
            NET_MSG_TRANSMIT_TO_ALL_CLIENTS((&session->clientList), (&cmd));
        } break;
    }
}

internal void Net_TransmitToClients(GameSession* session, GameScene* gs)
{
    if(!IsRunningServer(session->netMode)) { return; }

    const i32 packetSize = 1024;
    u8 packetBytes[packetSize];
    ByteBuffer packetBuf = Buf_FromBytes(packetBytes, packetSize);
    Buf_Clear(&packetBuf);

    for (i32 i = 0; i < session->clientList.max; ++i)
    {
        Client* cl = &session->clientList.items[i];
        if (cl->state == CLIENT_STATE_FREE) { continue; }

        // TODO: Preventing transmit to local clients... this will have to loopback
        if (cl->connectionId == 0) { continue; }

        // TODO: Sending once per tick regardless of framerate at the moment
        ByteBuffer* b = &cl->stream.outputBuffer;
        
        i32 pendingBytes = b->Written();
        if (pendingBytes == 0)
        {
            // nothing to transmit - write keep alive
            // acks are piggy-backed on regular packets. No traffic == no acks
            //printf("SV 2 CL: Nothing to transmit, writing keepalive\n");
            Cmd_Test cmd = {};
            cmd.data = 1234;
            NET_MSG_TO_OUTPUT((&cl->stream), (&cmd));
        }
        //printf("%dB, ", pendingBytes);
        
		// Write reliable
        Stream_OutputToPacket(cl->connectionId, &cl->stream, &packetBuf);
		// sync check
		packetBuf.ptrWrite += COM_WriteU32(NET_DESERIALISE_CHECK, packetBuf.ptrWrite);
        // Write unreliable
        packetBuf.ptrWrite += COM_WriteU16(0, packetBuf.ptrWrite);
        // Send
        ZNet_SendData(cl->connectionId, packetBuf.ptrStart, (u16)packetBuf.Written(), 0);
    }
}

/**
 * Load UNRELIABLE local client inputs into a server packet
 * returns bytesWritten
 */
u32 Net_WriteClient2ServerOutput(
    GameSession* session,
    GameScene* gs,
    NetStream* server,
    ByteBuffer* packetBuf)
{
    if(!IsRunningClient(session->netMode))
    {
        printf("CL2SV Not running a client!\n");
        return 0;
    }

    u8* headerPos = packetBuf->ptrWrite;
    packetBuf->ptrWrite += NET_SIZE_OF_UNRELIABLE_HEADER;

    Client* cl = App_FindLocalClient(&session->clientList, 0);

    if (cl)
    {
       Cmd_PlayerInput cmd = {};
       cmd.clientId = cl->clientId;
       cmd.input = cl->input;
       packetBuf->ptrWrite += cmd.Write(packetBuf->ptrWrite);
       //NET_MSG_TO_OUTPUT(&g_serverStream, &cmd);
    }
    else
    {
        printf("CL No local client %d to send!\n", session->clientList.localClientId);
    }

    u16 bytesWritten = (u16)(packetBuf->ptrWrite - (headerPos + sizeof(u16)));
    //printf("CL Wrote %d unreliable bytes\n", bytesWritten);
    COM_WriteU16(bytesWritten, headerPos);
    return bytesWritten;
}

internal void Net_TransmitToServer(GameSession* session, GameScene* gs)
{
    if(!IsRunningClient(session->netMode)) { return; }
    // Conn Id will be 0 if connection has not been established so drop out
    if (g_session.remoteConnectionId == 0) { return; }

    const i32 packetSize = 1024;
    u8 packetBytes[packetSize];
    ByteBuffer packetBuf = Buf_FromBytes(packetBytes, packetSize);
    // sanitise packet
    Buf_Clear(&packetBuf);

    ByteBuffer* outputBuf = &g_serverStream.outputBuffer;
    i32 pendingBytes = outputBuf->Written();
    if (pendingBytes == 0)
    {
        // nothing to transmit - write keep alive
        // acks are piggy-backed on regular packets. No traffic == no acks
        // Will also automatically keep the connection alive
        //printf("CL 2 SV: Nothing to transmit, writing keepalive\n");
        Cmd_Test cmd = {};
        cmd.data = 5678;
        NET_MSG_TO_OUTPUT(&g_serverStream, &cmd)
    }
    //printf("CL %d bytes in output buffer\n", b->Written());
    // Write reliable messages
    Stream_OutputToPacket(session->remoteConnectionId, &g_serverStream, &packetBuf);

    // Write magic number for read validation between sections
    packetBuf.ptrWrite += COM_WriteU32(NET_DESERIALISE_CHECK, packetBuf.ptrWrite);
    
    // Write unreliable messages
    Net_WriteClient2ServerOutput(session, gs, &g_serverStream, &packetBuf);
    //// TODO: Output player input to SV
    //Client* cl = App_FindLocalClient(&session->clientList, 0);
    //if (cl)
    //{
    //    Cmd_PlayerInput cmd = {};
    //    cmd.clientId = cl->clientId;
    //    cmd.input = cl->input;
    //    NET_MSG_TO_OUTPUT(&g_serverStream, &cmd);
    //}
    //

    // Send
    ZNet_SendData(session->remoteConnectionId, packetBuf.ptrStart, (u16)packetBuf.Written(), 0);
}

/////////////////////////////////////////////////////////////////
// Network Frame loop
/////////////////////////////////////////////////////////////////
internal void Net_Tick(GameSession* session, GameScene* gs, GameTime* time)
{
    switch (session->netMode)
    {
        case NETMODE_NONE:
        {
            return;
        } break;

        case NETMODE_LISTEN_SERVER:
        {
			printf(".");
            ZNet_Tick(time->deltaTime);
            for (i32 i = 0; i < session->clientList.max; ++i)
            {
                Client* cl = &session->clientList.items[i];
                if (cl->state == CLIENT_STATE_FREE) { continue; }
                Net_ReadInput(&cl->stream, cl);
            }
        } break;

        // case NETMODE_DEDICATED_SERVER:
        // {
        //     ZNet_Tick(time->deltaTime);
        // } break;

        case NETMODE_CLIENT:
        {
            ZNet_Tick(time->deltaTime);
            Net_ReadInput(&g_serverStream, NULL);
        } break;

        default:
        {
            APP_ASSERT(0, "Unknown netmode\n");
        } break;
    }
}

internal void Net_Transmit(GameSession* session, GameScene* gs, GameTime* time)
{
    switch (session->netMode)
    {
        case NETMODE_NONE:
        {
            return;
        } break;

        case NETMODE_LISTEN_SERVER:
        {
            Net_TransmitToClients(session, gs);
        } break;

        case NETMODE_DEDICATED_SERVER:
        {
            Net_TransmitToClients(session, gs);
        } break;

        case NETMODE_CLIENT:
        {
            // Transmit input
            Net_TransmitToServer(session, gs);
        } break;

        default:
        {
            APP_ASSERT(0, "Unknown netmode\n");
        } break;
    }
}
