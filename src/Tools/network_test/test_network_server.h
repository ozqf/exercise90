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
void SV_SendOutput()
{
    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }
        
        ReliableStream* stream = &cl->reliableStream;

        //PacketTransmissionRecord rec = 
    }
}

// Load frame based messages into client outputs
// This should be the ONLY place that deals with writing packets, all the rest
// should be messages loaded into the client's output buffer
void SV_ServerTickFrameOutput()
{
    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }
        
        ReliableStream* stream = &cl->reliableStream;

        // Write to output buffer
        TestMsg1 m1 = {};
	    m1.member1 = 256;
	    m1.member2 = 512;
	    m1.member3 = 768;

        Buf_WriteMessageHeader(&stream->outputBuffer, stream->outputSequence, m1.Measure());
        stream->outputBuffer.ptrWrite += m1.Write(stream->outputBuffer.ptrWrite);

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
    ZNet_Init(TNet_CreateNetFunctions(), TNet_CreateOutputInterface(), ZNET_SIM_MODE_NONE);
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
