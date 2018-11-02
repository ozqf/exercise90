#pragma once

#include "test_network_win32.h"


//////////////////////////////////////////////////////////////////////
// SERVER
//////////////////////////////////////////////////////////////////////
void TNet_ServerSendChatMsg(i32 senderPublicId, char* msg, u16 numChars)
{
    // TODO: Buffer chat message into client output.
    #if 0
    ByteBuffer b = TNET_GetWriteBuffer();
    b.ptrWrite += COM_WriteByte(TNET_MSG_TYPE_S2C_CHAT, b.ptrWrite);
    b.ptrWrite += COM_WriteI32(senderPublicId, b.ptrWrite);
    b.ptrWrite += COM_WriteU16(numChars, b.ptrWrite);
    b.ptrWrite += COM_COPY(msg, b.ptrWrite, numChars);

    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }

        // build packet
        ZNet_SendData(cl->connId, b.ptrStart, (u16)b.Written(), 0);
    }
    #endif
}

void TNet_ServerSendClientList(i32 senderPublicId, char* msg, u16 numChars)
{
    #if 0
    ByteBuffer b = TNET_GetWriteBuffer();
    b.ptrWrite += COM_WriteByte(TNET_MSG_TYPE_S2C_CHAT, b.ptrWrite);
    b.ptrWrite += COM_WriteI32(senderPublicId, b.ptrWrite);
    b.ptrWrite += COM_WriteU16(numChars, b.ptrWrite);
    b.ptrWrite += COM_COPY(msg, b.ptrWrite, numChars);

    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }

        // build packet
        ZNet_SendData(cl->connId, b.ptrStart, (u16)b.Written(), 0);
    }
    #endif
}

void TNet_SendKeepAlivePacket(i32 connId)
{
    
}

// Load client outputs into a packet and transmit
// This should be the ONLY place that deals with writing packets, all the rest
// should be messages loaded into the client's output buffer
void SV_SendOutput()
{
    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }
        
        ReliableStream* stream = &cl->reliableStream;

        u32 packetSequence = ZNet_GetNextSequenceNumber(cl->connId);

        //u32 sequence = ++stream->outputSequence;

        TransmissionRecord* rec = Stream_AssignTransmissionRecord(
            stream->transmissions, packetSequence);
        
        COM_ZeroMemory(g_dataBuffer, DATA_BUFFER_SIZE);
        ByteBuffer b = Buf_FromBytes(g_dataBuffer, DATA_BUFFER_SIZE);
        // step over header
        // u16 num reliable bytes
        // u16 num unreliable bytes
        // u32 reliable sequence
        b.ptrWrite += sizeof(u16) + sizeof(u16) + sizeof(u32);
        u8* reliableStart = b.ptrWrite;

        // iterate output buffer
        u8* read = stream->outputBuffer.ptrStart;
        u8* end = stream->outputBuffer.ptrWrite;

        // messages will store the offset of their sequence from this number, which
        // is read from the first message
        u32 firstReliableId = 0;
		//printf("Sending %d bytes of reliable msgs\n", (end - read));
        while (read < end)
        {
            u32 id = COM_ReadU32(&read);
			//if (id == 0) { break; }
            u32 size = COM_ReadU32(&read);
			u32 space = b.Space();
			if (b.Space() < size)
			{
				printf("Packet full!\n");
				break;
			}

            if (rec->numReliableMessages == 0)
            {
                firstReliableId = id;
            }
            u32 idOffset = id - firstReliableId;
            u16 msgHeader = Stream_PackMessageHeader((u8)idOffset, (u16)size);
            printf("First id %d offset %d size %d\n", firstReliableId, idOffset, size);
            b.ptrWrite += COM_WriteU16(msgHeader, b.ptrWrite);
			//printf("Writing msg %d (%d bytes)\n", id, size);
            //b.ptrWrite += COM_WriteU32(id, b.ptrWrite);
            b.ptrWrite += COM_COPY(read, b.ptrWrite, size);
            rec->reliableMessageIds[rec->numReliableMessages] = id;
            
            rec->numReliableMessages++;
			if (rec->numReliableMessages == MAX_PACKET_TRANSMISSION_MESSAGES)
			{
				printf("Max messages per packet!\n");
				break;
			}
            read += size;
        }
        Stream_PrintTransmissionRecord(rec);
        u16 reliableBytes = (u16)(b.ptrWrite - reliableStart);
        u8* write = b.ptrStart;
        write += COM_WriteU16(reliableBytes, write);
        // unreliable bytes could be written here
        write += COM_WriteU16(0, write);
        write += COM_WriteU32(firstReliableId, write);
		//printf("Wrote %d packet bytes (%d messages)\n", b.Written(), rec->numReliableMessages);

        u32 sendSequence = ZNet_SendData(
            cl->connId,
            b.ptrStart,
            (u16)b.Written(),
            0
        );
    }
}

