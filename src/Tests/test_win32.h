#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include "../common/com_module.h"

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
#define UDP_BUFFER_LENGTH 1024

/*
- Network IO interface -
void OpenSocket(int port);
void CloseSocket();
Send();
*/

// Example from:
// https://www.binarytides.com/udp-socket-programming-in-winsock/
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
    sockaddr_in server;
	//sockaddr_in si_other;

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

    //i32 slen, recv_len;
    //char buf[UDP_BUFFER_LENGTH];

#if 0
    while(1)
    {
        printf("Waiting for data...\n");
        fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', UDP_BUFFER_LENGTH);

        //try to receive some data, this is a blocking call
        iResult = (recv_len = recvfrom(s, buf, UDP_BUFFER_LENGTH, 0, (struct sockaddr *) &si_other, &slen));
        if (iResult == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }
#endif

    printf("Shutting down... ");
    closesocket(s);
    WSACleanup();
    printf("Done\n");
}