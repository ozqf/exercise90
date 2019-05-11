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

internal AppPerformanceStat g_performanceStats[8];

// 
internal i32 g_fakeLagMinMS = 150;
internal i32 g_fakeLagMaxMS = 200;
// 0 to 1 values.
internal f32 g_fakeLoss = 0.1f;

internal u32 g_lastPlatformFrame = 0;

internal Heap g_heap;

internal i32 g_isRunningClient = 0;
internal i32 g_isRunningServer = 0;

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

#define DEBUG_STRING_LENGTH 8192
#define DEBUG_NUM_STRINGS 8
internal char g_debugStrBuffer[DEBUG_STRING_LENGTH];
internal CharBuffer g_debugStr;
internal RendObj g_debugRendObjs[DEBUG_NUM_STRINGS];

#define MAX_DEBUG_SCENE_ITEMS 64
internal RenderScene g_debugScene;
internal RenderListItem g_debugSceneItems[MAX_DEBUG_SCENE_ITEMS];

internal i32 g_debugDrawServerScene = 0;
internal i32 g_debugDrawServerTests = 0;
internal i32 g_debugDrawClientPredictions = 1;

#define APP_FLAG_PRINT_SPEEDS (1 << 0)
#define APP_FLAG_PRINT_SERVER (1 << 1)
#define APP_FLAG_PRINT_CLIENT (1 << 2)

internal i32 g_debugPrintFlags = 0
    | APP_FLAG_PRINT_SPEEDS
    | APP_FLAG_PRINT_SERVER
    | APP_FLAG_PRINT_CLIENT
;

//#define APP_DEBUG_LOG_FRAME_TIMING

#define APP_MAX_DEBUG_STRING_POSITIONS 4
internal Vec3 g_debugStrPositions[APP_MAX_DEBUG_STRING_POSITIONS] =
{
    // Viewport Screen quarters
    { -1, 1 },
    { -0.334f, 1 },
    { 0.334f, 1 },
    { -1, 0 },
};

internal i32 App_StartSession(i32 sessionType);

void App_FatalError(char* msg, char* heading)
{
    printf("FATAL %s: %s\n", heading, msg);
    ILLEGAL_CODE_PATH
}
