#pragma once

#include "../common/com_module.h"

// Fully unpacked descriptor for use by client/server streams
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

	i32 Space()
	{
		return size - (cursor - ptr);
	}
};

// TODO: Replace this with a compacted version eventually
struct PacketHeader
{
    i32 transmissionTickNumber;
	f32 transmissionTime;
    i32 lastReceivedTickNumber;
    u16 reliableOffset;
    u16 unreliableOffset;
};

internal i32 Packet_GetHeaderSize()
{
    return sizeof(PacketHeader);
}

internal void Packet_ReadHeader(u8* ptr, PacketHeader* h)
{
    *h = *(PacketHeader*)ptr;
}

// Returns bytes written
internal i32 Packet_WriteHeader(u8* ptr, PacketHeader* h)
{
    *(PacketHeader*)ptr = *h;
	return sizeof(PacketHeader);
}

internal i32 Packet_WriteFromStream(
	NetStream* stream, u8* buf, i32 capacity, f32 ellapsed,
	i32 tickNumber, i32 lastReceivedTick)
{
	PacketHeader h = {};
	h.transmissionTickNumber = tickNumber;
	h.transmissionTime = ellapsed;
	h.lastReceivedTickNumber = lastReceivedTick;
	
	u8* payloadStart = buf + Packet_GetHeaderSize();
	u8* payloadEnd = buf + capacity;
	u8* cursor = payloadStart;
	
	// iterate reliable output buffer, loading as many commands as possible
	u8* streamRead = stream->outputBuffer.ptrStart;
	u8* streamEnd = stream->outputBuffer.ptrWrite;
	while (streamRead < streamEnd)
	{
		Command* cmd = (Command*)streamRead;
		Assert(Cmd_Validate(cmd) == COM_ERROR_NONE)
		streamRead += cmd->size;
		if (cmd->size <= (streamEnd - streamRead))
		{
			cursor += COM_COPY(cmd, cursor, cmd->size);
		}
	}
	return (cursor - buf);
}
