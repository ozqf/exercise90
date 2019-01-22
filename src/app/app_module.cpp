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
internal FakeSocket g_localServerSocket;

// fake socket (port 667)
// Local client reads from this
// Local server sends to this
internal FakeSocket g_localClientSocket;

// World scene
#define MAX_WORLD_SCENE_ITEMS 2048
internal RenderScene g_worldScene;
internal RenderListItem g_worldSceneItems[MAX_WORLD_SCENE_ITEMS];

// Buffer used to feed commands into server every tick
internal ByteBuffer g_serverPlatformInput;

internal ByteBuffer g_loopbackBuffer;

#define APP_MAX_MALLOCS 1024
internal MallocItem g_mallocItems[APP_MAX_MALLOCS];
internal MallocList g_mallocs;

//internal ZNetHandle* g_clientNet;
//internal ZNetHandle* g_serverNet;
internal ZNetAddress g_localServerAddress;


internal i32 App_StartSession(i32 sessionType);

void App_FatalError(char* msg, char* heading)
{
    printf("FATAL %s: %s\n", heading, msg);
    ILLEGAL_CODE_PATH
}

// App wide access to server input buffer... good idea?
// TODO: No checks on whether this buffer is being read!
internal ByteBuffer* GetServerInput() { return &g_serverPlatformInput; }

#include "app_networks.h"
#include "app_platform.h"
