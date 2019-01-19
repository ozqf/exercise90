#pragma once

#include "../common/com_module.h"

#define NET_CONNECTION_STATE_NONE 0
#define NET_CONNECTION_STATE_REQUESTING 1
#define NET_CONNECTION_STATE_CHALLENGING 2
#define NET_CONNECTION_STATE_RESPONDING 3
#define NET_CONNECTION_STATE_CONNECTED 4

#define NET_MAX_PACKET_SIZE 1400

struct NetConnection
{
	i32 socketId;
	ZNetAddress remoteAddress;
	i32 id;
	i32 state;

	i32 request;
	i32 challenge;
};

struct NetServer
{
	i32 socketIndex;
	NetConncection* connections;
	i32 maxConnections;

};

struct NetPacketBuffer
{
	ZNetAddress address;
	u8 data[NET_MAX_PACKET_SIZE];
	u8 dataSize;
};
