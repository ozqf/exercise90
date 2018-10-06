#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include <conio.h>
#include <stdio.h>

#include "../../common/com_module.h"

#include "../../platform/win32_net/win32_net_interface.h"
#include "../../network/znet_interface.h"

#include "test_network_types.h"

#if 1
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#endif

global_variable TestGameNetwork g_network = {};

#define DATA_BUFFER_SIZE 1024
global_variable u8 g_dataBuffer[DATA_BUFFER_SIZE];

void TNet_ConnectionAccepted(ZNetConnectionInfo* conn)
{
    if (g_network.isServer)
    {
        printf("TNET: Client Conn %d accepted\n", conn->id);
        g_network.AssignNewClient(conn->id, 0);
    }
	else
    {
        printf("TNET: Server Conn %d accepted\n", conn->id);
        g_network.SetServer(conn->id);
    }
    g_network.PrintfDebug();
}
void TNet_ConnectionDropped(ZNetConnectionInfo* conn)
{
	printf("TNET: Conn %d dropped\n", conn->id);
    g_network.DeleteClient(conn->id);
    g_network.PrintfDebug();
}
void TNet_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes)
{
    u8* read = bytes;
    u8 type = COM_ReadByte(&read);
    printf("TNET: Received type %d (bytes: %d, sequence: %d) from %d\n", type, numBytes, info->remoteSequence, info->sender.id);
    switch (type)
    {
        case 1:
        {
            printf("TYPE ONE OMG\n");
        } break;
    }
}
void TNet_DeliveryConfirmed(u32 packetNumber)
{
	printf("TNET: Delivery of %d confirmed\n", packetNumber);
}

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

void TNet_ServerSendState()
{
    for (i32 i = 0; i < g_network.capacity; ++i)
    {
        TestClient* cl = &g_network.clients[i];
        if (!cl->inUse) { continue; }

        // build packet
        COM_ZeroMemory(g_dataBuffer, DATA_BUFFER_SIZE);
        ByteBuffer b = Buf_FromBytes(g_dataBuffer, DATA_BUFFER_SIZE);
        b.ptrWrite += COM_WriteByte(1, b.ptrWrite);
        ZNet_SendData(cl->connId, b.ptrStart, (u16)b.Written());
    }
}

void Test_Server(u16 serverPort)
{
    printf("Server\n");
    ZNet_Init(TNet_CreateNetFunctions(), TNet_CreateOutputInterface());
    i32 tickRateMS = 500;
    f32 tickRateSeconds = 1.0f / (f32)(1000 / tickRateMS);
	
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

void Test_Client(u16 serverPort, u16 clientPort)
{
    printf("Client\n");
    ZNet_Init(TNet_CreateNetFunctions(), TNet_CreateOutputInterface());
    i32 tickRateMS = 500;
    f32 tickRateSeconds = 1.0f / (f32)(1000 / tickRateMS);

    ZNetAddress addr = {};
    addr.ip4Bytes[0] = 127;
    addr.ip4Bytes[1] = 0;
    addr.ip4Bytes[2] = 0;
    addr.ip4Bytes[3] = 1;
    addr.port = serverPort;
    g_network.isServer = 0;
    ZNet_StartSession(NETMODE_CLIENT, &addr, clientPort);
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
    getc(stdin);
}

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
    #if 1
    else if (!COM_CompareStrings(mode, "live"))
    {
        printf("Waiting for key:\n");
        while (!_kbhit())
        {
            printf(".");
        }
        printf("\nYou pressed %c\n", _getch());
    }
    #endif
    else
    {
        printf("Win32 Net test: Unknown mode %s\n", mode);
        return;
    }
    #endif
}
