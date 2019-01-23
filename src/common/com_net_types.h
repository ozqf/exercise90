#pragma once

#include "../common/com_module.h"

struct ZNetAddress
{
    u16 ip4Bytes[4];
    u16 port;
};

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
	// Has allocated buffers
	i32 initialised;
    // latest reliable command from remote executed here
    u32             inputSequence;
    ByteBuffer      inputBuffer;

    // id of next reliable message sent to remote
    u32             outputSequence;
    ByteBuffer      outputBuffer;
    // the most recented remotely acknowledged message Id
    u32             ackSequence;
	
	AckStream		ackStream;

    TransmissionRecord transmissions[MAX_TRANSMISSION_RECORDS];
};
#if 0
struct PacketDescriptor
{
    u8* ptr;
	u8* cursor;
    u16 size;
	
	u32 transmissionSimFrameNumber;
	f32 transmissionSimTime;
	// if 0, no data
	// num bytes is offset gap to unreliable section - sync check size.
	u16 reliableOffset;
	u32 deserialiseCheck;
	// if 0, no data.
	// num bytes is size of packet - offset.
	u16 unreliableOffset;

    ByteBuffer reliableBuffer;
    ByteBuffer unreliableBuffer;
	
	i32 Space()
	{
		return size - (cursor - ptr);
	}
};
#endif
ZNetAddress COM_LocalHost(u16 port);
void COM_InitStream(NetStream* stream, ByteBuffer input, ByteBuffer output);
