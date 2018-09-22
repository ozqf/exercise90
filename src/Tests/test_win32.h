#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include "../common/com_module.h"

#include "../platform/win32_net/win32_net_interface.h"
#include "../platform/win32_net/win32_net_module.cpp"

#if 0
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#endif

#if 0
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
#endif

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

    Net_RunLoopbackTest();

    //Net_RunLoopbackTest();
    #if 0
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
