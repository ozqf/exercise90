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

internal PlatformInterface g_platform = {};
internal i32 g_simFrameRate = 1;
internal f32 g_simFrameAcculator = 0;
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

internal i32 App_StartSession(i32 sessionType);

void App_FatalError(char* msg, char* heading)
{
    printf("FATAL %s: %s\n", heading, msg);
    ILLEGAL_CODE_PATH
}

#include "app_networks.h"
#include "app_platform.h"
