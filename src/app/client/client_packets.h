#pragma
/*
Read/Write packets for client
*/
#include "../../common/com_module.h"
#include "../packet.h"
#include "../../interface/sys_events.h"

internal void CL_LogCommandBuffer(ByteBuffer* b, char* label)
{
    APP_LOG(64, "CL %s contents\n", label);
    u8* read = b->ptrStart;
    u8* end = b->ptrWrite;
    while (read < end)
    {
        Command* cmd = (Command*)read;
        i32 err = Cmd_Validate(cmd);
        if (err != COM_ERROR_NONE)
        {
            APP_LOG(64, "  cmd validate failed with code %d\n", err);
            return;
        }
        read += cmd->size;
        APP_LOG(128, "  Type %d Tick %d Size %d\n",
            cmd->type, cmd->tick, cmd->size);
    }
}

internal i32 CL_WriteUnreliableSection(ByteBuffer* packet, C2S_Input* userInput)
{
    u8* start = packet->ptrWrite;
    // Send ping
	CmdPing ping = {};
	// TODO: Stream enqueue will set the sequence for us
	// so remove sending 0 here.
	Cmd_Prepare(&ping.header, g_ticks, 0);
	ping.sendTime = g_elapsed;
    packet->ptrWrite += COM_COPY(&ping, packet->ptrWrite, ping.header.size);
	
	// TODO: Encode userInput
	packet->ptrWrite += COM_COPY(
		userInput, packet->ptrWrite, userInput->header.size);
	
    return (packet->ptrWrite - start);
}

internal void CL_WritePacket(f32 time, C2S_Input* userInput)
{
    #if 1
	//printf("CL Write packet for user %d\n", g_ids.privateId);
	//Stream_EnqueueOutput(&user->reliableStream, &ping.header);
	
	// enqueue
	//ByteBuffer* buf = App_GetLocalClientPacketForWrite();
	
	u8 buf[1400];
    ByteBuffer packet = Buf_FromBytes(buf, 1400);
    u32 packetSequence = g_acks.outputSequence++;
	Ack_RecordPacketTransmission(&g_acks, packetSequence, time);
	
    u32 ack = g_acks.remoteSequence;
    u32 ackBits = Ack_BuildOutgoingAckBits(&g_acks);
    Packet_StartWrite(&packet, g_ids.privateId, packetSequence, ack, ackBits, 0, 0, 0);
	
	//TransmissionRecord* rec = Stream_AssignTransmissionRecord(
	//	g_reliableStream.transmissions, packetSequence);
    packet.ptrWrite += COM_WriteI32(COM_SENTINEL_B, packet.ptrWrite);
    i32 unreliableWritten = CL_WriteUnreliableSection(&packet, userInput);
    Packet_FinishWrite(&packet, 0, unreliableWritten);
    i32 total = packet.Written();
	
    App_SendTo(0, &g_serverAddress, buf, total);
    
	//Packet_WriteFromStream(
    //    &user->reliableStream, &user->unreliableStream, buf, 1400, g_elapsed, g_ticks, 0);
    #endif
}

internal i32 CL_ReadPacketUnreliableInput(
    ByteBuffer* buf, NetStream* stream)
{
    //CL_LogCommandBuffer(buf, "Unreliable packet section");
    u8* read = buf->ptrStart;
    u8* end = buf->ptrWrite;
    APP_LOG(128, "CL Reading %d unreliable bytes\n", (end -read));
    while (read < end)
    {
        Command* h = (Command*)read;
        i32 err = Cmd_Validate(h);
        if (err != COM_ERROR_NONE)
        {
            return err;
        }
        read += h->size;
        i32 wrote = Stream_EnqueueUnreliableInput(stream, h);
        if (wrote)
        {
            //APP_LOG(128, "CL Enqueue unreliable type %d size %d\n",
            //    h->type, wrote);
        }
    }
    return COM_ERROR_NONE;
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
        Stream_EnqueueReliableInput(stream, h);
    }
    return COM_ERROR_NONE;
}

internal i32 CL_ReadPacket(
    SysPacketEvent* ev,
    NetStream* reliableStream,
    NetStream* unreliableStream,
    f32 time)
{
    // -- Descriptor --
    i32 headerSize = sizeof(SysPacketEvent);
    i32 dataSize = ev->header.size - headerSize;
    u8* data = (u8*)(ev) + headerSize;
    //printf("CL %d Packet bytes from %d\n", dataSize, ev->sender.port);
	
    PacketDescriptor p;
    i32 err = Packet_InitDescriptor(
        &p, data, dataSize);
	if (err != COM_ERROR_NONE)
	{
		printf("  Error %d deserialising packet\n", err);
		return COM_ERROR_DESERIALISE_FAILED;
	}
    /*printf("  Seq %d Tick %d Time %.3f\n",
        p.packetSequence,
        p.transmissionSimFrameNumber,
        p.transmissionSimTime);*/
    
    // -- ack --
    Ack_RecordPacketReceived(&g_acks, p.packetSequence);
	u32 packetAcks[ACK_RESULTS_CAPACITY]; 
	i32 numPacketAcks = Ack_CheckIncomingAcks(
		&g_acks, p.ackSequence, p.ackBits, packetAcks, time
	);
	Stream_ProcessPacketAcks(reliableStream, packetAcks, numPacketAcks);
    
    // -- reliable section --
    // TODO: Put this byte buffer on the descriptor and initialise it there
    // to save all this faff in multiple places
    ByteBuffer reliableSection = {};
    //printf("  Reliable bytes: %d\n", p.numReliableBytes);
    reliableSection.ptrStart = data + Packet_GetHeaderSize();
    reliableSection.ptrWrite = reliableSection.ptrStart + p.numReliableBytes;
    reliableSection.ptrEnd = reliableSection.ptrWrite;
    reliableSection.capacity = p.numReliableBytes;
    CL_ReadPacketReliableInput(&reliableSection, reliableStream);
    //Cmd_PrintBuffer(reliableSection.ptrStart, reliableSection.Written());
	
	// -- unreliable section --
    ByteBuffer unreliableSection = {};
    unreliableSection.ptrStart = data + p.unreliableOffset;
    unreliableSection.ptrWrite =
        unreliableSection.ptrStart +
        p.numUnreliableBytes;
    unreliableSection.ptrEnd = unreliableSection.ptrWrite;
    err = CL_ReadPacketUnreliableInput(&unreliableSection, unreliableStream);
    if (err)
    {
        APP_PRINT(64, "CL err %d reading unreliable", err) 
    }

    return COM_ERROR_NONE;
}
