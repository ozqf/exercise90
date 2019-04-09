#pragma once

#include "server.h"

internal i32 SV_WriteUnreliableSection(User* user, ByteBuffer* packet)
{
    i32 capacity = packet->Space();
    u8* start = packet->ptrWrite;
    // Send ping
	//CmdPing ping = {};
	// TODO: Stream enqueue will set the sequence for us
	// so remove sending 0 here.
    //Cmd_InitPing(&ping, g_ticks, 0, g_elapsed);
    //packet->ptrWrite += COM_COPY(
    //    &ping, packet->ptrWrite, ping.header.size);

    // send input confirmation
    SimEntity* avatar = Sim_GetEntityBySerial(&g_sim, user->entSerial);
    Vec3 pos = {};
    if (avatar) { pos = avatar->previousPos; }
    S2C_InputResponse response = {};
    Cmd_InitInputResponse(
        &response,
        g_ticks,
        user->userInputSequence,
        pos
        );
    packet->ptrWrite += COM_COPY(
        &response, packet->ptrWrite, response.header.size);
    APP_LOG(128, "SV Write ent sync for: ");
	i32 syncMessagesWritten = 0;
	for (i32 i = 0; i < g_sim.maxEnts; ++i)
	{
		SimEntity* ent = &g_sim.ents[i];
		if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        if (ent->isLocal) { continue; }
		
		if (packet->Space() < sizeof(S2C_EntitySync))
		{
			break;
		}
        APP_LOG(32, "%d, ", ent->id.serial);
        if (ent->flags & SIM_ENT_FLAG_POSITION_SYNC)
        {
            S2C_EntitySync cmd = {};
            Cmd_WriteEntitySync(&cmd, g_ticks, 0, ent);
            packet->ptrWrite += COM_COPY(
                &cmd, packet->ptrWrite, cmd.header.size);
            //APP_LOG(128, "SV Wrote ent %d sync\n", ent->id.serial);
		    syncMessagesWritten++;
        }
		/*switch (ent->tickType)
		{
            case SIM_TICK_TYPE_ACTOR:
			case SIM_TICK_TYPE_WANDERER:
			{
                S2C_EntitySync cmd = {};
                Cmd_WriteEntitySync(&cmd, g_ticks, 0, ent);
                packet->ptrWrite += COM_COPY(
                    &cmd, packet->ptrWrite, cmd.header.size);
                //APP_LOG(128, "SV Wrote ent %d sync\n", ent->id.serial);
				syncMessagesWritten++;
			} break;
		}*/
	}
    APP_LOG(8, "\n");
    i32 written = (packet->ptrWrite - start);
    //APP_LOG(128, "SV Wrote %d sync messages (%d bytes of %d capacity)\n",
    //    syncMessagesWritten, written, capacity);
    return written;
}

internal i32 SV_WriteReliableSection(
    User* user, ByteBuffer* packet, i32 capacity, TransmissionRecord* rec)
{
    i32 space = capacity;
    ByteBuffer* cmds = &user->reliableStream.outputBuffer;
    u8* read = cmds->ptrStart;
    u8* end = cmds->ptrWrite;
    i32 numCommandsNotWritten = 0;
    while(read < end)
    {
        Command* cmd = (Command*)read;
        Assert(Cmd_Validate(cmd) == COM_ERROR_NONE)
        i32 size = cmd->size;
        read += size;
        if (cmd->size > space) { numCommandsNotWritten++; continue; }
        
        packet->ptrWrite += COM_COPY(cmd, packet->ptrWrite, size);
        space -= size;
		
		// Record message
		rec->reliableMessageIds[rec->numReliableMessages++] = cmd->sequence;
		Assert(rec->numReliableMessages < MAX_PACKET_TRANSMISSION_MESSAGES)
    }
    if (numCommandsNotWritten > 0)
    {
        APP_LOG(128, "SV no space for %d commands to user %d (%d bytes in output)\n",
            numCommandsNotWritten, user->ids.privateId, cmds->Written());
    }
    return (capacity - space);
}

