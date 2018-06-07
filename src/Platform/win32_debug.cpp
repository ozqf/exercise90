#ifndef WIN32_DEBUG_CPP
#define WIN32_DEBUG_CPP

#define WIN32_DEBUG_ENABLED 0

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "win32_system_include.h"

/****************************************************************
Create a command window for debug output
****************************************************************/

struct Win32_TextInput
{
    char* ptr;
    i32 start;
    i32 position;
    i32 length;
};

Win32_TextInput g_inputText;
u8 g_debugInputActive = 0;
RenderListItem g_rendDebugItem;
RenderScene g_debugScene;

void Win32_ToggleDebugInput()
{
    g_debugInputActive = !g_debugInputActive;
    printf("PLATFORM Debug input active %d\n", g_debugInputActive);
}

char g_lastDebugChar = 0;

void Win32_ResetDebugInput()
{
    COM_ZeroMemory((u8*)g_inputText.ptr, g_inputText.length);
    *(g_inputText.ptr) = '>';
    g_inputText.position = 1;
}

void Win32_DebugReadKey(u32 VKCode, LPARAM lParam)
{
    if (VKCode == 13)
    {
        // execute
        // start at 1 to avoid including the '>' prompt
        // position is also the null terminator
        Win32_ParseTextCommand(g_inputText.ptr + 1, 0, g_inputText.position);
        Win32_ResetDebugInput();
        return;
    }
    // skip unwanted keys
    else if (VKCode == 16)
    {
        return;
    }
    else if (VKCode == 8)
    {
        if (g_inputText.position == 1) { return; }
        --g_inputText.position;
        *(g_inputText.ptr + g_inputText.position) = 0;
    }
    else
    {
        char c = (char)VKCode;
        if (VKCode == 190)
        {
            c = '.';
        }
        *(g_inputText.ptr + g_inputText.position) = c;
        g_inputText.position++;
        *(g_inputText.ptr + g_inputText.position) = 0;
    }

#if 0
    // conversion cack not working yet. Just rip it straight out.
    //if (VKCode == 0) { return; }
    // Backspace: 8
    // Enter: 13
    // Shift: 16
    WORD word;
    i32 result = ToAscii(VKCode, 0, 0, &word, 0);
    
    char c = (char)word;
    //printf("VKCode: %d. Result: %d. Try Key: %c vs %d\n", VKCode, result, c, word);
    //if (c == g_lastDebugChar) { return; }
    g_lastDebugChar = c;
#endif
    //printf("> %c\n", c);

    RendObj_SetAsAsciCharArray(&g_rendDebugItem.obj, g_inputText.ptr, g_inputText.position, 0.05f, 1, 1, 1);
}

void InitDebug()
{
    g_inputText = {};
    g_inputText.start = 1;
    g_inputText.position = g_inputText.start;
    g_inputText.length = 256;
    g_inputText.ptr = (char*)malloc(g_inputText.length);
    Win32_ResetDebugInput();
    //COM_ZeroMemory((u8*)g_inputText.ptr, g_inputText.length);
    //*(g_inputText.ptr) = '>';
    //g_inputText.position = 1;
    // *(g_inputText.ptr + g_inputText.position++) = 'D';
    // *(g_inputText.ptr + g_inputText.position++) = 'E';
    // *(g_inputText.ptr + g_inputText.position++) = 'B';
    // *(g_inputText.ptr + g_inputText.position++) = 'U';
    // *(g_inputText.ptr + g_inputText.position++) = 'G';
    //g_inputText.position = 5;

    // Init debug render elements
    g_rendDebugItem = {};
    g_rendDebugItem.transform.pos.x = -1;
    g_rendDebugItem.transform.pos.y = 1;
    RendObj_SetAsAsciCharArray(&g_rendDebugItem.obj, g_inputText.ptr, 256, 0.05f, 1, 1, 1);

    g_debugScene.settings.projectionMode = RENDER_PROJECTION_MODE_IDENTITY;
    g_debugScene.settings.fov = 90;
    g_debugScene.settings.orthographicHalfHeight = 8;
    g_debugScene.sceneItems = &g_rendDebugItem;
    g_debugScene.numObjects = 1;
    g_debugScene.maxObjects = 1;

#if WIN32_DEBUG_ENABLED

    /**
     * https://justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/
     * Spawns cmd but window is empty and printf doesn't work
     */

    AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    //*stdout = *hf_out;
    // minus stdout define:
    (*__acrt_iob_func(1)) = *hf_out;

    FILE * current = __acrt_iob_func(1);
    *current = *hf_out;

    //*__acrt_iob_func = *hf_out;
	// #define stdout (__acrt_iob_func(1))

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;

#endif
}

#endif