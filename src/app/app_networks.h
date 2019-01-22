#pragma once

void App_SendTo(i32 socketIndex, ZNetAddress* addr, u8* data, i32 dataSize)
{
    if (addr->port != APP_CLIENT_LOOPBACK_PORT)
    {
        // TODO: Call Socket for remote sends!
        ILLEGAL_CODE_PATH
    }
    else
    {
        // straight to input
        Sys_WritePacketEvent(&g_loopbackBuffer, socketIndex, addr, data, dataSize);

        //printf("APP Sending %d bytes to Client loopback\n", dataSize);
        //COM_PrintBytes(g_loopbackBuffer.ptrStart, g_loopbackBuffer.Written(), 16);
        //printf("  %d bytes in platform buffer\n", g_loopbackBuffer.Written());
    }
}

// Client's transmission function
void App_CL_SendTo(i32 connId, u8* data, i32 dataSize)
{
    //ZNet_SendData(g_clientNet, connId, data, (u16)dataSize, 0);
}

#if 0
/*
Functions related to routing client and server network calls.
Sockets opened on local loopback ports are redirected by this layer
to a local store read by the connections counterpart.

Intention is that even in single player, the client and server
modules should be unaware of each other and always believe they are
talking through a network to remote instances

*/
#include "app.h"
#include "../network/znet_interface.h"

////////////////////////////////////////////////////////////////////////
// Client - Platform functions
////////////////////////////////////////////////////////////////////////
internal i32 App_CLNet_Init()
{
    return g_platform.Net_Init();
}

internal i32 App_CLNet_Shutdown()
{
    return g_platform.Net_Shutdown();
}

internal i32 App_CLNet_OpenSocket(u16 port, u16* portOpened)
{
    if (port == APP_CLIENT_LOOPBACK_PORT)
    {
        *portOpened = APP_CLIENT_LOOPBACK_PORT;
        return COM_ERROR_NONE;
    }
    return g_platform.Net_OpenSocket(port, portOpened);
}

internal i32 App_CLNet_CloseSocket(i32 socketIndex)
{
    return g_platform.Net_CloseSocket(socketIndex);
}

internal i32 App_CLNet_Read(i32 socketIndex, ZNetAddress* sender,  MemoryBlock* dataPtr)
{
    //printf("CL Read Socket %d\n", socketIndex);

    u8* bytes;
    i32 numBytes;
    g_localClientSocket.Read(&bytes, &numBytes, sender);
    if (bytes != NULL)
    {
        dataPtr->ptrMemory = (void*)bytes;
        dataPtr->size = numBytes;
        //printf("  CL READING %d BYTES\n", numBytes);
        *sender = {};
        sender->port = APP_SERVER_LOOPBACK_PORT;
        return numBytes;
    }
    return 0;
    //return g_platform.Net_Read(socketIndex, sender, dataPtr);
}

internal i32  App_CLNet_SendTo(
    i32 transmittingSocketIndex, ZNetAddress* address, u16 port, u8* data, i32 dataSize)
{
    if (port == APP_SERVER_LOOPBACK_PORT)
    {
        //printf(" Client sending %d bytes on loopback port\n", dataSize);
        g_localServerSocket.SendPacket(address, (u8*)data, (u16)dataSize);
        return COM_ERROR_NONE;
    }
    printf(" Client sending %d bytes on port %d\n", dataSize, port);
    return g_platform.Net_SendTo(transmittingSocketIndex, address, port, data, dataSize);
}

internal void App_CLNet_FatalError(char* msg, char* heading)
{
    g_platform.Platform_Error(msg, heading);
}

////////////////////////////////////////////////////////////////////////
// Client - output callbacks
////////////////////////////////////////////////////////////////////////
internal void App_CLNet_ConnectionAccepted(ZNetConnectionInfo* conn)
{

}

internal void App_CLNet_ConnectionDropped(ZNetConnectionInfo* conn)
{

}

internal void App_CLNet_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes)
{
    printf("CL Received %d of data from conn %d (%d.%d.%d.%d:%d)\n",
        numBytes,
        info->sender.id,
        info->sender.address.ip4Bytes[0],
        info->sender.address.ip4Bytes[1],
        info->sender.address.ip4Bytes[2],
        info->sender.address.ip4Bytes[3],
        info->sender.address.port);
}

internal void App_CLNet_DeliveryConfirmed(ZNetConnectionInfo* info, u32 packetNumber)
{

}

////////////////////////////////////////////////////////////////////////
// Client - build objects
////////////////////////////////////////////////////////////////////////
internal ZNetPlatformFunctions App_CLNet_CreatePlatformFunctions()
{
    ZNetPlatformFunctions funcs = {};
    funcs.Init = App_CLNet_Init;
    funcs.Shutdown = App_CLNet_Shutdown;
    funcs.OpenSocket = App_CLNet_OpenSocket;
    funcs.CloseSocket = App_CLNet_CloseSocket;
    funcs.Read = App_CLNet_Read;
    funcs.SendTo = App_CLNet_SendTo;
    funcs.FatalError = App_CLNet_FatalError;
    return funcs;
}

