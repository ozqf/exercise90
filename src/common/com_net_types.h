#pragma once

#include "../common/com_module.h"

struct ZNetAddress
{
    u16 ip4Bytes[4];
    u16 port;
};

ZNetAddress COM_LocalHost(u16 port);