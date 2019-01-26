#pragma
/*
Read/Write packets for client
*/
#include "../../common/com_module.h"
#include "../packet.h"
#include "../../interface/sys_events.h"

internal i32 CL_WriteUnreliableSection(ByteBuffer* packet)
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

internal void CL_WritePacket()
{
    #if 1
	printf("CL Write packet for user %d\n", g_ids.privateId);
	//Stream_EnqueueOutput(&user->reliableStream, &ping.header);
	
	// enqueue
	//ByteBuffer* buf = App_GetLocalClientPacketForWrite();
	
	u8 buf[1400];
    ByteBuffer packet = Buf_FromBytes(buf, 1400);
    u32 sequence = g_acks.outputSequence++;
    u32 ack = g_acks.remoteSequence;
    u32 ackBits = Ack_BuildOutgoingAckBits(&g_acks);

    Packet_StartWrite(&packet, g_ids.privateId, sequence, ack, ackBits, 0, 0, 0);
    packet.ptrWrite += COM_WriteI32(COM_SENTINEL_B, packet.ptrWrite);
    i32 unreliableWritten = CL_WriteUnreliableSection(&packet);
    Packet_FinishWrite(&packet, 0, unreliableWritten);
    i32 total = packet.Written();
	
	Ack_RecordPacketTransmission(&g_acks, ack);
    App_SendTo(0, &g_serverAddress, buf, total);
    
	//Packet_WriteFromStream(
    //    &user->reliableStream, &user->unreliableStream, buf, 1400, g_elapsed, g_ticks, 0);
    #endif
}

internal i32 CL_ReadPacketReliableInput(ByteBuffer* buf, NetStream* stream)
{
    u8* read = buf->ptrStart;
    u8* end = buf->ptrWrite;
    while (read < end)
    {
        Command* h = (Command*)read;
        i32 err = Cmd_Validate(h);
        if (err != COM_ERROR_NONE)
        {
            return err;
        }
        Assert(!Cmd_Validate(h))
        read += h->size;
        Stream_EnqueueInput(stream, h);
    }
    return COM_ERROR_NONE;
}

internal i32 CL_ReadPacket(SysPacketEvent* ev, NetStream* reliableStream)
{
    // -- Descriptor --
    i32 headerSize = sizeof(SysPacketEvent);
    i32 dataSize = ev->header.size - headerSize;
    u8* data = (u8*)(ev) + headerSize;
    printf("CL %d Packet bytes from %d\n", dataSize, ev->sender.port);
	
    PacketDescriptor p;
    i32 err = Packet_InitDescriptor(
        &p, data, dataSize);
	if (err != COM_ERROR_NONE)
	{
		printf("  Error %d deserialising packet\n", err);
		return COM_ERROR_DESERIALISE_FAILED;
	}
    printf("  Seq %d Tick %d Time %.3f\n",
        p.packetSequence,
        p.transmissionSimFrameNumber,
        p.transmissionSimTime);
    
    // -- ack --
    Ack_RecordPacketReceived(&g_acks, p.packetSequence);
	u32 packetAcks[ACK_RESULTS_CAPACITY]; 
	i32 numPacketAcks = Ack_CheckIncomingAcks(
		&g_acks, p.ackSequence, p.ackBits, packetAcks
	);
	Stream_ProcessPacketAcks(reliableStream, packetAcks, numPacketAcks);
    
    // -- reliable section --
    // TODO: Put this byte buffer on the descriptor and initialise it there
    // to save all this faff in multiple places
    ByteBuffer reliableSection = {};
    printf("  Reliable bytes: %d\n", p.numReliableBytes);
    reliableSection.ptrStart = data + Packet_GetHeaderSize();
    reliableSection.ptrWrite = reliableSection.ptrStart + p.numReliableBytes;
    reliableSection.ptrEnd = reliableSection.ptrWrite;
    reliableSection.capacity = p.numReliableBytes;
    CL_ReadPacketReliableInput(&reliableSection, reliableStream);
    //Cmd_PrintBuffer(reliableSection.ptrStart, reliableSection.Written());
	
	// -- unreliable section --
	
    return COM_ERROR_NONE;
}
