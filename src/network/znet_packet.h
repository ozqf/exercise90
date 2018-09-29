#pragma once

#include "znet_module.cpp"


// Returns 0 if all is okay
internal i32 ZNet_ParsePacketHeader(u8* bytes, i32 numBytes, ZNetAddress* address, ZNetPacket* result)
{
    *result = {};

    u8* dataStart = bytes + sizeof(ZNetPacketHeader);
	u8* read = bytes;
	
    // TODO: Pulling out bytes directly. Replace me
	read += COM_COPY(read, &result->header, sizeof(ZNetPacketHeader));
	if (result->header.protocol != ZNET_PROTOCOL)
	{
        printf("SV protocol expected %d but got %d\n",
            ZNET_PROTOCOL, result->header.protocol
        );
		return 1;
	}

    i32 packetBytes = numBytes - sizeof(ZNetPacketHeader);
    i32 checkSum = COM_SimpleHash(read, packetBytes);
    if (result->header.dataChecksum != checkSum)
    {
        printf("SV read checksum %d but calculated %d\n",
            result->header.dataChecksum, checkSum
        );
        return 2;
    }

    result->address = *address;
    result->bytes = dataStart;
    result->numBytes = numBytes;

	return 0;
}

/*internal*/ i32 ZNet_BuildPacket(
    ByteBuffer* packetBuffer,
    u8* dataBytes,
    i32 numBytes,
    ZNetAddress* dest)
{
    i32 totalSize = sizeof(ZNetPacketHeader) + numBytes;
    NET_ASSERT(packetBuffer->capacity >totalSize, "Dataload too large for packet\n");
	// header
	ZNetPacketHeader h = {};
	h.protocol = ZNET_PROTOCOL;
	h.dataChecksum = COM_SimpleHash(dataBytes, numBytes);

    //packetBuffer->ptrWrite += COM_COPY(&h, packetBuffer->ptrWrite, sizeof(ZNetPacketHeader));
    packetBuffer->ptrWrite += h.Write(packetBuffer->ptrWrite);
    packetBuffer->ptrWrite += COM_COPY(dataBytes, packetBuffer->ptrWrite, numBytes);
    return (packetBuffer->ptrWrite - packetBuffer->ptrStart);
}
