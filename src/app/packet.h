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
    u32 transmissionTickNumber;
	f32 transmissionTime;
    u32 lastReceivedTickNumber;
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
