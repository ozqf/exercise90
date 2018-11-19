#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include <conio.h>
#include <stdio.h>

#include "../../common/com_module.h"

#include "../../platform/win32_net/win32_net_interface.h"
#include "../../network/znet_interface.h"

#include "test_network_types.h"
#include "test_network_messages.h"
#include "test_network_buffer.h"
#include "test_reliable_stream.h"

#if 1
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#endif

/*
Notes:
Clients have two states
> the state of their connection (connected, pending, etc)
> the state of their client (loading or in session)
Connection state is handled by ZNet layer
Client state is handled outside the network layer, and is data related

Current 'state' + the client's public Id must be transferred to them before
they can be considered 'inPlay'

Steps to sync client and server:
Server requires client's 'info'
Client requires current server state (right now, clients in chatroom)

--- Reliability ---
> ZNet layer will report packet delivery confirmations but cannot be certain that a
    packet WASN'T delivered (though it will be forced to conclude this if the packet's
    sequence falls outside the moving ack window, this still doesn't mean it wasn't
    *eventually* delivered somehow)
> ZNet only understands packet sequences, and has no concept of individual messages.
> Application layer works with individual message Ids. Packet ids are used to acknowledge
    message delivery by logging the message ids stored in each packet.
> Application splits data loads into reliable and unreliable messages. Reliable messages first,
    unreliable second. Unreliable messages have no messageId and are not acked.

Transmission:
> Each reliable message increments a messageId.
> Buffer reliable messages in a 'reliableOutput' buffer.
> When sending packets, record the packet sequence and an array of all reliable messages in
    that packet (eg packet 1, messages 1,2,3,4,5 etc)
> When ZNet confirms a packet was received, all messages in that packet can be removed from
    the reliable output buffer and the transmission record cleared.

Reception:
> Start with 'remoteMessageId' of 0. The first reliable message sent must be messageId of 1.
> When receiving reliable messages, place them in a 'reliableInput' buffer.
> On Read, look in reliableInput for messages with 'remoteMessageId + 1'
	> If a message is found, read it and increment remoteSequence by 1.
	> Repeat until the buffer is empty or no message of remoteSequence + 1 is found.
> Every outbound packet is marked with the remoteSequence. The sender can then discard
	all messages before that number.

Storage:
Application layer requires a mechanism to buffer reliable network messages both for
retransmission and in-order execution.

Stream
> WriteToInput
> WriteToOutput
> ReadInput
> WriteOutput

Packet structure
> (u16) bytes of reliable messages
> (u16) bytes of unreliable messages
> (u32) reliableSequence
> Reliable messages:
    > messageHeader (u16) - 6 bits offset from reliableSequence, 10 bits of size in bytes
    > Message - (size specified in messageHeader, first byte == type)
    > messageId (u16)
    > Message
    > ...etc
> Unreliable Messages
    > Message
    > Message
    > Message
    > ..etc
*/

#define SERVER_TICK_RATE 60
#define CLIENT_TICK_RATE 60

// interface
ZNetPlatformFunctions TNet_CreateNetFunctions();
ZNetOutputInterface TNet_CreateOutputInterface();

global_variable TestGameNetwork g_network = {};

#define DATA_BUFFER_SIZE 1024
global_variable u8 g_dataBuffer[DATA_BUFFER_SIZE];


ByteBuffer TNET_GetWriteBuffer()
{
    COM_ZeroMemory(g_dataBuffer, DATA_BUFFER_SIZE);
    return Buf_FromBytes(g_dataBuffer, DATA_BUFFER_SIZE);
}

// Client/server functions
#include "test_network_client.h"
#include "test_network_server.h"

/////////////////////////////////////////////////////////////////////
// Network callbacks
/////////////////////////////////////////////////////////////////////

void TNet_ConnectionAccepted(ZNetConnectionInfo* conn)
{
    if (g_network.isServer)
    {
        printf("TNET: Server accepted client %d (public %d)\n", conn->id, conn->publicId);
        g_network.AssignNewClient(conn->id, conn->publicId);
    }
	else
    {
        printf("TNET: Client: Server approved connection %d (public %d)\n", conn->id, conn->publicId);
        g_network.isActive = 1;
        g_network.SetServer(conn->id);
        // TODO: Record local public and private ids here!
    }
    g_network.PrintfDebug();
}

void TNet_ConnectionDropped(ZNetConnectionInfo* conn)
{
	printf("TNET: Conn %d dropped\n", conn->id);
    g_network.DeleteClient(conn->id);
    g_network.PrintfDebug();
    if (!g_network.isServer) { g_network.isActive = 0; }
}



