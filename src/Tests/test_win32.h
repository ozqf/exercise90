#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include "../common/com_module.h"

#include "../platform/win32_net/win32_net_interface.h"
//#include "../platform/win32_net/win32_net_module.cpp"

#if 0
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#endif

void Test_Server(u16 port)
{

}

void Test_Client(u16 port)
{
    
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
