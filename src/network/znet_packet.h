#pragma once

#include "znet_module.cpp"

internal void ZNet_DebugPacket(u8* packetStart, u16 packetSize)
{
    i32 packetHeaderSize = sizeof(ZNetPacketHeader);
    u8* payloadBytes = packetStart + packetHeaderSize;
    u16 payloadSize = (u16)(packetSize - packetHeaderSize);

    ZNetPacketHeader h = {};
    COM_COPY(packetStart, &h, sizeof(ZNetPacketHeader));

    printf("Packet size: %d (%d header, %d payload)\n", packetSize, sizeof(ZNetPacketHeader), payloadSize);

    printf("Protocol (%d) Bytes: ", h.protocol);
    COM_PrintBytes(packetStart, 4, 8);
    printf("Checksum (%d) Bytes: ", h.dataChecksum);
    COM_PrintBytes(packetStart + 4, 4, 8);
    i32 checkSum = COM_SimpleHash(payloadBytes, payloadSize);
    printf("Payload Hash (calc %d): ", checkSum);
    COM_PrintBytes((u8*)&checkSum, 4, 8);
}

// Returns 0 if all is okay
internal i32 ZNet_ParsePacketHeader(u8* bytes, u16 numBytes, ZNetAddress* address, ZNetPacket* result)
{
    *result = {};
	u8* read = bytes;
	
    // TODO: Pulling out bytes directly. Replace me
	read += COM_COPY(read, &result->header, sizeof(ZNetPacketHeader));
	if (result->header.protocol != ZNET_PROTOCOL)
	{
        printf("ZNET protocol expected %d but got %d\n",
            ZNET_PROTOCOL, result->header.protocol
        );
		return 1;
	}

    u8* payloadStart = bytes + sizeof(ZNetPacketHeader);
    u16 payloadBytes = numBytes - sizeof(ZNetPacketHeader);
    i32 checkSum = COM_SimpleHash(payloadStart, payloadBytes);
    if (result->header.dataChecksum != checkSum)
    {
        printf("ZNET read checksum %d but calculated %d\n",
            result->header.dataChecksum, checkSum
        );
        ZNet_DebugPacket(bytes, numBytes);
        return 2;
    }

    result->address = *address;
    result->bytes = payloadStart;
    //result->numBytes = numBytes;
    result->numBytes = payloadBytes;

	return 0;
}

internal i32 ZNet_BuildPacket(
    ByteBuffer* packetBuffer,
    u8* dataBytes,
    i32 numDataBytes,
    ZNetAddress* dest,
    i32 debug)
{
    i32 totalSize = sizeof(ZNetPacketHeader) + numDataBytes;
    //NET_ASSERT(packetBuffer->capacity >totalSize, "Dataload too large for packet\n");
	// header
	ZNetPacketHeader h = {};
	h.protocol = ZNET_PROTOCOL;
	h.dataChecksum = COM_SimpleHash(dataBytes, numDataBytes);
    //printf(" Outgoing Packet checksum: %d\n", h.dataChecksum);

    //packetBuffer->ptrWrite += COM_COPY(&h, packetBuffer->ptrWrite, sizeof(ZNetPacketHeader));
    packetBuffer->ptrWrite += h.Write(packetBuffer->ptrWrite);
    packetBuffer->ptrWrite += COM_COPY(dataBytes, packetBuffer->ptrWrite, numDataBytes);

    if (debug)
    {
        ZNet_DebugPacket(packetBuffer->ptrStart, (u16)packetBuffer->Written());
    }

    return (packetBuffer->ptrWrite - packetBuffer->ptrStart);
}
