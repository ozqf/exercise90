#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include <conio.h>
#include <stdio.h>

#include "../../common/com_module.h"

#include "../../platform/win32_net/win32_net_interface.h"
#include "../../network/znet_interface.h"

#include "test_network_types.h"
#include "test_network_buffer.h"
#include "test_network_reliability.h"

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
    packet WASN'T delivered.
> ZNet only understands packet sequences, and has no concept of individual messages.
> Application layer works with individual message Ids. Packet ids are used to acknowledge
    message delivery
> Application splits data loads into reliable and unreliable messages. Reliable messages first,
    unreliable second. Unreliable messages have no messageId and are no acked.

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

*/

#define SERVER_TICK_RATE 2
#define CLIENT_TICK_RATE 2

// interface
ZNetPlatformFunctions TNet_CreateNetFunctions();
ZNetOutputInterface TNet_CreateOutputInterface();

global_variable TestGameNetwork g_network = {};

#define DATA_BUFFER_SIZE 1024
global_variable u8 g_dataBuffer[DATA_BUFFER_SIZE];

#define TNET_MSG_TYPE_TEST 1
#define TNET_MSG_TYPE_C2S_CHAT 2
#define TNET_MSG_TYPE_C2S_INFO 3
#define TNET_MSG_TYPE_S2C_CHAT 4
#define TNET_MSG_TYPE_S2C_CLIENT_LIST 5


ByteBuffer TNET_GetWriteBuffer()
{
    COM_ZeroMemory(g_dataBuffer, DATA_BUFFER_SIZE);
    return Buf_FromBytes(g_dataBuffer, DATA_BUFFER_SIZE);
}

//////////////////////////////////////////////////////////////////////
// SERVER
//////////////////////////////////////////////////////////////////////
void TNet_ServerSendChatMsg(i32 senderPublicId, char* msg, u16 numChars)
{
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
}

void TNet_ServerSendClientList(i32 senderPublicId, char* msg, u16 numChars)
{
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
}

void TNet_ServerSendState()
{
    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }

        // build packet
        //COM_ZeroMemory(g_dataBuffer, DATA_BUFFER_SIZE);
        //ByteBuffer b = Buf_FromBytes(g_dataBuffer, DATA_BUFFER_SIZE);
        ByteBuffer b = TNET_GetWriteBuffer();
        b.ptrWrite += COM_WriteByte(1, b.ptrWrite);
        u32 sendSequence = ZNet_SendData(cl->connId, b.ptrStart, (u16)b.Written(), 0);
        //printf("Sent sequence %d\n", sendSequence);
    }
}

void Test_Server(u16 serverPort)
{
    printf("Server\n");
    ZNet_Init(TNet_CreateNetFunctions(), TNet_CreateOutputInterface());

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
        i32 error = ZNet_Tick(tickRateSeconds);
        TNet_ServerSendState();
		
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
        Sleep(tickRateMS);
    }
    getc(stdin);
}

//////////////////////////////////////////////////////////////////////
// CLIENT
//////////////////////////////////////////////////////////////////////
void Test_ClientSendChatMessage(char* buf, u32 length)
{
    if (!g_network.isActive) { printf("Cannot send, network not active\n"); return; }
    ByteBuffer b = TNET_GetWriteBuffer();
    b.ptrWrite += COM_WriteByte(TNET_MSG_TYPE_C2S_CHAT, b.ptrWrite);
    b.ptrWrite += COM_WriteU16((u16)length, b.ptrWrite);
    u32 pos = 0;
    while (pos < length)
    {
        *b.ptrWrite = (u8)buf[pos];
        b.ptrWrite += sizeof(u8);
        pos++;
    }
    printf("\nSent %d chars\n", length);
    ZNet_SendData(g_network.server.connId, b.ptrStart, (u16)b.Written(), 0);
}

void Test_ClientSendState(i32 connId)
{
    ByteBuffer b = TNET_GetWriteBuffer();
    b.ptrWrite += COM_WriteByte(1, b.ptrWrite);
    u32 sendSequence = ZNet_SendData(connId, b.ptrStart, (u16)b.Written(), 0);
    //printf("Sent sequence %d\n", sendSequence);
}

void Test_ClientSendInfo()
{

}

// Keep seeing 83... wtf is keycode 83?
#define KEYCODE_ENTER 13
#define KEYCODE_BACKSPACE 8
#define KEYCODE_DELETE 224
#define KEYCODE_ESCAPE 27

