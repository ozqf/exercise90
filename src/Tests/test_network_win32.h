#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include "../common/com_module.h"

#include "../platform/win32_net/win32_net_interface.h"
#include "../network/znet_interface.h"
#if 1
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#endif

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

void Test_Server(u16 serverPort)
{
    printf("Server\n");
    ZNet_Init(TNet_CreateNetFunctions());
    i32 tickRateMS = 500;
    f32 tickRateSeconds = 1.0f / (f32)(1000 / tickRateMS);

    ZNet_StartSession(NETMODE_DEDICATED_SERVER, NULL, serverPort);
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
            return;
        }
        Sleep(tickRateMS);
    }
    //getc(stdin);
}

void Test_Client(u16 serverPort, u16 clientPort)
{
    printf("Client\n");
    ZNet_Init(TNet_CreateNetFunctions());
    i32 tickRateMS = 500;
    f32 tickRateSeconds = 1.0f / (f32)(1000 / tickRateMS);

    ZNetAddress addr = {};
    addr.ip4Bytes[0] = 127;
    addr.ip4Bytes[1] = 0;
    addr.ip4Bytes[2] = 0;
    addr.ip4Bytes[3] = 1;
    addr.port = serverPort;
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
            return;
        }
        Sleep(tickRateMS);
    }
    //getc(stdin);
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
    else
    {
        printf("Win32 Net test: Unknown mode %s\n", mode);
        return;
    }
    #endif
}