/*
For reliable messages this is where they are copied into their respective input buffers
*/
void TNet_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes)
{
    ReliableStream* stream;
    if (g_network.isServer)
    {
        TestClient* cl = g_network.GetClient(info->sender.id);
        stream = &cl->reliableStream;
    }
    else
    {
        stream = &g_network.server.reliableStream;
    }

    u8* read = bytes;
    u16 reliableBytes = COM_ReadU16(&read);
    u16 unreliableBytes = COM_ReadU16(&read);
    if (reliableBytes != 0)
    {
        //printf(
        //    "TNET: Received sequence %d. %d Reliable, %d  unreliable bytes\n",
        //    info->remoteSequence, reliableBytes, unreliableBytes);
        //printf("TNET: Received type %d (bytes: %d, sequence: %d) from %d\n",
        //    type, numBytes, info->remoteSequence, info->sender.id);
        Stream_PacketToInput(stream, read, reliableBytes);
        //printf("> %d bytes in Input\n", stream->inputBuffer.Written());
    }
}

void TNet_DeliveryConfirmed(ZNetConnectionInfo* conn, u32 packetNumber)
{
    // need the connId!

	//printf("TNET: Delivery of %d confirmed\n", packetNumber);
    // Search for and remove packet record
    if (g_network.isServer)
    {
        //printf("ACK server confirming delivery of %d from conn %d\n", packetNumber, conn->id);
        TestClient* cl = g_network.GetClient(conn->id);
        
        TransmissionRecord* rec =  Stream_FindTransmissionRecord(
            cl->reliableStream.transmissions, packetNumber);
        if (rec)
        {
            //Stream_PrintTransmissionRecord(rec);
            Stream_ClearReceivedMessages(rec, &cl->reliableStream.outputBuffer);
        }
    }
    else
    {
        ReliableStream* s = &g_network.server.reliableStream;
        TransmissionRecord* rec = Stream_FindTransmissionRecord(
            s->transmissions, packetNumber);
        if (rec)
        {
            Stream_ClearReceivedMessages(rec, &s->outputBuffer);
        }
    }
}

/////////////////////////////////////////////////////////////////
// Network init
//////////////////////////////////////////////////////////////
ZNetPlatformFunctions TNet_CreateNetFunctions()
{
    ZNetPlatformFunctions x = {};
    x.Init = Net_Init;
    x.Shutdown = Net_Shutdown;
    x.OpenSocket = Net_OpenSocket;
    x.CloseSocket = Net_CloseSocket;
    x.Read = Net_Read;
    x.SendTo = Net_SendTo;
    return x;
}

ZNetOutputInterface TNet_CreateOutputInterface()
{
	ZNetOutputInterface x = {};
	x.ConnectionAccepted = TNet_ConnectionAccepted;
	x.ConnectionDropped = TNet_ConnectionDropped;
	x.DataPacketReceived = TNet_DataPacketReceived;
	x.DeliveryConfirmed = TNet_DeliveryConfirmed;
	return x;
}

/////////////////////////////////////////////////////////////////
// Entry point
/////////////////////////////////////////////////////////////////
#define TEST_SERVER_PORT 23232
#define TEST_CLIENT_PORT 61200

// Example from:
// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-recvfrom
// https://www.binarytides.com/udp-socket-programming-in-winsock/
void Test_Win32(i32 argc, char* argv[])
{
    //TNet_TestReliability();
    #if 1
    printf("Test winsock\n");
    srand((u32)argv);
    printf("SEED: %d\n", rand());

    #if 1
    if (argc != 2)
    {   
        printf("Arg count mismatch. Needed 2 got %d\n", argc);
        //Test_Server(TEST_SERVER_PORT);
        Test_Client(TEST_SERVER_PORT, TEST_CLIENT_PORT);
        return;
    }
    char* mode = argv[1];
    if (!COM_CompareStrings(mode, "server"))
    {
        Test_Server(TEST_SERVER_PORT);
    }
    else if (!COM_CompareStrings(mode, "client"))
    {
        Test_Client(TEST_SERVER_PORT, TEST_CLIENT_PORT);
    }
    else if (!COM_CompareStrings(mode, "both"))
    {
        //Net_RunLoopbackTest();
    }
	else if (!COM_CompareStrings(mode, "rel"))
	{
		TNet_TestReliability();
	}
    else if (!COM_CompareStrings(mode, "znet"))
	{
		ZNet_RunTests();
	}
    
    #if 1
    else if (!COM_CompareStrings(mode, "live"))
    {
        i32 running = 1;
        u32 ticks = 0;
        while(running)
        {
            Sleep(100);
            printf("Waiting for key:\n");
            while (!_kbhit())
            {
                printf("\r%d", ticks++);
                //printf(".");
            }
            int c = _getch();
            if (c == KEYCODE_ESCAPE)
            {
                running = false;
                printf("Exit");
                continue;
            }
            printf("\nYou pressed %c (keycode %d)\n", c, c);
        }
        
    }
    #endif
    else
    {
        printf("Win32 Net test: Unknown mode %s\n", mode);
        return;
    }
    #endif
    #endif
}