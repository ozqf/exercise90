#ifndef ZNET_MODULE_CPP
#define ZNET_MODULE_CPP

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

internal ZNet_ErrorHandler g_errorHandler = NULL;

internal void Net_FatalError(char* message, char* heading)
{
    if (g_errorHandler)
    {
        g_errorHandler(message, heading);
    }
    else
    {
        printf("NET FATAL: %s\n", message);
        ILLEGAL_CODE_PATH
    }
}

#define NET_ASSERT(expression, msg) if(!(expression)) \
{ \
    char assertBuf[512]; \
    sprintf_s(assertBuf, 512, "%s, %d: %s\n", __FILE__, __LINE__, msg); \
    Net_FatalError(assertBuf, "Fatal error"); \
} \

#define ZNET_DEFAULT_PORT 23232
#define ZNET_CONNECTION_FLAG_LOCAL (1 << 0)

#define ZNET_SV2C_STATE_DISCONNECTED 0
#define ZNET_SV2C_STATE_CHALLENGED 1
#define ZNET_SV2C_STATE_CONNECTED 2

// id of 0 == unassigned.
struct ZNetConnection
{
    u32 id;
    u32 sequence;
    u32 challenge;
    i32 state;
    ZNetAddress address;
    u32 flags;
    f32 tick;
};

#define MAX_CONNECTIONS 16
struct ZNet
{
    ZNetConnection connections[MAX_CONNECTIONS];
    i32 maxConnections = MAX_CONNECTIONS;
    i32 socketIndex;
    i32 netMode;
};


/////////////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////////////
internal ZNetPlatformFunctions g_netPlatform;
internal ZNet g_net;

#include "znet_network.h"

// system lifetime
void ZNet_Init(ZNetPlatformFunctions platform)
{
    g_netPlatform = platform;
}

void ZNet_Shutdown()
{
    ZNet* net = &g_net;
    net->netMode = 0;
    if (net->socketIndex != -1)
    {
        g_netPlatform.CloseSocket(net->socketIndex);
        net->socketIndex = -1;
    }
}

// ZNET_MODULE_CPP
#endif