void Test_Client(u16 serverPort, u16 clientPort)
{
    printf("Client\n");
    ZNet_Init(TNet_CreateNetFunctions(), TNet_CreateOutputInterface());

    f32 tickRateSeconds = SERVER_TICK_RATE;
    i32 tickRateMS = (i32)(1000.0f / tickRateSeconds);
    //i32 tickRateMS = 100;
    //f32 tickRateSeconds = 1.0f / (f32)(1000 / tickRateMS);

    ZNetAddress addr = {};
    addr.ip4Bytes[0] = 127;
    addr.ip4Bytes[1] = 0;
    addr.ip4Bytes[2] = 0;
    addr.ip4Bytes[3] = 1;
    addr.port = serverPort;
    g_network.isServer = 0;
    ZNet_StartSession(NETMODE_CLIENT, &addr, clientPort);

    // basic chat interface

    char chatMsg[256];
    COM_ZeroMemory((u8*)chatMsg, 256);
    i32 position = 0;

    char lineClearBuf[80];
    COM_SetMemory((u8*)lineClearBuf, 80, (u8)' ');
    lineClearBuf[79] = '\0';
    
    i32 running = 1;
    u32 ticks = 0;
    while(running)
    {
        //printf("Waiting for key at pos %d:\n", position);
        while (!_kbhit())
        {
            //system("cls");
            i32 error = ZNet_Tick(tickRateSeconds);
            if (g_network.server.connId != 0)
            {
                Test_ClientSendState(g_network.server.connId);
            }
            
            //printf("\r%d", ticks++);
            printf("\r%s", lineClearBuf);
            printf("\r%s", chatMsg);
            Sleep(tickRateMS);
        }
        int c = _getch();
        if (c == KEYCODE_ESCAPE)
        {
            running = false;
            printf("Exit");
            continue;
        }
        else if (c == KEYCODE_ENTER)
        {
            //printf("\rSAY: %s\n", chatMsg);
            
            if (position > 0)
            {
                if (chatMsg[0] == '/')
                {
                    // exec command
                    printf("EXEC COMMAND\n");
                }
                else
                {
                    // send chat
                    // + 1 to position to include null terminator
                    Test_ClientSendChatMessage(chatMsg, position + 1);
                }
                COM_ZeroMemory((u8*)chatMsg, 256);
                position = 0;
            }
        }
        else if (c == KEYCODE_BACKSPACE)
        {
            //printf("Delete char at %d\n", position);
            chatMsg[position] = '\0';
            if (position > 0)
            {
                position--;
            }
        }
        else if (c != '\0')
        {
            chatMsg[position] = (char)c;
            if (position < (256 - 1))
            {
                position++;
            }
        }
        //printf("\nYou pressed %c (keycode %d)\n", c, c);
    }
    #if 0
    for(;;)
    {
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
        Sleep(tickRateMS);
    }
    
    #endif

    // keep window open
    getc(stdin);
}

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

void TNet_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes)
{
    u8* read = bytes;
    u8 type = COM_ReadByte(&read);
    printf("TNET: Received sequence %d\n", info->remoteSequence);
    //printf("TNET: Received type %d (bytes: %d, sequence: %d) from %d\n", type, numBytes, info->remoteSequence, info->sender.id);
    switch (type)
    {
        case 1:
        {
            //printf("TNET TYPE %d\n", type);
        } break;

        case TNET_MSG_TYPE_C2S_CHAT:
        {
            if (g_network.isServer)
            {
                //printf("TNET TYPE %d\n", type);
                // sanitise a little
                char buf[256];
                u16 numChars = COM_ReadU16(&read);
                if (numChars > 256) { numChars = 256; }

                COM_ZeroMemory((u8*)buf, 256);
                COM_COPY(read, buf, numChars);
                TestClient* cl = g_network.GetClient(info->sender.id);
                if (!cl)
                {
                    printf("SV Not client %d on server\n", info->sender.id);
                    break;
                }
                printf("%d (%d) says: %s\n", cl->publicClientId, info->sender.id, buf);
                TNet_ServerSendChatMsg(cl->publicClientId, buf, numChars);
            }
            
        } break;

        case TNET_MSG_TYPE_S2C_CHAT:
        {
            char buf[256];
            i32 senderPublicId = COM_ReadI32(&read);
            u16 numChars = COM_ReadU16(&read);
            if (numChars > 256) { numChars = 256; }
            COM_ZeroMemory((u8*)buf, 256);
            COM_COPY(read, buf, numChars);
            printf("%d says: %s\n", senderPublicId, buf);
        } break;
    }
}

void TNet_DeliveryConfirmed(u32 packetNumber)
{
	printf("TNET: Delivery of %d confirmed\n", packetNumber);
}

/////////////////////////////////////////////////////////////////
// Network init
/////////////////////////////////////////////////////////////////
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
	/*
	void (*ConnectionAccepted)		(ZNetConnectionInfo* conn);
	void (*ConnectionDropped)		(ZNetConnectionInfo* conn);
	void (*DataPacketReceived)		(ZNetPacketInfo* info, u8* bytes, u16 numBytes);
	void (*DeliveryConfirmed)		(u32 packetNumber);
	*/
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
    printf("Test winsock\n");
    srand((u32)argv);
    printf("SEED: %d\n", rand());

    #if 1
    if (argc != 2)
    {   
        printf("Arg count mismatch. Needed 2 got %d\n", argc);
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
}
