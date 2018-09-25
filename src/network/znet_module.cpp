#ifndef ZNET_MODULE_CPP
#define ZNET_MODULE_CPP

#include <stdio.h>
#include "znet_interface.h"

/*
App Network layer functionality:

> Packet Read/Write
> App -> platform communication (do this via platform input buffer?)
> Handle connection persistence

> System will open one unreliable UDP socket
> connections are managed via randomly assigned ids.
> connections store the 

*/

#define NET_ASSERT(expression, msg) if(!(expression)) \
{ \
    char assertBuf[512]; \
    sprintf_s(assertBuf, 512, "%s, %d: %s\n", __FILE__, __LINE__, msg); \
    Net_FatalError(assertBuf, "Fatal Net Error"); \
} \

#define ZNET_DEFAULT_PORT 23232
#define ZNET_CONNECTION_FLAG_LOCAL (1 << 0)

#define ZNET_STATE_DISCONNECTED 0
// client side
#define ZNET_STATE_CONNECTING 1
#define ZNET_STATE_RESPONDING 2
#define ZNET_STATE_CONNECTED 3
// server side
#define ZNET_STATE_CHALLENGED 4
#define ZNET_STATE_SERVER 5

// id of 0 == unassigned.
struct ZNetConnection
{
    u32 id;
    u32 sequence;
    u32 challenge;
    //i32 state;
    ZNetAddress remoteAddress;
    u32 flags;
    f32 tick;
};

#define ZNET_PROTOCOL 90

struct ZNetPacketHeader
{
    u32 protocol;
    i32 dataChecksum;   // TODO implement this
};

#define ZNET_MSG_TYPE_NONE 0
#define ZNET_MSG_TYPE_CONNECTION_REQUEST 1111
#define ZNET_MSG_TYPE_CHALLENGE 2
#define ZNET_MSG_TYPE_CHALLENGE_RESPONSE 3
#define ZNET_MSG_TYPE_CONNECTION_APPROVED 4


struct ZNetConnectionStatus
{
    i32 type;
    u32 challenge;
};

struct ZNetPendingConnection
{
    ZNetAddress address;
    u32 challenge;
};

#define MAX_CONNECTIONS 16
#define MAX_PENDING_CONNECTIONS 32
struct ZNet
{
    ZNetConnection connections[MAX_CONNECTIONS];
    i32 maxConnections = MAX_CONNECTIONS;
    i32 socketIndex;
    i32 state;
    u16 serverPort;
    i32 client2ServerId;
    u32 tickCount;

    ZNetPendingConnection pendingConnections[MAX_PENDING_CONNECTIONS];
};

/////////////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////////////

#define ZNET_DATA_WRITE_SIZE 1024   // >= size of data for this packet
#define ZNET_PACKET_WRITE_SIZE 1400 // >= size of data + packet header
#define ZNET_PACKET_READ_SIZE 1400 // >= size of data + packet header
// Buffers
// write data to this:
internal u8 g_dataWriteBuffer[ZNET_DATA_WRITE_SIZE];
// final write buffer before call to platform send
internal u8 g_packetWriteBuffer[ZNET_PACKET_WRITE_SIZE];

internal u8 g_packetReadBuffer[ZNET_PACKET_READ_SIZE];

internal ZNetPlatformFunctions g_netPlatform;
internal ZNet g_net;

internal ZNet_ErrorHandler g_errorHandler = NULL;

internal void Net_FatalError(char* message, char* heading)
{
    if (g_netPlatform.FatalError)
    {
        g_netPlatform.FatalError(message, heading);
    }
    else
    {
        printf("NET FATAL: %s\n", message);
        ILLEGAL_CODE_PATH
    }
}

#include "znet_connection.h"
#include "znet_network.h"

///////////////////////////////////////////////////
// system lifetime
///////////////////////////////////////////////////
void ZNet_Init(ZNetPlatformFunctions platform)
{
    printf("ZNet Initialising... ");
    g_netPlatform = platform;
    printf("Done\n");
}

void ZNet_Shutdown()
{
    printf("ZNet shutting down... ");
    ZNet* net = &g_net;
    net->state = 0;
    if (net->socketIndex != -1)
    {
        g_netPlatform.CloseSocket(net->socketIndex);
        net->socketIndex = -1;
    }
    printf("Done\n");
}

// ZNET_MODULE_CPP
#endif