internal void SV_WriteUserPacket(User* user, f32 time)
{
    #if 1
	//printf("SV Write packet for user %d\n", user->ids.privateId);
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
	Ack_RecordPacketTransmission(&user->acks, packetSequence, time);

	u8 buf[packetSize];
    ByteBuffer packet = Buf_FromBytes(buf, packetSize);
	
	// -- header --
	u32 ack = user->acks.remoteSequence;
    u32 ackBits = Ack_BuildOutgoingAckBits(&user->acks);
    Packet_StartWrite(&packet, user->ids.privateId, packetSequence, ack, ackBits, 0, 0, 0);
	
	// -- record packet payload and load reliable commands -- 
	TransmissionRecord* rec = Stream_AssignTransmissionRecord(
		user->reliableStream.transmissions, packetSequence);
    
    i32 reliableWritten = SV_WriteReliableSection(user, &packet, reliableAllocation, rec);
	//printf("  Reliable wrote %d bytes of %d allowed\n", reliableWritten, reliableAllocation);

    // -- write mid-packet deserialise check and unreliable sync data -- 
    packet.ptrWrite += COM_WriteI32(COM_SENTINEL_B, packet.ptrWrite);
    i32 unreliableWritten = SV_WriteUnreliableSection(user, &packet);
    //i32 unreliableWritten = 0;
    if (unreliableWritten > 0)
    {
        APP_LOG(128, "SV Wrote %d unreliable bytes\n", unreliableWritten);
    }
    
	// -- Finish --
    Packet_FinishWrite(&packet, reliableWritten, unreliableWritten);
    i32 total = packet.Written();
    App_SendTo(0, &user->address, buf, total);
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

internal void SV_ReadUnreliableSection(User* user, ByteBuffer* b)
{
    u8* read = b->ptrStart;
    u8* end = b->ptrWrite;
    while (read < end)
    {
        Command* header = (Command*)read;
        i32 err = Cmd_Validate(header);
        if (err)
        {
            APP_LOG(128, "SV read unreliable cmd failed %d\n", err);
            return;
        }
        read += header->size;
        switch (header->type)
        {
            case CMD_TYPE_C2S_INPUT:
            {
                C2S_Input* cmd = (C2S_Input*)header;
                if (cmd->userInputSequence > user->userInputSequence)
                {
                    user->userInputSequence = cmd->userInputSequence;
                }
                if (cmd->header.tick > user->latestServerTick)
                {
                    user->latestServerTick = cmd->header.tick;
                }
                Sim_SetActorInput(&g_sim, &cmd->input, user->entSerial);
            } break;

            default:
            {
                APP_LOG(64, "SV Unknown unreliable cmd type %d\n",
                    header->type);
            } break;
        }
    }
}

internal void SV_ReadPacket(SysPacketEvent* ev, f32 time)
{
	i32 headerSize = sizeof(SysPacketEvent);
    i32 dataSize = ev->header.size - headerSize;
    u8* data = (u8*)(ev) + headerSize;
    APP_LOG(64, "SV Read %d Packet bytes from %d\n", dataSize, ev->sender.port);

    PacketDescriptor p;
    i32 err = Packet_InitDescriptor(
        &p, data, dataSize);
	if (err != COM_ERROR_NONE)
	{
		printf("  Error %d deserialising packet\n", err);
		return;
	}
    /*printf("  Seq %d Tick %d Time %.3f\n",
        p.packetSequence,
        p.transmissionSimFrameNumber,
        p.transmissionSimTime);*/
	ev->header.type = SYS_EVENT_SKIP;

    User* user = User_FindByPrivateId(&g_users, p.id);
    if (user)
    {
        //printf("\tSV packet from user %d\n", p.id);
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
        &user->acks, p.ackSequence, p.ackBits, packetAcks, time);
	
	Stream_ProcessPacketAcks(
        &user->reliableStream, packetAcks, numPacketAcks);
	
	// -- reliable section --
	
	
	// -- unreliable section --
	ByteBuffer b = {};
    b.ptrStart = data + p.unreliableOffset;
    b.ptrWrite = b.ptrStart + p.numUnreliableBytes;
    b.ptrEnd = b.ptrWrite;
    
    SV_ReadUnreliableSection(user, &b);
}
