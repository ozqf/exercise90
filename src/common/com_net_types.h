#pragma once

#include "../common/com_module.h"

struct ZNetAddress
{
    u16 ip4Bytes[4];
    u16 port;
};

inline ZNetAddress COM_LocalHost(u16 port)
{
    ZNetAddress addr;
    addr.ip4Bytes[0] = 127;
    addr.ip4Bytes[1] = 0;
    addr.ip4Bytes[2] = 0;
    addr.ip4Bytes[3] = 1;
    addr.port = port;
    return addr;
}

struct StreamMsgHeader
{
    u32 id;
    u32 size;
};

#define NET_DESERIALISE_CHECK 0xDEADBEEF

// (u16 - num reliable bytes) (u32 - first message Id)
#define NET_SIZE_OF_RELIABLE_HEADER (sizeof(u16) + sizeof(u32))

// (u16 -num reliable bytes)
#define NET_SIZE_OF_UNRELIABLE_HEADER (sizeof(u16))

#define MAX_PACKET_TRANSMISSION_MESSAGES 64
struct TransmissionRecord
{
	u32 sequence;
	u32 numReliableMessages;
	u32 reliableMessageIds[MAX_PACKET_TRANSMISSION_MESSAGES];
};

#define MAX_TRANSMISSION_RECORDS 33
struct NetStream
{
    // latest reliable command from remote executed here
    u32 inputSequence;
    // id of next reliable message sent to remote
    u32 outputSequence;
    // the most recented acknowledged message Id
    u32 ackSequence;

    ByteBuffer inputBuffer;
    ByteBuffer outputBuffer;
    
    TransmissionRecord transmissions[MAX_TRANSMISSION_RECORDS];
};
