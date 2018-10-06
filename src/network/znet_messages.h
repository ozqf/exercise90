#pragma once

#include "znet_module.cpp"

/*
General structure:

-- Connection Request --
- Header -
Type - Byte
ClientSalt
ServerSalt

- Header -
Type - Byte
ClientSalt
ServerSalt

Data

*/

// Client2Server
struct ZNet_ConnectionRequest
{
	u32 clientSalt;
};

// Server2Client
struct ZNet_ConnectionChallenge
{
	// the XOR of these two values HAS to be unique for client (potential or not).
	// instead of asking the client to reroll their salt, the server should reroll
	// it's salt until the XOR is unique.
	u32 clientSalt;
	u32 serverSalt;
};

// Client2Server
struct ZNet_ChallengeResponse
{
	u32 saltXOR;
};

struct ZNet_ConnectionAccepted
{
	u32 saltXOR;
};

struct ZNet_StandardPacketHeader_Thing_This_Name_Should_Be_Replaced
{
	u32 saltXOR;
};

// returns number of bytes written
i32 ZNet_WriteConnRequest(ByteBuffer* b, i32 clientSalt)
{
	u8* ptr = b->ptrWrite;
	b->ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_CONNECTION_REQUEST, b->ptrWrite);
	b->ptrWrite += COM_WriteI32(clientSalt, b->ptrWrite);
	return b->ptrWrite - ptr;
}

// returns number of bytes written
i32 ZNet_WriteChallengeResponse(ByteBuffer* b, i32 response)
{
	u8* ptr = b->ptrWrite;
	b->ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_CHALLENGE_RESPONSE, b->ptrWrite);
	b->ptrWrite += COM_WriteI32(response, b->ptrWrite);
	return b->ptrWrite - ptr;
}

void ZNet_SendConnectionApproved(ZNet* net, ZNetConnection* conn)
{
	ByteBuffer b = ZNet_GetDataWriteBuffer();
	b.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_CONNECTION_APPROVED, b.ptrWrite);
	b.ptrWrite += COM_WriteI32(conn->id, b.ptrWrite);
	ByteBuffer p = ZNet_GetPacketWriteBuffer();
	ZNet_BuildPacket(&p, b.ptrStart, b.Written(), NULL, 0);
	ZNet_Send(&conn->remoteAddress, p.ptrStart, p.Written());
}

void ZNet_SendKeepAlive(ZNet* net, ZNetConnection* conn)
{
	ByteBuffer b = ZNet_GetDataWriteBuffer();
	b.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_KEEP_ALIVE, b.ptrWrite);
	b.ptrWrite += COM_WriteI32(conn->id, b.ptrWrite);
	ByteBuffer p = ZNet_GetPacketWriteBuffer();
	ZNet_BuildPacket(&p, b.ptrStart, b.Written(), NULL, 0);
	ZNet_Send(&conn->remoteAddress, p.ptrStart, p.Written());
}

void ZNet_SendDisconnectCommand(ZNet* net, ZNetConnection* conn, char* msg)
{
	ZNetConnectionInfo info = {};
	info.address = conn->remoteAddress;
	info.id = conn->id;
	g_output.ConnectionDropped(&info);

	ByteBuffer b = ZNet_GetDataWriteBuffer();
	b.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_CONNECTION_DENIED, b.ptrWrite);
	b.ptrWrite += COM_WriteI32(conn->id, b.ptrWrite);
	i32 len = COM_StrLen(msg);
	printf("Writing disconnect msg: \"%s\" (%d chars)\n", msg, len);
	b.ptrWrite += COM_WriteI32(len, b.ptrWrite);
	do
	{
		char c = *msg;
		printf("%c, ", c);
		*b.ptrWrite = (u8)c;
		msg++;
		len--;
		b.ptrWrite++;
	} while (len);
	printf("\nWrote msg bytes: %d...", b.Written());
	ByteBuffer p = ZNet_GetPacketWriteBuffer();
	ZNet_BuildPacket(&p, b.ptrStart, b.Written(), NULL, 0);
	ZNet_Send(&conn->remoteAddress, p.ptrStart, p.Written());
	printf("done\n");
}

void ZNet_SendData(i32 connId, u8* data, u16 numBytes)
{
	ZNet* net = &g_net;
	//printf("ZNet send %d bytes to conn %d\n", numBytes, connId);
	ZNetConnection* conn = ZNet_GetConnectionById(net, connId);
	NET_ASSERT(conn, "No connection found for data transmission\n");
	i32 dataHeaderSize = 1 + 4 + 2;
	NET_ASSERT((numBytes < (ZNET_DATA_WRITE_SIZE + dataHeaderSize)), "Packet data too large\n");

	// TODO: Teehee well this is a lot of unnecessary copying.
	// external data buffer -> internal data buffer -> packet buffer -> send
	//printf("  Sending %d byte data packet to conn %d\n", numBytes, connId);

	ByteBuffer b = ZNet_GetDataWriteBuffer();
	b.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_DATA, b.ptrWrite);
	b.ptrWrite += COM_WriteI32(conn->sequence, b.ptrWrite);
	conn->sequence++;
	b.ptrWrite += COM_COPY(data, b.ptrWrite, numBytes);
	/*
	- Test data packet contents -
	1 byte for type
	4 bytes for sequence
	1 byte of data
	*/
	
	ByteBuffer p = ZNet_GetPacketWriteBuffer();
	ZNet_BuildPacket(&p, b.ptrStart, b.Written(), NULL, 0);

	ZNet_Send(&conn->remoteAddress, p.ptrStart, p.Written());
}
