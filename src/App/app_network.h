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


internal void Net_ServerReadUnreliable(Client* cl, u8* ptr, u16 numBytes);
internal void Net_ClientReadUnreliable(u8* bytes, u16 numBytes);

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
        printf("  - issuing update cmd for clientId %d\n", spawnClient.clientId);

        // Exec local client update - will be recreated to clients when executed
        APP_WRITE_CMD(0, spawnClient);
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
		if (cl == NULL)
		{
			printf("  ERROR No Client to remove with Id %d\n", conn->id);
			return;
		}
        printf("SV Write Removal of client %d\n", cl->clientId);
        // Delete client
        Cmd_ClientUpdate cmd = {};
        cmd.clientId = cl->clientId;
        cmd.connectionId = cl->connectionId;
        cmd.state = CLIENT_STATE_FREE;
        APP_WRITE_CMD(0, cmd);
    }
    else
    {
        printf(">>> CL WRITE DISCONNECT CMD\n");
        Cmd_Quick cmd = {};
        cmd.SetAsConnectionLost(0);
        APP_WRITE_CMD(0, cmd);
    }
}

void Net_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes)
{
	i32 sizeofPacketCmdHeader = sizeof(u8) + (sizeof(i32) * 2);
	i32 numCommandBytes = sizeofPacketCmdHeader + numBytes;
	CmdHeader h = {};
    h.Set(CMD_TYPE_PACKET, (u16)numCommandBytes);
	
	u8* write = App_CreateCommandSpace(CMD_TYPE_PACKET, 0, (u16)numCommandBytes);
	write += COM_WriteByte(CMD_TYPE_PACKET, write);
	write += COM_WriteI32(info->sender.id, write);
	write += COM_WriteI32(numBytes, write);
	write += COM_COPY(bytes, write, numBytes);
}

void Net_DeliveryConfirmed(ZNetConnectionInfo* conn, u32 packetNumber)
{
    Cmd_Quick cmd = {};
    cmd.SetAsPacketDelivered(conn->id, packetNumber);
    APP_WRITE_CMD(0, cmd);
}

