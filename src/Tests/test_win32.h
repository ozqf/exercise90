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

#define NET_NON_BLOCKING_ERROR 10035
#define DEFAULT_PORT_SERVER 23232
#define DEFAULT_PORT_CLIENT 23233
#define UDP_BUFFER_LENGTH 4096
#define LOCALHOST_ADDRESS "127.0.0.1"

#define NET_ERROR_CASE(errorCode, msg) case errorCode##: printf(##msg##); break;

// Add new error codes as you run into them!
// https://docs.microsoft.com/en-us/windows/desktop/winsock/windows-sockets-error-codes-2
void Net_PrintNetworkError(i32 code)
{
    switch (code)
    {
        NET_ERROR_CASE(10014, "Bad Address\n");
        NET_ERROR_CASE(10022, "Invalid parameter\n");
        NET_ERROR_CASE(10035, "Non-blocking socket had nothing available\n");
        NET_ERROR_CASE(10047, "Address family not supported by protocol family.\n");
        NET_ERROR_CASE(10048, "Address in use\n");
        default: printf("Unknown error code... Sorry\n"); break;
    }
}

/*
- Network IO interface -
void OpenSocket(int port);
void CloseSocket();
Send();
*/

struct Win32_Socket
{
	i32 isActive;
    WSADATA wsaData;
    SOCKET socket;
    sockaddr_in server;
    i32 slen;
    u16 port;
};

#define MAX_SOCKETS 2
internal Win32_Socket g_connections[MAX_SOCKETS];

void Net_Send(Win32_Socket* winSock, char* address, u16 port, char* outboundMsg)
{
    printf("Sending %s to %s:%d\n", outboundMsg, address, port);
	sockaddr_in toAddress;
    toAddress.sin_port = DEFAULT_PORT_SERVER;
    toAddress.sin_family = AF_INET;
    toAddress.sin_addr.S_un.S_addr = InetPton(AF_INET, address, &toAddress.sin_addr.S_un.S_addr);
    
    i32 toAddressSize = sizeof(toAddress);
    
    i32 sendLength = COM_StrLen(outboundMsg) + 1;

    i32 sendResult = sendto(
        winSock->socket,
        outboundMsg,
        sendLength,
        0,
        (sockaddr*)&toAddress,
        toAddressSize
    );
    if (sendResult == SOCKET_ERROR)
    {
        i32 errorCode = WSAGetLastError();
        printf("Send error: %d\n", errorCode);
    }
    else
    {
        printf("Send Result: %d\n", sendResult);
    }
}

i32 Net_ReadSocket(Win32_Socket* winSock)
{
    // optional struct to store the sender's address
    sockaddr_in fromAddress;
    i32 fromAddressSize = sizeof(fromAddress);

    i32 recv_len;
    char buf[UDP_BUFFER_LENGTH];
    memset(buf, '\0', UDP_BUFFER_LENGTH);
    i32 flags = 0;
    recv_len = recvfrom(
        winSock->socket,
        buf,
        UDP_BUFFER_LENGTH,
        flags,
        (struct sockaddr *) &fromAddress,
        &fromAddressSize
    );
    
    if (recv_len == SOCKET_ERROR)
    {
        i32 errorCode = WSAGetLastError();
        if (errorCode != NET_NON_BLOCKING_ERROR)
        {
            printf("recvfrom() failed with error code %d\n", errorCode);
            Net_PrintNetworkError(errorCode);
        }
        else
        {
            printf(".");
        }
    }
    else
    {
        printf("\nReceived length: %d\n", recv_len);
    }
    return recv_len > 0 ? recv_len : 0;
}

