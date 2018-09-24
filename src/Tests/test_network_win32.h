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

void Test_Server(u16 port)
{
    printf("Server\n");
    ZNet_Init(TNet_CreateNetFunctions());

    ZNetAddress addr = {};
    addr.port = 23232;
    ZNet_StartSession(NETMODE_DEDICATED_SERVER, addr);
    for(;;)
    {
        ZNet_Tick();
        Sleep(1000);
    }
    //getc(stdin);
}

void Test_Client(u16 port)
{
    printf("Client\n");
    ZNet_Init(TNet_CreateNetFunctions());
    ZNetAddress addr = {};
    addr.ip4Bytes[0] = 127;
    addr.ip4Bytes[1] = 0;
    addr.ip4Bytes[2] = 0;
    addr.ip4Bytes[3] = 1;
    addr.port = 23234;
    ZNet_StartSession(NETMODE_CLIENT, addr);
    for(;;)
    {
        ZNet_Tick();
        Sleep(1000);
    }
    //getc(stdin);
}

// Example from:
// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-recvfrom
// https://www.binarytides.com/udp-socket-programming-in-winsock/
void Test_Win32(i32 argc, char* argv[])
{
    printf("Test winsock\n");

    #if 1
    if (argc != 2)
    {   
        printf("Arg count mismatch. Needed 2 got %d\n", argc);
        return;
    }
    char* mode = argv[1];
    if (!COM_CompareStrings(mode, "server"))
    {
        Test_Server(23232);
    }
    else if (!COM_CompareStrings(mode, "client"))
    {
        Test_Client(23233);
    }
    else if (!COM_CompareStrings(mode, "both"))
    {
        Net_RunLoopbackTest();
    }
    else
    {
        printf("Win32 Net test: Unknown mode %s\n", mode);
        return;
    }
    #endif
}