/////////////////////////////////////////////////////////////////
// Network callback event processing
/////////////////////////////////////////////////////////////////
internal void Net_ProcessPacketDelivery(GameSession* session, i32 connId, u32 packetNumber)
{
    switch (session->netMode)
    {
        case NETMODE_LISTEN_SERVER:
        {
            Client* cl = App_FindClientByConnectionId(&session->clientList, connId);
            APP_ASSERT(cl, "SV Unknown client for packet delivery confirmation\n");
            APP_ASSERT((cl->state != CLIENT_STATE_FREE), "SV Client is in state FREE for delivery confirmation\n");
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

internal void Net_ProcessPacket(i32 sourceConnectionId, u8* bytes, u16 numBytes)
{
    // TODO: Copy packet reading to app command buffer
    switch (g_session.netMode)
    {
        case NETMODE_LISTEN_SERVER:
        {
            //Stream_PrintPacketManifest(bytes, numBytes);
            ClientList* cls = &g_session.clientList;
            
            Client* cl = App_FindClientByConnectionId(&g_session.clientList, sourceConnectionId);
            APP_ASSERT(cl, "SV Unknown client for packet received\n");
            APP_ASSERT((cl->state != CLIENT_STATE_FREE),
                "SV Client is in state FREE for packet received\n");

            u8* read = bytes;
            read = Stream_PacketToInput(&cl->stream, read);
			// Deserialise sync check
			u32 positionCheck = COM_ReadU32(&read);
			if (positionCheck != NET_DESERIALISE_CHECK)
			{
				printf("SV ABORT Deserialise check %s got 0x%X\n",
                    NET_DESERIALISE_CHECK_LABEL,
                    positionCheck);
                COM_PrintBytesHex(bytes, numBytes, 32);
                return;
			}
            u16 numUnreliableBytes = COM_ReadU16(&read);
            if (numUnreliableBytes > 0)
            {
                Net_ServerReadUnreliable(cl, read, numUnreliableBytes);
            }
        } break;

        case NETMODE_CLIENT:
        {
            APP_ASSERT(
                (sourceConnectionId == g_session.remoteConnectionId),
                "Received packet from unknown source");

            u8* read = bytes;
			// Read reliable section
            read = Stream_PacketToInput(&g_serverStream, read);
			// Deserialise sync check
			u32 positionCheck = COM_ReadU32(&read);
			if (positionCheck != NET_DESERIALISE_CHECK)
			{
				printf("CL ABORT Deserialise check %s got 0x%X\n",
                    NET_DESERIALISE_CHECK_LABEL,
                    positionCheck);
                Stream_PrintPacketManifest(bytes, numBytes);
                COM_PrintBytesHex(bytes, numBytes, 32);
                return;
			}
            u16 numUnreliableBytes = COM_ReadU16(&read);
            if (numUnreliableBytes > 0)
            {
                Net_ClientReadUnreliable(read, numUnreliableBytes);
            }
        } break;

        default:
        {
            printf("NET Unknown mode %d for processing packet!\n", g_session.netMode);
            ILLEGAL_CODE_PATH
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
// Network Execute from read
/////////////////////////////////////////////////////////////////
internal void Net_ClientExecuteServerMessage(u8* bytes, u16 numBytes)
{
    u8 type = *bytes;
    u16 bytesRead = 0;
    // Can black list specific commands if required here
    if (type == CMD_TYPE_TEST)
    {
        Cmd_Test cmd;
        bytesRead = cmd.Read(bytes);
        if (numBytes != UINT16_MAX)
        {
            APP_ASSERT((bytesRead == numBytes), "CL Exec SV msg - Read() size mismatch")
        }
        return;
    }
    else if (type == CMD_TYPE_ENTITY_STATE_2)
    {
        printf("CL Received Ent State\n");
    }
    //printf("CL Write SV msg to App Buffer %d\n", type);
    APP_COPY_COMMAND_BYTES(0, bytes, numBytes);
}

internal void Net_ClientReadUnreliable(u8* bytes, u16 numBytes)
{
    u8* end = bytes + numBytes;
    while (bytes < end)
    {
        u8 sequenceOffset;
        u16 size;
        Stream_ReadPacketHeader(COM_ReadU16(&bytes), &sequenceOffset, &size);
        if (size == 0) { printf("CL SV Cmd gave a size of 0!"); return; }
        Net_ClientExecuteServerMessage(bytes, size);
        bytes += size;
    }
}

/////////////////////////////////////////////////////////////////
// Network Read
// Pull commands from the given stream and 
/////////////////////////////////////////////////////////////////

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

/**
 * Load UNRELIABLE local client inputs into a server packet
 * returns bytesWritten
 * - Always write something here to keep conn alive
 * 
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
        if (cl->state == CLIENT_STATE_PLAYING)
        {
            Cmd_PlayerInput cmd = {};
            cmd.clientId = cl->clientId;
            cmd.input = cl->input;
            packetBuf->ptrWrite += cmd.Write(packetBuf->ptrWrite);
            //NET_MSG_TO_OUTPUT(&g_serverStream, &cmd);
        }
        else
        {
            // nothing to transmit - write keep alive
            // acks are piggy-backed on regular packets. No traffic == no acks
            // Will also automatically keep the connection alive
            packetBuf->ptrWrite += COM_WriteU16(
                Stream_WritePacketHeader(0, 5), packetBuf->ptrWrite);
            Cmd_Test cmd = {};
            cmd.data = 5678;
            packetBuf->ptrWrite += cmd.Write(packetBuf->ptrWrite);
        }
    }
    else
    {
        printf("CL No local client %d to send from!\n",
            session->clientList.localClientId);
    }

    u16 bytesWritten = (u16)(packetBuf->ptrWrite - (headerPos + sizeof(u16)));
    COM_WriteU16(bytesWritten, headerPos);
    return bytesWritten;
}

internal void Net_TransmitToServer(GameSession* session, GameScene* gs, GameTime* time)
{
    if(!IsRunningClient(session->netMode)) { return; }
    // Conn Id will be 0 if connection has not been established so drop out
    if (g_session.remoteConnectionId == 0) { return; }

    const i32 packetSize = 1024;
    u8 packetBytes[packetSize];
    ByteBuffer packetBuf = Buf_FromBytes(packetBytes, packetSize);
    // sanitise packet
    Buf_Clear(&packetBuf);
    
    // Write reliable messages
    Stream_OutputToPacket(
        session->remoteConnectionId, &g_serverStream, &packetBuf, time->deltaTime);

    // Write magic number for read validation between sections
    packetBuf.ptrWrite += COM_WriteU32(NET_DESERIALISE_CHECK, packetBuf.ptrWrite);
    
    // Write unreliable messages
    Net_WriteClient2ServerOutput(session, gs, &g_serverStream, &packetBuf);

    // Send
    ZNet_SendData(
        session->remoteConnectionId, packetBuf.ptrStart, (u16)packetBuf.Written(), 0);
}

internal void NET_WriteImpulse(GameSession* gs, i32 impulse)
{
    Cmd_ServerImpulse cmd = {};
    cmd.clientId = gs->clientList.localClientId;
    cmd.impulse = impulse;
    if (IS_SERVER)
    {
        APP_WRITE_CMD(0, cmd);
    }
    else if (IS_CLIENT)
    {
        printf("CL Write Impulse %d to server\n", impulse);
        NET_MSG_TO_OUTPUT(&g_serverStream, &cmd);
    }
    else
    {
        printf("NET Cannot impulse, not running\n");
    }
}

/////////////////////////////////////////////////////////////////
// Network Frame loop
/////////////////////////////////////////////////////////////////
internal void Net_ReadPackets(GameSession* session, GameScene* gs, GameTime* time)
{
    switch (session->netMode)
    {
        case NETMODE_NONE:
        {
            return;
        } break;

        case NETMODE_LISTEN_SERVER:
        {
            ZNet_Tick(time->deltaTime);
        } break;

        // case NETMODE_DEDICATED_SERVER:
        // {
        //     ZNet_Tick(time->deltaTime);
        // } break;

        case NETMODE_CLIENT:
        {
            ZNet_Tick(time->deltaTime);
        } break;

        default:
        {
            APP_ASSERT(0, "Unknown netmode\n");
        } break;
    }
}

internal void Net_ReadInputStreams(GameSession* session, GameScene* gs, GameTime* time)
{
    switch (session->netMode)
    {
        case NETMODE_NONE:
        {
            return;
        } break;

        case NETMODE_LISTEN_SERVER:
        {
            for (i32 i = 0; i < session->clientList.max; ++i)
            {
                Client* cl = &session->clientList.items[i];
                if (cl->state == CLIENT_STATE_FREE) { continue; }
                Net_ReadInput(&cl->stream, cl);
                
                // If client is syncing, check whether they have hi tthe end of their
                // sync command queue
                if (cl->state == CLIENT_STATE_SYNC)
                {
                    i32 diff = cl->syncCompleteMessageId - cl->stream.ackSequence;
                    printf("SV Sync %d has %d remaining cmds\n", cl->connectionId, diff);
                    printf("  sync complete on %d, seq %d\n",
                        cl->syncCompleteMessageId,
                        cl->stream.ackSequence);
                    if (diff <= 0)
                    {
                        Cmd_ClientUpdate cmd = {};
                        cmd.Set(cl, 0);
                        cmd.state = CLIENT_STATE_OBSERVER;
                        APP_WRITE_CMD(0, cmd);
                    }
                }
            }
        } break;

        // case NETMODE_DEDICATED_SERVER:
        // {
        //     ZNet_Tick(time->deltaTime);
        // } break;

        case NETMODE_CLIENT:
        {
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
            Net_TransmitToClients(session, gs, time);
        } break;

        case NETMODE_DEDICATED_SERVER:
        {
            Net_TransmitToClients(session, gs, time);
        } break;

        case NETMODE_CLIENT:
        {
            // Transmit input
            Net_TransmitToServer(session, gs, time);
        } break;

        default:
        {
            APP_ASSERT(0, "Unknown netmode\n");
        } break;
    }
}

internal char* Net_GetModeLabel(u8 netMode)
{
    switch (netMode)
    {
        case NETMODE_SINGLE_PLAYER: return "Single player";
        case NETMODE_CLIENT: return "Client";
        case NETMODE_LISTEN_SERVER: return "Listen Server";
        case NETMODE_REPLAY: return "Replay";
        case NETMODE_DEDICATED_SERVER: "Dedicated Server";
        case NETMODE_NONE: return "None";
        default: return "Unknown mode";
    }
}

internal void Net_WriteDebug(ZStringHeader* txt, GameSession* session)
{
    char* start = txt->chars;
    char* write = start;
    char* end = start + txt->maxLength;

    write += sprintf_s(write, (end - write),
        "===== NETWORK =====\nMode: %s\nRemote ConnId: %d\nLocal Client ID: %d\n-----CLIENTS-----\n",
        Net_GetModeLabel(session->netMode),
        session->remoteConnectionId,
        session->clientList.localClientId
    );

    ClientList* cls = &session->clientList;
    i32 numClients = cls->max;
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client* cl = &cls->items[i];
        if (cl->state == CLIENT_STATE_FREE) { continue; }
        write += sprintf_s(write, (end - write),
            "ID: %d: IsLocal %d, State %d, Avatar: %d/%d, Input: %d ConnId %d\n",
				cl->clientId, cl->isLocal,
                cl->state, cl->entId.iteration,
                cl->entId.index, cl->input.buttons,
                cl->connectionId
        );
    }

    write = ZNet_WriteDebug(write, end);

    txt->length = (end - write);
}