// returns 0 on success, or error code
i32 Net_OpenSocket(Win32_Socket* winSock, u16 port)
{
	Assert(!winSock->isActive)

    winSock->port = port;

    
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &winSock->wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return iResult;
    }
    printf("Winsock initialised\n");
    
    if ((winSock->socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        i32 errorCode = WSAGetLastError();
        printf("Could not create socket: %d\n", errorCode);
        return errorCode;
    }
    printf("Socket Created\n");

    winSock->server.sin_family = AF_INET;
    winSock->server.sin_addr.s_addr = htons(INADDR_ANY);
    winSock->server.sin_port = htons(winSock->port);

    u_long mode = 1;  // 1 to enable non-blocking socket
    ioctlsocket(winSock->socket, FIONBIO, &mode);

    iResult = bind(winSock->socket, (sockaddr*)&winSock->server, sizeof(winSock->server));
    if (iResult == SOCKET_ERROR)
    {
        i32 errorCode = WSAGetLastError();
        printf("Failed to bind socket with code: %d\n", errorCode);
        Net_PrintNetworkError(errorCode);
        return errorCode;
    }
    printf("Socket bound on port %d\n", winSock->port);

	winSock->isActive = 1;
	return 0;
}

void Net_Shutdown()
{
    printf("\nShutting down...\n");
    for (i32 i = 0; i < MAX_SOCKETS; ++i)
    {
        Win32_Socket* sock = &g_connections[i];
        if (!sock->isActive) { continue; }

        i32 result = closesocket(sock->socket);
        printf("Closing socket %d result: %d\n", i, result);
    }
    WSACleanup();
    printf("Done\n");
}

void Net_RunLoopbackTest()
{
    i32 errorCode;
    errorCode = Net_OpenSocket(&g_connections[0], DEFAULT_PORT_SERVER);
    if (errorCode)
    {
        printf("Failed to open socket: %d\n", errorCode);
        return;
    }
    errorCode = Net_OpenSocket(&g_connections[1], DEFAULT_PORT_CLIENT);
    if (errorCode)
    {
        printf("Failed to open socket: %d\n", errorCode);
        return;
    }

    // This shit doesn't work... why?
    Net_Send(&g_connections[1], LOCALHOST_ADDRESS, DEFAULT_PORT_SERVER, "foo");
    printf("Reading from socket: ");
    i32 i = 0;
    while (i++ < 100)
    {
        Net_ReadSocket(&g_connections[0]);
        Sleep(100);
    }
    
    Net_Shutdown();
}

void Net_RunServerTest()
{
    printf("Server test\n");
    i32 errorCode = Net_OpenSocket(&g_connections[0], DEFAULT_PORT_SERVER);
    if (errorCode)
    {
        printf("Failed to open socket: %d\n", errorCode);
        return;
    }
    
    for(;;)
    {
        i32 read = read = Net_ReadSocket(&g_connections[0]);
        if (read > 0)
        {
            printf("Holy shit read %d bytes\n", read);
            break;
        }
    }
}

void Net_RunClientTest()
{
    printf("Client test\n");
    i32 errorCode = Net_OpenSocket(&g_connections[0], DEFAULT_PORT_CLIENT);
    if (errorCode)
    {
        printf("Failed to open socket: %d\n", errorCode);
        return;
    }
    i32 count = 10;
    while (count-- > 0)
    {
        Net_Send(&g_connections[0], LOCALHOST_ADDRESS, DEFAULT_PORT_SERVER, "foo");
    }
}

#if 0
void Test_Client(u16 port)
{
    printf("Starting client\n");


    char* msg = "Message from client";

    //send the message
    #if 0
    if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    #endif
    COM_StrLen(msg);
}

