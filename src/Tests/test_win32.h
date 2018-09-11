#pragma once

#if 0
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#endif

// https://beej.us/guide/bgnet/html/single/bgnet.html
// https://docs.microsoft.com/en-us/windows/desktop/WinSock/getting-started-with-winsock
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

void Test_Win32()
{
    printf("Test winsock\n");

    WSADATA wsaData;
    
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return;
    }

}