internal ZNetOutputInterface App_CLNet_CreateOutputFunctions()
{
    ZNetOutputInterface funcs = {};
    funcs.ConnectionAccepted = App_CLNet_ConnectionAccepted;
    funcs.ConnectionDropped = App_CLNet_ConnectionDropped;
    funcs.DataPacketReceived = App_CLNet_DataPacketReceived;
    funcs.DeliveryConfirmed = App_CLNet_DeliveryConfirmed;
    return funcs;
}

////////////////////////////////////////////////////////////////////////
// Server - Platform functions
////////////////////////////////////////////////////////////////////////
internal i32 App_SVNet_Init()
{
    return g_platform.Net_Init();
}

internal i32 App_SVNet_Shutdown()
{
    return g_platform.Net_Shutdown();
}

internal i32 App_SVNet_OpenSocket(u16 port, u16* portOpened)
{
    if (port == APP_SERVER_LOOPBACK_PORT)
    {
        *portOpened = APP_SERVER_LOOPBACK_PORT;
        return COM_ERROR_NONE;
    }
    return g_platform.Net_OpenSocket(port, portOpened);
}

internal i32 App_SVNet_CloseSocket(i32 socketIndex)
{
    return g_platform.Net_CloseSocket(socketIndex);
}

internal i32 App_SVNet_Read(i32 socketIndex, ZNetAddress* sender,  MemoryBlock* dataPtr)
{
    //printf("SV Read Socket %d\n", socketIndex);

    u8* bytes;
    i32 numBytes;
    g_localServerSocket.Read(&bytes, &numBytes, sender);
    if (bytes != NULL)
    {
        dataPtr->ptrMemory = (void*)bytes;
        dataPtr->size = numBytes;
        *sender = {};
        sender->port = APP_CLIENT_LOOPBACK_PORT;
        //printf("  SV READING %d BYTES\n", numBytes);
        return numBytes;
    }
    return 0;
    //return g_platform.Net_Read(socketIndex, sender, dataPtr);
}

internal i32  App_SVNet_SendTo(
    i32 transmittingSocketIndex,
    ZNetAddress* address, u16 port, u8* data, i32 dataSize)
{
    if (port == APP_CLIENT_LOOPBACK_PORT)
    {
        printf(" Server sending %d bytes on loopback port\n", dataSize);
        g_localClientSocket.SendPacket(address, (u8*)data, (u16)dataSize);
        return COM_ERROR_NONE;
    }
    printf(" Server sending %d bytes on port %d\n", dataSize, port);
    return 0;
    //return g_platform.Net_SendTo(transmittingSocketIndex, address, port, data, dataSize);
}

internal void App_SVNet_FatalError(char* msg, char* heading)
{
    g_platform.Platform_Error(msg, heading);
}

////////////////////////////////////////////////////////////////////////
// Server - output callbacks
////////////////////////////////////////////////////////////////////////
internal void App_SVNet_ConnectionAccepted(ZNetConnectionInfo* conn)
{
    CmdUserJoined cmd;
    Cmd_InitUserJoined(&cmd, 0, 0, conn->id);
    Cmd_WriteToByteBuffer(GetServerInput(), &cmd.header);
}

internal void App_SVNet_ConnectionDropped(ZNetConnectionInfo* conn) { }
internal void App_SVNet_DataPacketReceived(ZNetPacketInfo* info, u8* bytes, u16 numBytes) { }
internal void App_SVNet_DeliveryConfirmed(ZNetConnectionInfo* info, u32 packetNumber) { }

////////////////////////////////////////////////////////////////////////
// Server - build objects
////////////////////////////////////////////////////////////////////////
internal ZNetPlatformFunctions App_SVNet_CreatePlatformFunctions()
{
    ZNetPlatformFunctions funcs = {};
    funcs.Init = App_SVNet_Init;
    funcs.Shutdown = App_SVNet_Shutdown;
    funcs.OpenSocket = App_SVNet_OpenSocket;
    funcs.CloseSocket = App_SVNet_CloseSocket;
    funcs.Read = App_SVNet_Read;
    funcs.SendTo = App_SVNet_SendTo;
    funcs.FatalError = App_SVNet_FatalError;
    return funcs;
}

internal ZNetOutputInterface App_SVNet_CreateOutputFunctions()
{
    ZNetOutputInterface funcs = {};
    funcs.ConnectionAccepted = App_SVNet_ConnectionAccepted;
    funcs.ConnectionDropped = App_SVNet_ConnectionDropped;
    funcs.DataPacketReceived = App_SVNet_DataPacketReceived;
    funcs.DeliveryConfirmed = App_SVNet_DeliveryConfirmed;
    return funcs;
}
#endif
