#pragma once

#include "../common/com_module.h"

#define APP_MAX_USERS 16
#define APP_DEFAULT_SOCKET_INDEX 0
#define APP_SERVER_LOOPBACK_PORT 666
#define APP_CLIENT_LOOPBACK_PORT 667

ByteBuffer* App_GetLocalClientPacketForRead();
ByteBuffer* App_GetLocalClientPacketForWrite();
ByteBuffer* App_GetLocalServerPacketForRead();
ByteBuffer* App_GetLocalServerPacketForWrite();
