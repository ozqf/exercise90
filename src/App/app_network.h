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
    printf("APP Connection %d accepted\n", conn->id);
    if (IsRunningServer(g_session.netMode))
    {
        // Create client
        Cmd_ClientUpdate spawnClient = {};
		spawnClient.connectionId = conn->id;
        spawnClient.clientId = App_GetNextClientId(&g_session.clientList);
        spawnClient.state = CLIENT_STATE_OBSERVER;
        APP_WRITE_CMD(0, CMD_TYPE_CLIENT_UPDATE, 0, spawnClient);
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
        APP_WRITE_CMD(0, CMD_TYPE_CLIENT_UPDATE, 0, spawnClient);
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
            u16 reliableBytes = COM_ReadU16(&read);
            u16 unreliableBytes = COM_ReadU16(&read);
            if (reliableBytes > 0)
            {
                //printf("SV writing %d reliable input bytes to conn %d\n", reliableBytes, cl->connectionId);
                Stream_PacketToInput(&cl->stream, read, reliableBytes);
            }
        } break;

        case NETMODE_CLIENT:
        {
            //printf("Received %d bytes from %d\n", numBytes, info->sender.id);
            APP_ASSERT(
                (info->sender.id == g_session.remoteConnectionId),
                "Received packet from unknown source");

            u8* read = bytes;
            u16 reliableBytes = COM_ReadU16(&read);
            u16 unreliableBytes = COM_ReadU16(&read);
            if (reliableBytes > 0)
            {
                Stream_PacketToInput(&g_serverStream, read, reliableBytes);
            }
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
        printf("CL Keep alive %d\n", cmd.data);
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
        printf("SV Keep alive %d\n", cmd.data);
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
    u8 packetBuffer[packetSize];

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
        
        Stream_OutputToPacket(cl->connectionId, &cl->stream, packetBuffer, packetSize);
    }
}

/**
 * Load local client inputs into a server packet
 * TODO: These messages should be UNRELIABLE but only the reliable stream exists atm so change later
 */
internal void Net_WriteClient2ServerOutput(GameSession* session, GameScene* gs, Client* cl, NetStream* server)
{
    if(!IsRunningClient(session->netMode)) { return; }
}

internal void Net_TransmitToServer(GameSession* session, GameScene* gs)
{
    if(!IsRunningClient(session->netMode)) { return; }
    if (g_session.remoteConnectionId == 0)
    {
        printf("CL Attempting send to SV but conn is 0\n");
        return;
    }
    const i32 packetSize = 1024;
    u8 packetBuffer[packetSize];

    ByteBuffer* b = &g_serverStream.outputBuffer;
    i32 pendingBytes = b->Written();
    if (pendingBytes == 0)
    {
        // nothing to transmit - write keep alive
        // acks are piggy-backed on regular packets. No traffic == no acks
        //printf("CL 2 SV: Nothing to transmit, writing keepalive\n");
        Cmd_Test cmd = {};
        cmd.data = 5678;
        NET_MSG_TO_OUTPUT((&g_serverStream), (&cmd));
    }
    //printf("CL %d bytes in output buffer\n", b->Written());
    Stream_OutputToPacket(session->remoteConnectionId, &g_serverStream, packetBuffer, packetSize);
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