void Test_Server(u16 port)
{
    printf("Starting server\n");

    Win32_Socket conn = {};
    conn.port = port;

    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &conn.wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return;
    }
    printf("Winsock initialised\n");
    
    if ((conn.socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return;
    }
    printf("Socket Created\n");

    conn.server.sin_family = AF_INET;
    conn.server.sin_addr.s_addr = INADDR_ANY;
    conn.server.sin_port = htons(conn.port);

    //u_long
    u_long mode = 1;  // 1 to enable non-blocking socket
    ioctlsocket(conn.socket, FIONBIO, &mode);

    iResult = bind(conn.socket, (sockaddr*)&conn.server, sizeof(conn.server));
    if (iResult == SOCKET_ERROR)
    {
        i32 errorCode = WSAGetLastError();
        printf("Failed to bind socket with code: %d\n", errorCode);
        Net_PrintNetworkError(errorCode);
        return;
    }
    printf("Socket bound\n");

    // optional struct to store the sender's address
    sockaddr_in fromAddress;
    i32 fromAddressSize = sizeof(fromAddress);

    /*
    struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
    };
    */
    sockaddr_in toAddress;
    toAddress.sin_port = DEFAULT_PORT_SERVER;
    toAddress.sin_family = AF_INET;
    toAddress.sin_addr.S_un.S_addr = InetPton(AF_INET, LOCALHOST_ADDRESS, &toAddress.sin_addr.S_un.S_addr);
    
    i32 toAddressSize = sizeof(toAddress);
    

    i32 recv_len;
    char buf[UDP_BUFFER_LENGTH];

    char* outboundMsg = "Foo";
    i32 sendLength = COM_StrLen(outboundMsg);
    
    i32 running = 1;
    i32 iterations = 0;
    while(running)
    {
        printf("------------------------------------------------------\nWaiting for data...\n");
        fflush(stdout);

        ///////////////////////////////////////////////////////////////////////////
        // Send

        printf("Send!\n");
        i32 sendResult = sendto(
            conn.socket,
            outboundMsg,
            sendLength,
            0,
            (sockaddr*)&toAddress,
            toAddressSize
        );
        if (sendResult == SOCKET_ERROR)
        {
            i32 errorCode = WSAGetLastError();
            printf("Send error: %d\n", errorCode);
        }
        else
        {
            printf("Send Result: %d\n", sendResult);
        }

        Sleep(50);
        
        ///////////////////////////////////////////////////////////////////////////
        // Receive

        //clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', UDP_BUFFER_LENGTH);
        i32 flags = 0;

        recv_len = recvfrom(
            conn.socket,
            buf,
            UDP_BUFFER_LENGTH,
            flags,
            (struct sockaddr *) &fromAddress,
            &fromAddressSize
        );
        printf("Received length: %d\n", recv_len);
        if (recv_len == SOCKET_ERROR)
        {
            i32 errorCode = WSAGetLastError();
            if (errorCode != NET_NON_BLOCKING_ERROR)
            {
                printf("recvfrom() failed with error code : %d\n" , errorCode);
                Net_PrintNetworkError(errorCode);
                running = 0;
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("No data!\n");
                iterations++;
                if (iterations > 10)
                {
                    running = false;
                }
            }
        }
    }

    printf("Shutting down... ");
    closesocket(conn.socket);
    WSACleanup();
    printf("Done\n");
}
#endif

// Example from:
// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-recvfrom
// https://www.binarytides.com/udp-socket-programming-in-winsock/
void Test_Win32(i32 argc, char* argv[])
{
    printf("Test winsock\n");

    //Net_RunLoopbackTest();
    #if 1
    if (argc != 2)
    {   
        printf("Arg count mismatch. Needed 2 got %d\n", argc);
        return;
    }
    char* mode = argv[1];
    if (!COM_CompareStrings(mode, "server"))
    {
        //Test_Server(DEFAULT_PORT_SERVER);
        Net_RunServerTest();
    }
    else if (!COM_CompareStrings(mode, "client"))
    {
        //Test_Client(DEFAULT_PORT_CLIENT);
        Net_RunClientTest();
    }
    else if (!COM_CompareStrings(mode, "both"))
    {
        Net_RunLoopbackTest();
    }
    else
    {
        printf("Unknown mode %s\n", mode);
        return;
    }
    #endif
}
