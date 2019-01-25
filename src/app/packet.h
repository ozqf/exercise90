#pragma once

#include "../common/com_module.h"



// Fully unpacked descriptor for use by client/server streams
struct PacketDescriptor
{
    u8* ptr;
	u8* cursor;
    i32 size;
	
	i32 id;

	i32 packetSequence;
	u32 ackSequence;
	u32 ackBits;
	
	u32 transmissionSimFrameNumber;
	f32 transmissionSimTime;
	i32 lastReceivedTickNumber;
	// if 0, no data
	// num bytes is offset gap to unreliable section - sync check size.
	i32 reliableOffset;
	i32 numReliableBytes;
	i32 deserialiseCheck;
	// if 0, no data.
	// num bytes is size of packet - offset.
	i32 unreliableOffset;
	i32 numUnreliableBytes;

	i32 Space()
	{
		return size - (cursor - ptr);
	}
};

// TODO: Replace this with a compacted version eventually
struct PacketHeader
{
	// TODO: This stuff
	// Packet header
	//i32 protocol
	//i32 checksum
	//u8 type;
	// private Id of the connection
	i32 id;
	
	// Packet reliability
	u32 packetSequence;
	u32 ackSequence;
	u32 ackBits;
	// timing
    i32 transmissionTickNumber;
	f32 transmissionTime;
    i32 lastReceivedTickNumber;
	// payload
    u16 numReliableBytes;
    u16 numUnreliableBytes;
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

internal void Packet_StartWrite(
	ByteBuffer* packet,
	i32 privateId,
	i32 packetSequence,
	u32 ackSequence,
	u32 ackBits,
	i32 simFrame,
	f32 time,
	i32 lastReceivedTickNumber)
{
	PacketHeader* h = (PacketHeader*)packet->ptrStart;
	*h = {};
	h->id = privateId;
	h->packetSequence = packetSequence;
	h->ackSequence = ackSequence;
	h->ackBits = ackBits;
	h->transmissionTickNumber = simFrame;
	h->transmissionTime = time;
	h->lastReceivedTickNumber = lastReceivedTickNumber;
	h->numReliableBytes = 0;
	h->numUnreliableBytes = 0;	
	packet->ptrWrite = packet->ptrStart + Packet_GetHeaderSize();
}

internal void Packet_FinishWrite(
	ByteBuffer* packet,
	i32 numReliableBytes,
	i32 numUnreliableBytes)
{
	PacketHeader* h = (PacketHeader*)packet->ptrStart;
	h->numReliableBytes = (u16)numReliableBytes;
	h->numUnreliableBytes = (u16)numUnreliableBytes;
}

// Return bytes written
internal i32 Packet_WriteFromStream(
	ByteBuffer* packet, ByteBuffer* stream, i32 maxBytes)
{
	u8* read = stream->ptrStart;
	u8* end = stream->ptrWrite;
	i32 written = 0;
	while (read < end)
	{
		Command* cmd = (Command*)read;
		Assert(Cmd_Validate(cmd) == COM_ERROR_NONE)
		read += cmd->size;
		if (cmd->size < maxBytes)
		{
			maxBytes -= cmd->size;
			written += cmd->size;
			packet->ptrWrite += COM_COPY(
				cmd, packet->ptrWrite, cmd->size);
		}
	}
	return written;
}

internal i32 Packet_WriteFromStream(
	NetStream* stream, NetStream* unreliableStream,
	u8* buf, i32 capacity, f32 ellapsed,
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
	printf("Packet reading %d bytes of input\n", (streamEnd - streamRead));
	while (streamRead < streamEnd)
	{
		Command* cmd = (Command*)streamRead;
		Assert(Cmd_Validate(cmd) == COM_ERROR_NONE)
		streamRead += cmd->size;
		//i32 space = (streamEnd - streamRead);
		if (cmd->size <= (payloadEnd - payloadStart))
		{
			printf("Packet copying command seq %d (%d bytes)\n",
				cmd->sequence, cmd->size);
			cursor += COM_COPY(cmd, cursor, cmd->size);
		}
	}
	h.numReliableBytes = (u16)(cursor - payloadStart);
	i32 reliableWritten = h.numReliableBytes;
	cursor += COM_WriteI32(COM_SENTINEL_B, cursor);

	h.numUnreliableBytes = 0;

	*(PacketHeader*)buf = h;
	
	printf("Packet Wrote %d reliable bytes\n", reliableWritten);
	i32 totalWritten = (cursor - buf);
	printf("Packet Wrote %d total bytes\n", totalWritten);
	return totalWritten;
}

internal i32 Packet_InitDescriptor(PacketDescriptor* packet, u8* buf, i32 numBytes)
{
	//printf("=== Build packet descriptor (%d bytes)===\n", numBytes);
	//COM_PrintBytesHex(buf, numBytes, 16);
	*packet = {};
	packet->ptr = buf;
	packet->size = numBytes;
	
	PacketHeader* h = (PacketHeader*)buf;
	packet->id = h->id;
	packet->packetSequence = h->packetSequence;
	packet->ackSequence = h->ackSequence;
	packet->ackBits = h->ackBits;
	
	packet->numReliableBytes = h->numReliableBytes;
	packet->numUnreliableBytes = h->numUnreliableBytes;
	packet->reliableOffset = (i32)(Packet_GetHeaderSize());
	//printf("Reliable bytes: %d\n", packet->numReliableBytes);
	//printf("Unreliable bytes: %d\n", packet->numUnreliableBytes);
	
	i32 syncOffset  = (packet->reliableOffset + packet->numReliableBytes);
	i32* syncCheckCursor = (i32*)(buf + syncOffset);
	packet->deserialiseCheck = *syncCheckCursor;
	if (packet->deserialiseCheck != COM_SENTINEL_B)
	{
		*packet = {};
		printf("Deserialise check failed! Expected %X got %X\n",
			COM_SENTINEL_B, packet->deserialiseCheck);
		return COM_ERROR_DESERIALISE_FAILED;
	}

	return COM_ERROR_NONE;
}
