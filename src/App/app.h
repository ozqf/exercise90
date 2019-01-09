#pragma once

#include "../common/com_module.h"

ByteBuffer* App_GetLocalClientPacketForRead();
ByteBuffer* App_GetLocalClientPacketForWrite();
ByteBuffer* App_GetLocalServerPacketForRead();
ByteBuffer* App_GetLocalServerPacketForWrite();
