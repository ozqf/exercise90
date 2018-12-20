#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include <conio.h>
#include <stdio.h>

#include "../../common/com_module.h"

#include "../../platform/win32_net/win32_net_interface.h"
#include "../../network/znet_interface.h"

#if 1
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>

// Example from:
// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-recvfrom
// https://www.binarytides.com/udp-socket-programming-in-winsock/
#endif

#include "../../App/app_network_stream.h"

/////////////////////////////////////////////////////////////////
// Entry point
/////////////////////////////////////////////////////////////////
#define TEST_SERVER_PORT 23232
#define TEST_CLIENT_PORT 61200

ErrorCode ReadReliableBuffer(u8* buf, i32 bytes)
{
	u8* cursor = buf;

	return COM_ERROR_NONE;
}

ErrorCode ReadPacketDescriptor(PacketDescriptor* packet, u8* buf, i32 size)
{
	*packet = {};
	packet->ptr = buf;
	packet->size = (u16)size;
	packet->cursor = packet->ptr;
	// Header
	packet->transmissionSimFrameNumber = COM_ReadU32(&packet->cursor);
	packet->transmissionSimTime = COM_ReadF32(&packet->cursor);
	packet->reliableOffset = COM_ReadU16(&packet->cursor);
	packet->unreliableOffset = COM_ReadU16(&packet->cursor);
	// deserialise sync check
	u8* checkPos = (buf + packet->unreliableOffset) - sizeof(u32);
	u32 check = *(u32*)checkPos;
	if (check != NET_DESERIALISE_CHECK)
	{
		return COM_ERROR_DESERIALISE_FAILED;
	}

	printf("Check: 0x%X\n", check);

	return COM_ERROR_NONE;
}

i32 WriteTestUnreliableData(u8* buf, u32 capacity)
{
	u8* cursor = buf;
	cursor += 12;
	return (cursor - buf);
}

#define TEST_RELIABLE_DATA_SIZE 15
i32 WriteTestReliableData(u8* buf, u32 capacity)
{
	u8* cursor = buf;
	// a sequence number
	cursor += COM_WriteU32(136, cursor);					// 4
	// a fake message with header
	cursor += COM_WriteU16(Stream_WritePacketHeader(0, 9), cursor); // 2
	// 9 bytes... 1 for type, 2 for some random values
	cursor += COM_WriteByte(127, cursor);					// 1
	cursor += COM_WriteI32(0x12341234, cursor);				// 4
	cursor += COM_WriteI32(0x43214321, cursor);				// 4
	// 4 + 2 + 1 + 4 + 4 == 15
	return (cursor - buf);
}

i32 CalcRemainingBytes(u8* bufferStart, u8* cursor, i32 totalCapacity)
{
	return totalCapacity - (cursor - bufferStart);
}

PacketDescriptor WriteTestPacket()
{
	PacketDescriptor packet = {};
	packet.size = 1400;
	packet.ptr = (u8*)malloc(packet.size);
	packet.cursor = packet.ptr;
	packet.transmissionSimFrameNumber = 300;
	packet.transmissionSimTime = 50;
	packet.deserialiseCheck = NET_DESERIALISE_CHECK;
	
	// Write header
	packet.cursor += COM_WriteU32(packet.transmissionSimFrameNumber, packet.cursor);
	packet.cursor += COM_WriteF32(packet.transmissionSimTime, packet.cursor);
	// record position for writing payload position offsets
	u8* payloadOffsetsPos = packet.cursor;
	// Write placeholders offsets - zeroes mean skip the given section
	packet.cursor += COM_WriteU16(0, packet.cursor);
	packet.cursor += COM_WriteU16(0, packet.cursor);
	
	// Write some test reliable data.
	u8* markReliableStart = packet.cursor;
	i32 reliableBytesWritten = WriteTestReliableData(
		packet.cursor, CalcRemainingBytes(packet.ptr, packet.cursor, packet.size));
		
	COM_ASSERT(reliableBytesWritten == TEST_RELIABLE_DATA_SIZE,
		"Unexpected amount of reliable data written\n")
	
	packet.cursor += reliableBytesWritten;
	packet.cursor += COM_WriteU32(packet.deserialiseCheck, packet.cursor);
	
	u8* markUnreliableStart = packet.cursor;
	
	i32 unreliableBytesWritten = WriteTestUnreliableData(
		packet.cursor, CalcRemainingBytes(packet.ptr, packet.cursor, packet.size));
	
	i32 reliableOffset = markReliableStart - packet.ptr;
	i32 unreliableOffset = markUnreliableStart - packet.ptr;
	i32 calculatedReliableSize = markUnreliableStart - markReliableStart - 4;
	
	payloadOffsetsPos += COM_WriteU16((u16)reliableOffset, payloadOffsetsPos);
	payloadOffsetsPos += COM_WriteU16((u16)unreliableOffset, payloadOffsetsPos);
	
	return packet;
}

void PrintPacketManifest(PacketDescriptor* packet)
{
	if (packet->ptr == NULL) { printf("Manifest cannot investigate a null packet"); return; }
	if (packet->size <= 0) { printf("Packet size of %d bytes is invalid\n", packet->size); return; }
}

void Test_NetworkWin32(i32 argc, char* argv[])
{
    COM_ASSERT(0, "Test");
	printf("Create packet test\n");
	PacketDescriptor in = WriteTestPacket();
	PacketDescriptor out;
	ErrorCode errorCode = ReadPacketDescriptor(&out, in.ptr, in.size);
	if (errorCode == COM_ERROR_NONE)
	{
		printf("Test completed\n");
	}
	else
	{
		printf("Deserialise error code: %d\n", errorCode);
	}	
	
	free((void*)in.ptr);
}
