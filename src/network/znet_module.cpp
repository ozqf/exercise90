#ifndef ZNET_MODULE_CPP
#define ZNET_MODULE_CPP

#include <stdio.h>
#include <stdlib.h>
#include <ctime>

//////////////////////////////////////////////////////////////
// External interface
//////////////////////////////////////////////////////////////
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

#define NET_ASSERT(ptrNet, expression, msg) if(!(expression)) \
{ \
    char assertBuf[512]; \
    sprintf_s(assertBuf, 512, "%s, %d: %s\n", __FILE__, __LINE__, msg); \
    Net_FatalError(ptrNet, assertBuf, "Fatal Net Error"); \
} \

//#define ZNET_DEFAULT_PORT 23232
#define ZNET_CONNECTION_FLAG_LOCAL (1 << 0)

#define ZNET_CONNECTION_TIMEOUT_SECONDS 10

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
#define ZNET_CONN_TYPE_LOCAL_CLIENT 3

#define ZNET_AWAITING_ACK_CAPACITY 33
#define ZNET_RECEIVED_CAPACITY 33

#define ZNET_HANDLE_2_NET(ptrHandle) ZNet* net = (ZNet*)##ptrHandle##;

struct ZNetAckRecord
{
    u32 sequence;
    i32 acked;
};


// id of 0 == unassigned.
struct ZNetConnection
{
    i32 id;
    //i32 publicId; // Handle public ids outside of the connection

    i32 type;
    u32 sequence;
    u32 challenge;
    u32 remoteSequence; // also next ack
    //i32 salt;
    ZNetAddress remoteAddress;
    u32 flags;
    f32 timeSinceLastMessage;
    i32 pendingKeepAlive;
    i32 keepAliveSendTicks;
	i32 ticksSinceLastMessage;

    // Rolling buffer, constantly overwrites old entries
    // access with sequence % capacity
    // packet sequences that fall out of this buffer before
    // ack are considered lost
    ZNetAckRecord awaitingAck[ZNET_AWAITING_ACK_CAPACITY];

    // packets received for constructing ack bits
    u32 received[ZNET_RECEIVED_CAPACITY];
};

#define ZNET_PROTOCOL 90
/*
Packet structure:
> Packet header:
    > u32 protocol magic number
    > i32 payload checksum
> Payload header
    > u8 type
    > i32 sender Id
    > u16 sequence
    > 
    > data...

Data layout is different for each packet type
for most:
> client/server xor - IDs the connection.
*/

#define ZNET_PACKET_HEADER_SIZE sizeof(ZNetPacketHeader)

struct ZNetPacketHeader
{
    u32 protocol;
    i32 dataChecksum;

    void Read(u8** ptr)
    {
        *ptr += COM_COPY(*ptr, this, sizeof(ZNetPacketHeader));
    }
    
    i32 Read(u8* ptr)
    {
        return COM_COPY(ptr, this, sizeof(ZNetPacketHeader));
    }

    void Write(u8** ptr)
    {
        *ptr += COM_COPY(this, *ptr, sizeof(ZNetPacketHeader));
    }

    i32 Write(u8* ptr)
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
    u16 numBytes;
};

#define ZNET_MSG_TYPE_NONE 0
#define ZNET_MSG_TYPE_CONNECTION_REQUEST 1
#define ZNET_MSG_TYPE_CHALLENGE 2
#define ZNET_MSG_TYPE_CHALLENGE_RESPONSE 3
#define ZNET_MSG_TYPE_CONNECTION_APPROVED 4
#define ZNET_MSG_TYPE_CONNECTION_DENIED 5
#define ZNET_MSG_TYPE_KEEP_ALIVE 6
#define ZNET_MSG_TYPE_KEEP_ALIVE_RESPONSE 7
// Data messages are anything external to ZNet
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

struct ZNet;
//////////////////////////////////////////////////////////////
// Internal interface
//////////////////////////////////////////////////////////////
internal void ZNet_Send(ZNet*, ZNetAddress* address, u8* bytes, i32 numBytes);
internal void ZNet_SendActual(ZNet*, ZNetAddress* address, u8* bytes, i32 numBytes);
internal ZNetConnection* ZNet_GetConnectionById(ZNet* net, i32 id);

// Needs a net instance:

// Doesn't need a valid net instance:
internal void ZNet_CheckAcks(ZNet*, ZNetConnection* conn, u32 ack, u32 ackBits);
internal void ZNet_RecordPacketTransmission(ZNetConnection* conn, u32 sequence);
internal u32  ZNet_BuildAckBits(ZNetConnection* conn, u32 remoteSequence);
internal void ZNet_PrintAwaitingAcks(ZNetConnection* conn);

