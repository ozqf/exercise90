#pragma once


#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "app.h"
#include "server/server.h"
#include "client/client.h"
#include "app_textures.h"
#include "../network/znet_interface.h"
#include "app_fake_socket.h"

#define APP_SESSION_TYPE_NONE 0
#define APP_SESSION_TYPE_SINGLE_PLAYER 1

internal AppPlatform g_platform = {};
internal ScreenInfo g_screenInfo;
internal i32 g_simFrameRate = 60;
internal f32 g_simFrameAcculator = 0;
internal i32 g_fakeLagMinMS = 150;
internal i32 g_fakeLagMaxMS = 250;
internal f32 g_fakeLoss = 0.02f;

internal u32 g_lastPlatformFrame = 0;

internal Heap g_heap;

internal i32 g_isRunningClient = 0;
internal i32 g_isRunningServer = 0;

// fake socket (port 666)
// Local server reads from this
// Local client sends to this
//internal FakeSocket g_localServerSocket;

// fake socket (port 667)
// Local client reads from this
// Local server sends to this
//internal FakeSocket g_localClientSocket;

internal FakeSocket g_loopbackSocket;

internal DoubleByteBuffer g_serverLoopback;
internal DoubleByteBuffer g_clientLoopback;

#define APP_MAX_MALLOCS 1024
internal MallocItem g_mallocItems[APP_MAX_MALLOCS];
internal MallocList g_mallocs;

#define MAX_WORLD_SCENE_ITEMS 2048
internal RenderScene g_worldScene;
internal RenderListItem g_worldSceneItems[MAX_WORLD_SCENE_ITEMS];

internal ZNetAddress g_localServerAddress;

#define DEBUG_STRING_LENGTH 2048
internal ZStringHeader g_clientDebugStr;
internal char g_clientDebugStrBuffer[DEBUG_STRING_LENGTH];
internal RendObj g_clientDebugStrRenderer;

internal ZStringHeader g_serverDebugStr;
internal char g_serverDebugStrBuffer[DEBUG_STRING_LENGTH];
internal RendObj g_serverDebugStrRenderer;

#define MAX_DEBUG_SCENE_ITEMS 64
internal RenderScene g_debugScene;
internal RenderListItem g_debugSceneItems[MAX_DEBUG_SCENE_ITEMS];

//#define APP_DEBUG_LOG_FRAME_TIMING

internal i32 App_StartSession(i32 sessionType);

void App_FatalError(char* msg, char* heading)
{
    printf("FATAL %s: %s\n", heading, msg);
    ILLEGAL_CODE_PATH
}
