#ifndef ZNET_MODULE_CPP
#define ZNET_MODULE_CPP

#include <stdio.h>
#include <ctime>
#include "znet_interface.h"

/*
App Network layer functionality:

> Packet Read/Write
> App -> platform communication (do this via platform input buffer?)
> Handle connection persistence

> System will open one unreliable UDP socket
> connections are identified by ids which are xors of random numbers generated on
	either connection side.


*/

#define NET_ASSERT(expression, msg) if(!(expression)) \
{ \
    char assertBuf[512]; \
    sprintf_s(assertBuf, 512, "%s, %d: %s\n", __FILE__, __LINE__, msg); \
    Net_FatalError(assertBuf, "Fatal Net Error"); \
} \

//#define ZNET_DEFAULT_PORT 23232
#define ZNET_CONNECTION_FLAG_LOCAL (1 << 0)

#define ZNET_CONNECTION_TIMEOUT_TICKS 4

#define ZNET_STATE_DISCONNECTED 0
// client side
#define ZNET_STATE_CONNECTING 1
#define ZNET_STATE_RESPONDING 2
#define ZNET_STATE_CONNECTED 3
// server side
#define ZNET_STATE_CHALLENGED 4
#define ZNET_STATE_SERVER 5

#define ZNET_CONN_TYPE_NONE 0
#define ZNET_CONN_TYPE_CLIENT2SERVER 1
#define ZNET_CONN_TYPE_SERVER2CLIENT 2

// id of 0 == unassigned.
struct ZNetConnection
{
    i32 id;
    i32 type;
    u32 sequence;
    u32 remoteSequence;
    u32 challenge;
    //i32 salt;
    ZNetAddress remoteAddress;
    u32 flags;
    f32 tick;
    i32 pendingKeepAlive;
    i32 keepAliveSendTicks;
	i32 ticksSinceLastMessage;
};

#define ZNET_PROTOCOL 90
/*
Packet structure:
> Packet header:
    > u32 protocol magic number
    > i32 payload checksum
> Payload
    > u8 type
    > data...

Data layout is different for each packet type
for most:
> client/server xor - IDs the connection.
*/

struct ZNetPacketHeader
{
    u32 protocol;
    i32 dataChecksum;

    inline void Read(u8** ptr)
    {
        *ptr += COM_COPY(*ptr, this, sizeof(ZNetPacketHeader));
    }
    
    inline i32 Read(u8* ptr)
    {
        return COM_COPY(ptr, this, sizeof(ZNetPacketHeader));
    }

    inline void Write(u8** ptr)
    {
        *ptr += COM_COPY(this, *ptr, sizeof(ZNetPacketHeader));
    }

    inline i32 Write(u8* ptr)
    {
        return COM_COPY(this, ptr, sizeof(ZNetPacketHeader));
    }

    i32 Measure()
    {
        return sizeof(ZNetPacketHeader);
    }
};

struct ZNetPayloadHeader
{
	u8 type;
    u32 salt;
	u32 sequence;
};

struct ZNetPacket
{
    ZNetPacketHeader header;
    ZNetAddress address;
    u8* bytes;
    i32 numBytes;
};

#define ZNET_MSG_TYPE_NONE 0
#define ZNET_MSG_TYPE_CONNECTION_REQUEST 1
#define ZNET_MSG_TYPE_CHALLENGE 2
#define ZNET_MSG_TYPE_CHALLENGE_RESPONSE 3
#define ZNET_MSG_TYPE_CONNECTION_APPROVED 4
#define ZNET_MSG_TYPE_CONNECTION_DENIED 5
#define ZNET_MSG_TYPE_KEEP_ALIVE 6
#define ZNET_MSG_TYPE_KEEP_ALIVE_RESPONSE 7
#define ZNET_MSG_TYPE_DATA 8


struct ZNetConnectionStatus
{
    i32 type;
    u32 challenge;
};

struct ZNetPending
{
    ZNetAddress address;
    u32 clientSalt;
    u32 challenge;          // if challenge is 0 then connection is considered free
    i32 ticks;
};

#define MAX_CONNECTIONS 16
#define MAX_PENDING_CONNECTIONS 32
// assuming tick rate of 60, timeout after 30 seconds
// TODO: Switch to delta time so tick rate can vary!
#define MAX_PENDING_CONNECTION_TICKS 1800
struct ZNet
{
    ZNetConnection connections[MAX_CONNECTIONS];
	i32 isListening;
    i32 maxConnections = MAX_CONNECTIONS;
    i32 socketIndex;
    i32 state;
    u16 selfPort;
    i32 client2ServerId;
    u32 tickCount;

    ZNetPending pendingConnections[MAX_PENDING_CONNECTIONS];
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

internal i32 ZNet_CreateSalt()
{
    i32 result = 0;
    do
    {
        result = (i32)(COM_STDRandf32() * INT_MAX);
    } while (result == 0);
    return result;
}

// if anything is async in future this will pull a free buffer from a list
internal inline ByteBuffer ZNet_GetDataWriteBuffer()
{
	return Buf_FromBytes(g_dataWriteBuffer, ZNET_DATA_WRITE_SIZE);
}
internal inline ByteBuffer ZNet_GetPacketWriteBuffer()
{
	return Buf_FromBytes(g_packetWriteBuffer, ZNET_PACKET_WRITE_SIZE);
}

// Internal interface
internal void ZNet_Send(ZNetAddress* address, u8* bytes, i32 numBytes);

#include "znet_packet.h"
#include "znet_messages.h"
#include "znet_connection.h"
#include "znet_pending.h"
#include "znet_main.h"

///////////////////////////////////////////////////
// system lifetime
///////////////////////////////////////////////////
void ZNet_Init(ZNetPlatformFunctions platform)
{
    printf("ZNet Initialising... ");
    g_netPlatform = platform;
    COM_ZeroMemory((u8*)&g_net, sizeof(ZNet));
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
