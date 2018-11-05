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

void SV_ExecuteClientMessage(TestGameNetwork* net, TestClient* cl, u8* bytes)
{
    u8 type = *bytes;
    //printf("SV exec type %d for client %d\n", type, cl->publicClientId);
    switch (type)
    {
        case TNET_MSG_TYPE_C2S_CHAT:
        {
            MsgC2SChat msg = {};
			COM_ZeroMemory((u8*)&msg, sizeof(msg));
            msg.Read(bytes);
            printf("%d says: %s\n", cl->publicClientId, msg.message);
        } break;

    }
}

/**
 * Iterate clients and read their inputs
 */
void SV_ReadInput(TestGameNetwork* net)
{
    for (i32 i = 0; i < net->capacity; ++i)
    {
        TestClient* cl = &net->clients[i];
        if (!cl->inUse) { continue; }

        ReliableStream* stream = &cl->reliableStream;
        ByteBuffer* b = &stream->inputBuffer;

        // Read buffer
        u32 nextMsg = stream->inputSequence + 1;
        // TODO: This necessary? Figure out what might happen with a 16 bit
        // sequence number
        if (nextMsg == 0)
        {
            printf("ERROR: Next msg is 0!\n");
        }
        StreamMsgHeader* h = Stream_FindMessageById(b->ptrStart, b->ptrWrite, nextMsg);
        // No pending message?
        if (!h) { continue; }
        // update input sequence number
        stream->inputSequence = nextMsg;

        u8* msg = (u8*)h + sizeof(StreamMsgHeader);
        //printf("SV exec msg %d from CL %d\n", nextMsg, cl->connId);
        SV_ExecuteClientMessage(net, cl, msg);
        // Clear message
        u8* blockStart = (u8*)h;
        u32 blockSize = sizeof(StreamMsgHeader) + h->size;
        b->ptrWrite = Stream_CollapseBlock(blockStart, blockSize, b->ptrWrite);

    }
}

/**
 * Load client outputs into a packet and transmit
 * This should be the ONLY place that deals with writing packets, all the rest
 * should be messages loaded into the client's output buffer
 */
void SV_SendOutput()
{
    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }

        Stream_OutputToPacket(
            &g_network,
            cl->connId,
            &cl->reliableStream,
            g_dataBuffer,
            DATA_BUFFER_SIZE
        );
        #if 0
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
            //printf("First id %d offset %d size %d\n", firstReliableId, idOffset, size);
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
        //Stream_PrintTransmissionRecord(rec);
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
        #endif
    }
}

// Load frame based messages into client output streams
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
    }
}

void Test_Server(u16 serverPort)
{
    printf("Server\n");
    ZNet_Init(TNet_CreateNetFunctions(), TNet_CreateOutputInterface(), ZNET_SIM_MODE_BAD);
    g_network.Init();

    f32 tickRateSeconds = SERVER_TICK_RATE;
    i32 tickRateMS = (i32)(1000.0f / tickRateSeconds);

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
        SV_ReadInput(&g_network);
        SV_ServerTickFrameOutput();
        SV_SendOutput();
        Sleep(tickRateMS);
    }
    getc(stdin);
}
