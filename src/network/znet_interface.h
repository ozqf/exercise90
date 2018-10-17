#ifndef ZNET_INTERFACE_H
#define ZNET_INTERFACE_H

#include "../common/com_module.cpp"

// TODO: Also in game.h at the moment, move this to a single location!

// MODE      | ID        SERVER      CLIENT      CONNECTION
//--------------------------------------------------------------
// SP        |  0          1           1           0
// Client    |  1          0           1           1
// Listen    |  2          1           1           1
// Dedicated |  3          1           0           1
// Replay    |  4          0           1           0

#define NETMODE_NONE 0
#define NETMODE_SINGLE_PLAYER 1
#define NETMODE_CLIENT 2
#define NETMODE_LISTEN_SERVER 3
#define NETMODE_DEDICATED_SERVER 4
#define NETMODE_REPLAY 5

#define ZNET_LOCAL_ADDRESS "127.0.0.1"

/////////////////////////////////////////////////////////////////
// Data types
/////////////////////////////////////////////////////////////////
struct ZNetConnectionInfo
{
	ZNetAddress address;
	// should be kept private between this specific client and server
	i32 id;
	// public client id is how the game and other clients can refer to each other.
    // eg players 0, 1, 2. but their actual *connection* ids are not exposed to other clients.
	i32 publicId;
};

struct ZNetPacketInfo
{
	ZNetConnectionInfo sender;
	u32 remoteSequence;
};

/////////////////////////////////////////////////////////////////
// Interfaces
// These are NOT CHECKED when used. Make sure you're sending a valid struct
/////////////////////////////////////////////////////////////////

// Required external OS interface
struct ZNetPlatformFunctions
{
    i32  (*Init)            ();
    i32  (*Shutdown)        ();
    i32  (*OpenSocket)      (u16 port, u16* portOpened);
    i32  (*CloseSocket)     (i32 socketIndex);
    i32  (*Read)            (i32 socketIndex, ZNetAddress* sender,  MemoryBlock* dataPtr);
    i32  (*SendTo)          (i32 transmittingSocketIndex, char* address, u16 port, char* data, i32 dataSize);

    void (*FatalError)      (char* msg, char* heading);
};

// Event callbacks
struct ZNetOutputInterface
{
	void (*ConnectionAccepted)		(ZNetConnectionInfo* conn);
	void (*ConnectionDropped)		(ZNetConnectionInfo* conn);
	void (*DataPacketReceived)		(ZNetPacketInfo* info, u8* bytes, u16 numBytes);
	void (*DeliveryConfirmed)		(u32 packetNumber);
};

u32 ZNet_SendData(i32 connId, u8* bytes, u16 numBytes, i32 printSendInfo);

// info
void ZNet_Info();
i32 ZNet_IsServer();

// system lifetime
void ZNet_Init(ZNetPlatformFunctions functions, ZNetOutputInterface output);
void ZNet_Shutdown();

// session lifetime
void ZNet_StartSession(u8 netMode, ZNetAddress* serverAddress, u16 localPort);
void ZNet_EndSession();
i32 ZNet_Tick(f32 deltaTime);

// ZNET_INTERFACE_H
#endif
