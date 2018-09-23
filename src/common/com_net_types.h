#pragma once

#include "../common/com_module.h"

///////////////////////////////////////
// Types
///////////////////////////////////////

struct ZNetAddress
{
    u16 ip4Bytes[4];
    u16 port;
};

struct ZNetConnection
{
    u32 id;
    ZNetAddress address;
};
