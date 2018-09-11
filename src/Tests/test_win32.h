#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

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

#define DEFAULT_PORT 23232

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
    printf("Winsock initialised\n");
    
    SOCKET s;
    sockaddr_in server;//, si_other;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return;
    }
    printf("Socket Created\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);
    iResult = bind(s, (sockaddr*)&server, sizeof(server));
    if (iResult == SOCKET_ERROR)
    {
        printf("Failed to bind socket with code: %d\n", WSAGetLastError());
        return;
    }
    printf("Socket bound\n");

    printf("Shutting down... ");
    closesocket(s);
    WSACleanup();
    printf("...Done\n");
}