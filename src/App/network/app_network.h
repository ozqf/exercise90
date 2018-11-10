#pragma once

/**
 * Notes:
 * Handles callbacks from ZNet connection layer and 
 * the client/server command streams that go into/out of the
 * game state
 */
#include "app_network_types.h"
#include "app_network_stream.h"

#include "../app_module.cpp"

/////////////////////////////////////////////////////////////////
// Network callbacks
/////////////////////////////////////////////////////////////////
void Net_ConnectionAccepted(ZNetConnectionInfo* conn)
{
    printf("APP Connection %d accepted\n", conn->id);
    if (IsRunningServer(g_gameState.netMode))
    {
        // Create client
        Cmd_ClientUpdate spawnClient = {};
		spawnClient.connectionId = conn->id;
        spawnClient.clientId = App_GetNextClientId(&g_gameState.clientList);
        spawnClient.state = CLIENT_STATE_OBSERVER;
        APP_WRITE_CMD(0, CMD_TYPE_CLIENT_UPDATE, 0, spawnClient);
    }
}

void Net_ConnectionDropped(ZNetConnectionInfo* conn)
{
    printf("APP Connection %d dropped\n", conn->id);
	if (IsRunningServer(g_gameState.netMode))
    {
		Client* cl = App_FindClientByConnectionId(&g_gameState.clientList, conn->id);
        // Delete client
        Cmd_ClientUpdate spawnClient = {};
        spawnClient.clientId = cl->connectionId;
        spawnClient.state = CLIENT_STATE_FREE;
        APP_WRITE_CMD(0, CMD_TYPE_CLIENT_UPDATE, 0, spawnClient);
    }
}

void Net_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes)
{
    printf("APP Received %d bytes from %d\n", numBytes, info->sender.id);
}

void Net_DeliveryConfirmed(ZNetConnectionInfo* conn, u32 packetNumber)
{
    printf("APP Confirmed delivery of conn %d packet %d\n", conn->id, packetNumber);
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


internal void Net_Tick(GameState* gs, GameTime* time)
{
    switch (gs->netMode)
    {
        case NETMODE_NONE:
        {
            return;
        } break;

        case NETMODE_LISTEN_SERVER:
        {
            ZNet_Tick(time->deltaTime);
        } break;

        case NETMODE_DEDICATED_SERVER:
        {
            ZNet_Tick(time->deltaTime);
        } break;

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
