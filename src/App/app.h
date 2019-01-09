#pragma once

#include "../common/com_module.h"

#define APP_MAX_USERS 16

ByteBuffer* App_GetLocalClientPacketForRead();
ByteBuffer* App_GetLocalClientPacketForWrite();
ByteBuffer* App_GetLocalServerPacketForRead();
ByteBuffer* App_GetLocalServerPacketForWrite();
