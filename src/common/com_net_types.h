#pragma once

#include "../common/common.h"

struct ZNetAddress
{
    u16 ip4Bytes[4];
    u16 port;
};

internal ZNetAddress COM_LocalHost(u16 port)
{
	ZNetAddress addr;
    addr.ip4Bytes[0] = 127;
    addr.ip4Bytes[1] = 0;
    addr.ip4Bytes[2] = 0;
    addr.ip4Bytes[3] = 1;
    addr.port = port;
    return addr;
}
