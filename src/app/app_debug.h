#pragma once

#include "app_internal.h"

internal void App_DebugInit()
{
    RScene_Init(
        &g_debugScene, g_debugSceneItems, MAX_DEBUG_SCENE_ITEMS,
        90, RENDER_PROJECTION_MODE_IDENTITY, 8);
    g_debugStr.Set(g_debugStrBuffer, DEBUG_STRING_LENGTH);
    /*
    // Server debug string
    g_serverDebugStr = {};
    g_serverDebugStr.Set(g_serverDebugStrBuffer, DEBUG_STRING_LENGTH);
    //g_serverDebugStr.chars = g_serverDebugStrBuffer;
    //g_serverDebugStr.maxLength = DEBUG_STRING_LENGTH;
    
    g_serverDebugStr.cursor += sprintf_s(
        g_serverDebugStr.cursor,
        g_serverDebugStr.Space(),
        "Server Debug Text"
        );
    */
    /*RendObj_SetAsAsciCharArray(
        &g_serverDebugStrRenderer,
        g_serverDebugStr.chars,
        g_serverDebugStr.Written(),
        0.05f,
        TEXT_ALIGNMENT_TOP_LEFT,
        Tex_GetTextureIndexByName(DEFAULT_CONSOLE_CHARSET_PATH),
        0, 1, 1
    );*/
    /*
    // Server client string
    g_clientDebugStr = {};
    g_clientDebugStr.Set(g_clientDebugStrBuffer, DEBUG_STRING_LENGTH);
    //g_clientDebugStr.chars = g_clientDebugStrBuffer;
    //g_clientDebugStr.maxLength = DEBUG_STRING_LENGTH;
    */
   /*
    g_clientDebugStr.cursor += sprintf_s(
        g_clientDebugStr.cursor,
        g_clientDebugStr.Space(),
        "Client Debug Text"
        );

    RendObj_SetAsAsciCharArray(
        &g_clientDebugStrRenderer,
        g_clientDebugStr.chars,
        g_clientDebugStr.Written(),
        0.05f,
        TEXT_ALIGNMENT_TOP_LEFT,
        Tex_GetTextureIndexByName(DEFAULT_CONSOLE_CHARSET_PATH),
        0, 1, 1
    );
    */
}

internal void App_SetStringRenderObj(RendObj* obj, CharBuffer* str)
{
    RendObj_SetAsAsciCharArray(
        obj,
        str->chars,
        str->Written(),
        0.05f,
        TEXT_ALIGNMENT_TOP_LEFT,
        Tex_GetTextureIndexByName(DEFAULT_CONSOLE_CHARSET_PATH),
        0, 1, 1
    );
}

internal void App_WriteSpeeds(CharBuffer* str)
{
    str->cursor += sprintf_s(
        str->cursor,
        str->Space(),
        "-APP TIME %.3fms-\nSV Speeds:\n\tInput %.3fms\n\tSim %.3fms\n\tOutput %.3fms\n",
        App_GetPerformanceTime(APP_STAT_FRAME_TOTAL),
        App_GetPerformanceTime(APP_STAT_SV_INPUT),
        App_GetPerformanceTime(APP_STAT_SV_SIM),
        App_GetPerformanceTime(APP_STAT_SV_OUTPUT)
    );
    // TODO: Move this stuff out to app layer
    str->cursor += sprintf_s(
        str->cursor,
        str->Space(),
        "CL Speeds:\n\tInput %.3fms\n\tSim %.3fms\n\tOutput %.3fms\n",
        App_GetPerformanceTime(APP_STAT_CL_INPUT),
        App_GetPerformanceTime(APP_STAT_CL_SIM),
        App_GetPerformanceTime(APP_STAT_CL_OUTPUT)
    );

}

internal void App_WriteDebugStrings()
{
	g_debugScene.numObjects = 0;
    g_debugStr.Reset();
    // Transform position on screen
    Transform t = {};
    t.pos.y = 1;
    t.pos.x = -1;
    CharBuffer speedsSub;
    CharBuffer serverSub;
    RenderListItem* r = NULL;
    if (g_debugPrintFlags & APP_FLAG_PRINT_SPEEDS)
    {
        speedsSub = g_debugStr.StartSubSection();
        App_WriteSpeeds(&speedsSub);
        g_debugStr.EndSubSection(&speedsSub);
        r = RScene_AssignNextItem(&g_debugScene);
        r->transform = t;
        App_SetStringRenderObj(&r->obj, &speedsSub);
        t.pos.x++;
    }

    if (g_debugPrintFlags & APP_FLAG_PRINT_SERVER)
    {
        serverSub = g_debugStr.StartSubSection();
        SV_WriteDebugString(&serverSub);
        g_debugStr.EndSubSection(&serverSub);
        r = RScene_AssignNextItem(&g_debugScene);
        r->transform = t;
        App_SetStringRenderObj(&r->obj, &serverSub);
        t.pos.x++;
    }

    if (g_debugPrintFlags & APP_FLAG_PRINT_CLIENT)
    {
        CharBuffer sub = g_debugStr.StartSubSection();
        CL_WriteDebugString(&sub);
        g_debugStr.EndSubSection(&sub);
        r = RScene_AssignNextItem(&g_debugScene);
        r->transform = t;
        App_SetStringRenderObj(&r->obj, &sub);
        t.pos.x++;
    }
}