// Load frame based messages into client
void SV_ServerTickFrameOutput()
{
    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }
        
        ReliableStream* stream = &cl->reliableStream;

        // Write to output buffer
        TestMsg1 m1 = {};
	    m1.member1 = COM_GetI32Sentinel();
	    m1.member2 = COM_GetI32Sentinel();
	    m1.member3 = COM_GetI32Sentinel();


        Buf_WriteStreamMsgHeader(
            &stream->outputBuffer,
            ++stream->outputSequence,
            m1.MeasureForWriting());
        stream->outputBuffer.ptrWrite += m1.Write(stream->outputBuffer.ptrWrite);
		u32 written = stream->outputBuffer.Written();
		//printf("W: %d\n", written);

        //COM_ZeroMemory(g_dataBuffer, DATA_BUFFER_SIZE);
        //ByteBuffer b = Buf_FromBytes(g_dataBuffer, DATA_BUFFER_SIZE);

        //u32 messageId = 0xABCDABCD;
        /*
        u32 message1 = 0xDEADBEEF;
        u32 message2 = 0xABCDDCBA;
        u32 message1Id = cl->reliableStream.outputSequence++;
        u32 message2Id = cl->reliableStream.outputSequence++;
        
        u16 messageBytes = 4 * sizeof(u32);
        ByteBuffer b = TNET_GetWriteBuffer();
        // packet header
        b.ptrWrite += COM_WriteU16(messageBytes, b.ptrWrite);
        b.ptrWrite += COM_WriteU16(0, b.ptrWrite);
        // message sequence + message itself
        b.ptrWrite += COM_WriteU32(message1Id, b.ptrWrite);
        b.ptrWrite += COM_WriteU32(message1, b.ptrWrite);
        b.ptrWrite += COM_WriteU32(message2Id, b.ptrWrite);
        b.ptrWrite += COM_WriteU32(message2, b.ptrWrite);

        u32 sendSequence = ZNet_SendData(
            cl->connId,
            b.ptrStart,
            (u16)b.Written(),
            0
        );
        */
        //printf("Sent sequence %d\n", sendSequence);
    }
}

void Test_Server(u16 serverPort)
{
    printf("Server\n");
    ZNet_Init(TNet_CreateNetFunctions(), TNet_CreateOutputInterface(), ZNET_SIM_MODE_BAD);
    g_network.Init();

    f32 tickRateSeconds = SERVER_TICK_RATE;
    i32 tickRateMS = (i32)(1000.0f / tickRateSeconds);

    //i32 tickRateMS = 250;
    //f32 tickRateSeconds = 1.0f / (f32)(1000 / tickRateMS);
	
	/*
	Notes:
	> How to transfer a arbitrary list of connections out of ZNet?
	> How to write packets, retreiving the packet sequence number
		and storing it for future delivery status recording?
    
    Testbed features:
    > Maintain list of 'clients'
    > Parse over connection list, 
	*/
    g_network.isServer = 1;
    ZNet_StartSession(NETMODE_DEDICATED_SERVER, NULL, serverPort);
    for(;;)
    {
        /*
        > Read input from connection layer
        > Iterate clients and load their respective frame based outputs
            (in real world frame based data would be unreliable, reliable commands
            would come from internal game events like enemies dying)
        > Iterate clients, sending a packet to each loaded with messages
        */
        
        i32 error = ZNet_Tick(tickRateSeconds);	
        if (error)
        {
            if (error == 1)
            {
                printf("Tests: network closed down\n");
            }
            else if (error == 2)
            {
                printf("Tests: network test completed\n");
            }
            break;
        }
        SV_ServerTickFrameOutput();
        SV_SendOutput();
        Sleep(tickRateMS);
    }
    getc(stdin);
}
