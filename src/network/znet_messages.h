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
	ZNet_Send(net, &conn->remoteAddress, p.ptrStart, p.Written());
}

void ZNet_SendKeepAlive(ZNet* net, ZNetConnection* conn)
{
	ByteBuffer b = ZNet_GetDataWriteBuffer();
	b.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_KEEP_ALIVE, b.ptrWrite);
	b.ptrWrite += COM_WriteI32(conn->id, b.ptrWrite);
	ByteBuffer p = ZNet_GetPacketWriteBuffer();
	ZNet_BuildPacket(&p, b.ptrStart, b.Written(), NULL, 0);
	ZNet_Send(net, &conn->remoteAddress, p.ptrStart, p.Written());
}

void ZNet_SendDisconnectCommand(ZNet* net, ZNetConnection* conn, char* msg)
{
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
	ZNet_Send(net, &conn->remoteAddress, p.ptrStart, p.Written());

	// Inform higher level
	ZNetConnectionInfo info = {};
	info.address = conn->remoteAddress;
	info.id = conn->id;
	net->output.ConnectionDropped(&info);

	printf("done\n");
}

// TODO: This is probably bad! We are assuming the packet sequence retrieved
// here will be the next one sent!
// Chicken-and-egg problem: to retrieve the transmission record to fill in
// We need the sequence number of the packet. But ZNet only tells the caller
// AFTER the caller has invoked ZNet_SendData
u32 ZNet_GetNextSequenceNumber(ZNetHandle* handle, i32 connId)
{
	ZNet* net = (ZNet*)handle;
	Assert(handle->memSize == sizeof(ZNet))
	ZNetConnection* conn = ZNet_GetConnectionById(net, connId);
	NET_ASSERT(net, (connId != 0), "Get Next Packet Sequence: Connection Id is zero");
	NET_ASSERT(net, conn, "Get Next Packet Sequence: Connection could not be found\n");
	return conn->sequence;
}

u32 ZNet_SendData(ZNetHandle* handle, i32 connId, u8* data, u16 numBytes, i32 printSendInfo)
{
	ZNet* net = (ZNet*)handle;
	NET_ASSERT(net, (numBytes <= ZNET_MAX_PAYLOAD_SIZE), "ZNET Packet payload too large");
	
	//printf("ZNet send %d bytes to conn %d\n", numBytes, connId);
	ZNetConnection* conn = ZNet_GetConnectionById(net, connId);
	if (conn == NULL)
	{
		printf("No connection %d found for packet start\n", connId);
		return 0;
	}
	
	// protocol (4)
	// checksum (4)
	// type (1)
	// id (4)
	// sequence (4)
	// ack (4)
	// ack bitfield (4)
	// 				(25) total
	// payload	(max MTU - header)

	ByteBuffer b = ZNet_GetPacketWriteBuffer();
	b.ptrWrite += COM_WriteI32(ZNET_PROTOCOL, b.ptrWrite);

	// step over space for checksum, it must be done when packet is finished
	// checksum is of payload header + payload itself
	u8* ptrCheckSum = b.ptrWrite;
	b.ptrWrite += sizeof(i32);
	u8* payloadStart = b.ptrWrite;

	// write payload header
	b.ptrWrite += COM_WriteByte(ZNET_MSG_TYPE_DATA, b.ptrWrite);
	b.ptrWrite += COM_WriteI32(conn->id, b.ptrWrite);
	u32 packetNumber = conn->sequence++;
	b.ptrWrite += COM_WriteU32(packetNumber, b.ptrWrite);
	u32 ack = conn->remoteSequence;
	u32 ackBits = ZNet_BuildAckBits(conn, ack);
	
	b.ptrWrite += COM_WriteU32(ack, b.ptrWrite);
	b.ptrWrite += COM_WriteU32(ackBits, b.ptrWrite);
	
	// copy payload, write checksum, send, return sequence for external tracking
	b.ptrWrite += COM_COPY(data, b.ptrWrite, numBytes);
	u32 payloadSize = b.ptrWrite - payloadStart;
	COM_WriteI32(COM_SimpleHash(payloadStart, payloadSize), ptrCheckSum);
	//printf("ZNET Send data payload of %d bytes to %d\n", numBytes, conn->id);

	ZNet_Send(net, &conn->remoteAddress, b.ptrStart, b.Written());
	ZNet_RecordPacketTransmission(conn, packetNumber);
	switch (printSendInfo)
	{
		case 1:
		{
			printf("Attempting send of seq %u, ack %u, ackBits %u\n", conn->sequence - 1, ack, ackBits);
		} break;
	}
	//printf("\n");
	//ZNet_PrintAwaitingAcks(conn);
	//printf("\n");
	
	return packetNumber;
}
