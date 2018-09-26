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

/*internal*/ void ZNet_BuildPacket(u8* dataBytes, i32 numBytes, ZNetAddress* dest, i32 salt)
{
	// header
	ZNetPacketHeader h = {};
	h.protocol = ZNET_PROTOCOL;
	h.dataChecksum = COM_SimpleHash(dataBytes, numBytes);
}
