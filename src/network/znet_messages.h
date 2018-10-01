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
	ZNet_BuildPacket(&p, b.ptrStart, b.Written(), NULL);
	ZNet_Send(&conn->remoteAddress, p.ptrStart, p.Written());
}

void ZNet_SendKeepAlive(ZNet* net, ZNetConnection* conn)
{
	ByteBuffer b = ZNet_GetDataWriteBuffer();
	b.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_KEEP_ALIVE, b.ptrWrite);
	b.ptrWrite += COM_WriteI32(conn->id, b.ptrWrite);
	ByteBuffer p = ZNet_GetPacketWriteBuffer();
	ZNet_BuildPacket(&p, b.ptrStart, b.Written(), NULL);
	ZNet_Send(&conn->remoteAddress, p.ptrStart, p.Written());
}
