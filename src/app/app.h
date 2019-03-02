#pragma once

#include "../common/com_module.h"

#define APP_MAX_ENTITIES 512

#define APP_MAX_USERS 16
#define APP_DEFAULT_SOCKET_INDEX 0
#define APP_SERVER_LOOPBACK_PORT 666
#define APP_CLIENT_LOOPBACK_PORT 667

#define APP_DEFAULT_JITTER_TICKS 4

void App_SendTo(i32 socketIndex, ZNetAddress* addr, u8* data, i32 dataSize);
void App_Log(char* msg);
void App_Print(char* msg);
void App_SetMouseMode(ZMouseMode mode);
void App_Error(char* msg, char* heading);
ScreenInfo App_GetScreenInfo();


// comment out to disable logging/printing by app layer
#define APP_FULL_LOGGING

#ifdef APP_FULL_LOGGING
#define APP_LOG(messageBufSize, format, ...) \
{ \
    char appLogBuf[##messageBufSize##]; \
    sprintf_s(appLogBuf, messageBufSize##, format##, ##__VA_ARGS__##); \
    App_Log(appLogBuf); \
}

#define APP_PRINT(messageBufSize, format, ...) \
{ \
    char appLogBuf[##messageBufSize##]; \
    sprintf_s(appLogBuf, messageBufSize##, format##, ##__VA_ARGS__##); \
    App_Print(appLogBuf); \
}
#else
#define APP_LOG(messageBufSize, format, ...)
#define APP_PRINT(messageBufSize, format, ...)
#endif
