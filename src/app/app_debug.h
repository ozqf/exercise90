#pragma once

#include "app_internal.h"

internal void App_DebugInit()
{
    RScene_Init(
        &g_debugScene, g_debugSceneItems, MAX_DEBUG_SCENE_ITEMS,
        90, RENDER_PROJECTION_MODE_IDENTITY, 8);

    // Server debug string
    g_serverDebugStr = {};
    g_serverDebugStr.chars = g_serverDebugStrBuffer;
    g_serverDebugStr.maxLength = DEBUG_STRING_LENGTH;

    g_serverDebugStr.length = sprintf_s(
        g_serverDebugStr.chars,
        g_serverDebugStr.maxLength,
        "Server Debug Text"
        );

    RendObj_SetAsAsciCharArray(
        &g_serverDebugStrRenderer,
        g_serverDebugStr.chars,
        g_serverDebugStr.length,
        0.05f,
        TEXT_ALIGNMENT_TOP_LEFT,
        Tex_GetTextureIndexByName("textures\\charset.bmp"),
        0, 1, 1
    );

    // Server client string
    g_clientDebugStr = {};
    g_clientDebugStr.chars = g_clientDebugStrBuffer;
    g_clientDebugStr.maxLength = DEBUG_STRING_LENGTH;

    g_clientDebugStr.length = sprintf_s(
        g_clientDebugStr.chars,
        g_clientDebugStr.maxLength,
        "Client Debug Text"
        );

    RendObj_SetAsAsciCharArray(
        &g_clientDebugStrRenderer,
        g_clientDebugStr.chars,
        g_clientDebugStr.length,
        0.05f,
        TEXT_ALIGNMENT_TOP_LEFT,
        Tex_GetTextureIndexByName("textures\\charset.bmp"),
        0, 1, 1
    );
}

internal void App_WriteDebugStrings()
{
    SV_WriteDebugString(&g_serverDebugStr);
    CL_WriteDebugString(&g_clientDebugStr);

    g_debugScene.numObjects = 0;
    // Transform position on screen
    Transform t = {};
    t.pos.x = -1;
    t.pos.y = 1;
    RScene_AddRenderItem(&g_debugScene, &t, &g_serverDebugStrRenderer);

    t.pos.x = 0;
    RScene_AddRenderItem(&g_debugScene, &t, &g_clientDebugStrRenderer);
}
