#pragma once

#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include "app.h"
#include "server/server.h"
#include "client/client.h"
#include "app_textures.h"
#include "../network/znet_interface.h"

#define APP_SESSION_TYPE_NONE 0
#define APP_SESSION_TYPE_SINGLE_PLAYER 1

internal PlatformInterface g_platform = {};
internal i32 g_simFrameRate = 5;
internal f32 g_simFrameAcculator = 0;
internal Heap g_heap;

internal i32 g_isRunningClient = 0;
internal i32 g_isRunningServer = 0;

// World scene
#define MAX_WORLD_SCENE_ITEMS 2048
internal RenderScene g_worldScene;
internal RenderListItem g_worldSceneItems[MAX_WORLD_SCENE_ITEMS];

internal ByteBuffer g_localClientPacket;
internal ByteBuffer g_localServerPacket;

#define APP_MAX_MALLOCS 1024
internal MallocItem g_mallocItems[APP_MAX_MALLOCS];
internal MallocList g_mallocs;

internal ZNetHandle* g_clientNet;
internal ZNetHandle* g_serverNet;
internal ZNetAddress g_localClientAddress = {};


internal i32 App_StartSession(i32 sessionType);

void App_FatalError(char* msg, char* heading)
{
    printf("FATAL %s: %s\n", heading, msg);
    ILLEGAL_CODE_PATH
}

#include "app_networks.h"
#include "app_platform.h"
