#pragma once

#include "server.h"

#if 0
internal void SV_EnqueueReliableOutput(User* user, Command* cmd)
{
    ByteBuffer* b = &user->reliableStream.outputBuffer;
    Assert(b->Space() >= cmd->size)
    b->ptrWrite += COM_CopyMemory((u8*)cmd, b->ptrWrite, cmd->size);
}

/*
i32                 Stream_BufferMessage(
                        ByteBuffer* b, u32 msgId, u8* bytes, i32 numBytes)
internal void       Stream_Clear(NetStream* stream)
i32                 Stream_WriteStreamMsgHeader(
                        u8* ptr, u32 msgId, i32 numBytes, f32 resendRateSeconds);
u16                 Stream_WritePacketHeader(u8 sequenceOffset, u16 size)
void                Stream_ReadPacketHeader(
                        u16 header, u8* sequenceOffset, u16* size)
TransmissionRecord* Stream_AssignTransmissionRecord(
                        TransmissionRecord* records,
                        u32 sequence)
TransmissionRecord* Stream_FindTransmissionRecord(
                        TransmissionRecord* records,
                        u32 sequence)
void                Stream_PrintPacketManifest(u8* bytes, u16 numBytes)
StreamMsgHeader*    Stream_FindMessageById(u8* read, u8* end, u32 id)
u8*                 Stream_CollapseBlock(
                        u8* blockStart,
                        u32 blockSpace,
                        u8* bufferEnd)
u32                 Stream_ClearReceivedOutput(
                        NetStream* stream, u32 packetSequence)
void                Stream_OutputToPacket(
                        i32 connId,
                        NetStream* s,
                        ByteBuffer* packetBuf,
                        f32 deltaTime)
u8*                 Stream_PacketToInput(NetStream* s, u8* ptr)

*/
#endif
internal i32 SV_WriteUnreliableSection(User* user, ByteBuffer* packet)
{
    u8* start = packet->ptrWrite;
    // Send ping
	CmdPing ping = {};
	// TODO: Stream enqueue will set the sequence for us
	// so remove sending 0 here.
	Cmd_Prepare(&ping.header, g_ticks, 0);
	ping.sendTime = g_elapsed;
    packet->ptrWrite += COM_COPY(&ping, packet->ptrWrite, ping.header.size);
    return (packet->ptrWrite - start);
}

internal i32 SV_WriteReliableSection(
    User* user, ByteBuffer* packet, i32 capacity, TransmissionRecord* rec)
{
    i32 space = capacity;
    ByteBuffer* cmds = &user->reliableStream.outputBuffer;
    u8* read = cmds->ptrStart;
    u8* end = cmds->ptrWrite;
    while(read < end)
    {
        Command* cmd = (Command*)read;
        Assert(Cmd_Validate(cmd) == COM_ERROR_NONE)
        i32 size = cmd->size;
        read += size;
        if (cmd->size > space) { continue; }
        
        packet->ptrWrite += COM_COPY(cmd, packet->ptrWrite, size);
        space -= size;
		
		// Record message
		rec->reliableMessageIds[rec->numReliableMessages++] = cmd->sequence;
		Assert(rec->numReliableMessages < MAX_PACKET_TRANSMISSION_MESSAGES)
    }
    return (capacity - space);
}

internal void SV_WriteUserPacket(User* user)
{
    #if 1
	printf("SV Write packet for user %d\n", user->ids.privateId);
	//Stream_EnqueueOutput(&user->reliableStream, &ping.header);
	
	// enqueue
	//ByteBuffer* buf = App_GetLocalClientPacketForWrite();
	
	const i32 packetSize = 1400;
	// unreliable may use whatever space is remaining, but
	// we always want to send *some* unreliable sync info.
	// so leave some space.
	const i32 reliableAllocation = 1000;
	
	// Record packet transmission for ack
	u32 packetSequence = user->acks.outputSequence++;
	TransmissionRecord* rec = Stream_AssignTransmissionRecord(
		user->reliableStream.transmissions, packetSequence);
    Ack_RecordPacketTransmission(&user->acks, packetSequence);
    u32 ack = user->acks.remoteSequence;
    u32 ackBits = Ack_BuildOutgoingAckBits(&user->acks);
	
	u8 buf[packetSize];
    ByteBuffer packet = Buf_FromBytes(buf, packetSize);
    Packet_StartWrite(&packet, user->ids.privateId, packetSequence, ack, ackBits, 0, 0, 0);
    i32 reliableWritten = SV_WriteReliableSection(user, &packet, reliableAllocation, rec);
	printf("  Reliable wrote %d bytes of %d allowed\n", reliableWritten, reliableAllocation);
    packet.ptrWrite += COM_WriteI32(COM_SENTINEL_B, packet.ptrWrite);
    i32 unreliableWritten = SV_WriteUnreliableSection(user, &packet);
	
	// Finished
    Packet_FinishWrite(&packet, reliableWritten, unreliableWritten);
    i32 total = packet.Written();
    App_SendTo(0, &user->address, buf, total);
    
	//Packet_WriteFromStream(
    //    &user->reliableStream, &user->unreliableStream, buf, 1400, g_elapsed, g_ticks, 0);
    #endif
}


internal void SV_WriteTestPacket()
{
    // Make a packet, no messages just a header
    u8 buf[1400];
    ByteBuffer b = Buf_FromBytes(buf, 1400);
    Packet_StartWrite(&b, 0, 0, 0, 0, g_ticks, g_elapsed, 0);
    b.ptrWrite += COM_WriteI32(COM_SENTINEL_B, b.ptrWrite);
    Packet_FinishWrite(&b, 0, 0);
    i32 written = b.Written();

    // loopback address
    ZNetAddress addr = {};
    addr.port = APP_CLIENT_LOOPBACK_PORT;

    App_SendTo(0, &addr, buf, written);
}

// internal void SV_ProcessPacketAcks(NetStream* stream, u32* packetAcks, i32 numPacketAcks)
// {
	// for (i32 i = 0; i < numPacketAcks; ++i)
	// {
		// Stream_ClearReceivedOutput(stream, packetAcks[i]);
	// }
// }

internal void SV_ReadPacket(SysPacketEvent* ev)
{
	i32 headerSize = sizeof(SysPacketEvent);
    i32 dataSize = ev->header.size - headerSize;
    u8* data = (u8*)(ev) + headerSize;
    printf("SV %d Packet bytes from %d\n", dataSize, ev->sender.port);

    PacketDescriptor p;
    i32 err = Packet_InitDescriptor(
        &p, data, dataSize);
	if (err != COM_ERROR_NONE)
	{
		printf("  Error %d deserialising packet\n", err);
		return;
	}
    printf("  Seq %d Tick %d Time %.3f\n",
        p.packetSequence,
        p.transmissionSimFrameNumber,
        p.transmissionSimTime);
	ev->header.type = SYS_EVENT_SKIP;

    User* user = User_FindByPrivateId(&g_users, p.id);
    if (user)
    {
        printf("\tSV packet from user %d\n", p.id);
    }
    else
    {
        printf("SV Couldn't find user %d for packet\n", p.id);
        return;
    }
	
	// -- Ack packets + commands --
    Ack_RecordPacketReceived(&user->acks, p.packetSequence);
    u32 packetAcks[ACK_RESULTS_CAPACITY];
    i32 numPacketAcks = Ack_CheckIncomingAcks(
        &user->acks, p.ackSequence, p.ackBits, packetAcks);
	
	Stream_ProcessPacketAcks(&user->reliableStream, packetAcks, numPacketAcks);
	
	// -- reliable section --
	
	
	// -- unreliable section --
	
}