#include "znet_simulation.h"

#define MAX_CONNECTIONS 16
#define MAX_PENDING_CONNECTIONS 32
// assuming tick rate of 60, timeout after 30 seconds
// TODO: Switch to delta time so tick rate can vary!
#define MAX_PENDING_CONNECTION_TICKS 1800

struct ZNet
{
    // Handle MUST be at the top!
    ZNetHandle handle;
    ZNetConnection connections[MAX_CONNECTIONS];
	i32 isListening;
    i32 maxConnections = MAX_CONNECTIONS;
    i32 socketIndex;
    i32 localSocketIndex;
    i32 state;
    u16 selfPort;
    i32 client2ServerId;
    u32 tickCount;
    f32 secondsEllapsed;

    // incremented with each client
    // TODO: public and not used for any client <-> server commands but
    // should still be randomly selected?
    i32 nextPublicClientId;

    ZNetPlatformFunctions platform;
    ZNetOutputInterface output;
    ZNetDelayedPacketStore store;

    // Buffers
    // write data to this:
    u8 g_dataWriteBuffer[ZNET_MAX_PAYLOAD_SIZE];

    // final write buffer before call to platform send
    u8 g_packetWriteBuffer[ZNET_MAX_UDP_PACKET_SIZE];

    u8 g_packetReadBuffer[ZNET_MAX_UDP_PACKET_SIZE];

    ZNetPending pendingConnections[MAX_PENDING_CONNECTIONS];
};

/////////////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////////////

// Buffers
// write data to this:
internal u8 g_dataWriteBuffer[ZNET_MAX_PAYLOAD_SIZE];

// final write buffer before call to platform send
internal u8 g_packetWriteBuffer[ZNET_MAX_UDP_PACKET_SIZE];

internal u8 g_packetReadBuffer[ZNET_MAX_UDP_PACKET_SIZE];

//internal ZNetPlatformFunctions g_netPlatform;
//internal ZNetOutputInterface g_output;
//internal ZNet g_net;
//internal ZNetDelayedPacketStore g_store;

internal void Net_FatalError(ZNet* net, char* message, char* heading)
{
    if (net->platform.FatalError)
    {
        net->platform.FatalError(message, heading);
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
internal ByteBuffer ZNet_GetDataWriteBuffer()
{
	return Buf_FromBytes(g_dataWriteBuffer, ZNET_MAX_UDP_PACKET_SIZE);
}
internal ByteBuffer ZNet_GetPacketWriteBuffer()
{
	return Buf_FromBytes(g_packetWriteBuffer, ZNET_MAX_UDP_PACKET_SIZE);
}


#include "znet_packet.h"
#include "znet_messages.h"
#include "znet_connection.h"
#include "znet_pending.h"
#include "znet_main.h"
#include "znet_tests.h"

///////////////////////////////////////////////////
// system lifetime
///////////////////////////////////////////////////
void ZNet_Init(ZNetHandle* handle, ZNetPlatformFunctions platform, ZNetOutputInterface outputInterface, i32 simMode)
{
    printf("ZNet Initialising... ");
    Assert(handle->memSize == sizeof(ZNet))
    ZNET_HANDLE_2_NET(handle)
	COM_ZeroMemory((u8*)net, sizeof(ZNet));
    net->platform = platform;
	net->output = outputInterface;
    // testing conditions:
    switch (simMode)
    {
        case ZNET_SIM_MODE_REALISTIC:
        {
            net->store.Init(75, 125, 0.01f);
            printf("Sim conditions: realistic\n");
        } break;

        case ZNET_SIM_MODE_BAD:
        {
            net->store.Init(200, 400, 0.2f);
            printf("Sim conditions: bad\n");
        } break;

        case ZNET_SIM_MODE_TERRIBLE:
        {
            // biblical packet loss
            net->store.Init(500, 1000, 0.6f);
            printf("Sim conditions: terrible\n");
        } break;

        default:
        {
            net->store.Init(0, 0, 0);
            printf("No sim delay\n");
        } break;
    }
    // no lag, no packet loss
    
    
    printf("Done\n");
}

i32 ZNet_RequiredInstanceSize()
{
    return sizeof(ZNet);
}

void ZNet_Shutdown(ZNet* handle)
{
    ZNET_HANDLE_2_NET(handle)
    printf("ZNet shutting down... ");
    if (net->socketIndex != -1)
    {
        net->platform.CloseSocket(net->socketIndex);
        net->socketIndex = -1;
    }
    COM_ZeroMemory((u8*)net, sizeof(ZNet));
    net->platform.Shutdown();
    printf("Done\n");
}

// ZNET_MODULE_CPP
#endif
