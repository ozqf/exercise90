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

char g_textCommandInput[2048];

KeyConversion g_keyConversions[2];

Win32_TextInput g_inputText;
u8 g_textBufferAwaitingProcessing = 0;
u8 g_debugInputActive = 0;

#define WIN32_NUM_DEBUG_ITEMS 2
// 0 = background, 1 = console text over background
RenderListItem g_rendDebugItems[WIN32_NUM_DEBUG_ITEMS];
RenderScene g_debugScene;

void Platform_WriteTextCommand(char* ptr)
{
    printf("PLATFORM writing text cmd %s\n", ptr);
    COM_CopyStringLimited(ptr, g_textCommandInput, 2048);
}

void Win32_ToggleDebugInput()
{
    g_debugInputActive = !g_debugInputActive;
    //printf("PLATFORM Debug input active %d\n", g_debugInputActive);
}

void Win32_SetDebugInputTextureIndex(i32 index)
{
    RendObj_AsciCharArray* c = &g_rendDebugItems[1].obj.data.charArray;
    c->textureIndex = index;
    printf("PLATFORM Set Console texture index %d\n", index);
}

char g_lastDebugChar = 0;

void Win32_ResetDebugInput()
{
    COM_ZeroMemory((u8*)g_inputText.ptr, g_inputText.length);
    *(g_inputText.ptr) = '>';
    g_inputText.position = 1;
}

void Win32_CheckTextBuffer()
{
    // Check user debug text input
    if (g_textBufferAwaitingProcessing)
    {
        g_textBufferAwaitingProcessing = 0;
        // execute
        // start at 1 to avoid including the '>' prompt
        // position is also the null terminator
        Win32_ParseTextCommand(g_inputText.ptr + 1, 0, g_inputText.position);
        Win32_ResetDebugInput();
    }

    // Check internal text command input
    i32 len = COM_StrLen(g_textCommandInput);
    if (len > 0)
    {
        Win32_ParseTextCommand(g_textCommandInput, 0, len);
        COM_ZeroMemory((u8*)g_textCommandInput, 2048);
    }
}

#define VK_CODE_BACKSPACE 8
#define VK_CODE_ENTER 13
#define VK_CODE_SHIFT 16
#define VK_CODE_SPACE 32

#define VK_CODE_SQUARE_BRACKET_LEFT 219
#define VK_CODE_SQUARE_BRACKET_RIGHT 221
#define VK_CODE_DASH 189
#define VK_CODE_FULL_STOP 190
#define VK_CODE_EQUALS 187
#define VK_CODE_HASH 222

u32 Win32_ConvertVKCode(u32 VKCode, u16 shift)
{
    KeyConversion* k = &g_keyConversions[0];
    while (k->VKCode != NULL)
    {
        if (k->VKCode == VKCode)
        {
            return shift > 1 ? k->shiftChar : k->baseChar;
        }
        ++k;
    }
    return VKCode;
}

void Win32_DebugReadKey(u32 VKCode, WPARAM wParam, LPARAM lParam)
{
    if (g_textBufferAwaitingProcessing) { return; }
    u16 shift = GetKeyState(VK_CODE_SHIFT);
    //u16 word;
    //u8 c = (u8)ToAscii(VKCode, 0, 0, &word, 0);
    //printf("%d to asci %d\n", VKCode, c);



    // translate specific keys
    VKCode = (i32)Win32_ConvertVKCode(VKCode, shift);

    //printf("Shift state: %d\n", shift);
    if (VKCode == VK_CODE_ENTER)
    {
        // Flag buffer as locked and awaiting reading
        g_textBufferAwaitingProcessing = 1;
        Win32_ToggleDebugInput();
        return;
    }
    // skip unwanted keys
    else if (VKCode == VK_CODE_SHIFT)
    {
        printf(" PLAT Shift pressed\n");
        return;
    }
    else if (VKCode == VK_CODE_BACKSPACE)
    {
        if (g_inputText.position == 1) { return; }
        --g_inputText.position;
        *(g_inputText.ptr + g_inputText.position) = 0;
    }
    else
    {
        u8 c = (u8)VKCode;
        printf("PLAT read keycode %d wParam: %d\n", c, wParam);
        // if (VKCode == 190)
        // {
        //     c = '.';
        // }
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
    // ,: 190
    WORD word;
    i32 result = ToAscii(VKCode, 0, 0, &word, 0);
    
    char c = (char)word;
    //printf("VKCode: %d. Result: %d. Try Key: %c vs %d\n", VKCode, result, c, word);
    //if (c == g_lastDebugChar) { return; }
    g_lastDebugChar = c;
#endif
    //printf("> %c\n", c);

    RendObj_SetAsAsciCharArray(&g_rendDebugItems[1].obj, g_inputText.ptr, g_inputText.position, 0.05f, 1, 1, 1, 1);
}

void InitDebug()
{
    g_keyConversions[0].VKCode = VK_CODE_DASH;
    g_keyConversions[0].baseChar = '-';
    g_keyConversions[0].shiftChar = '_';

    g_keyConversions[1].VKCode = VK_CODE_FULL_STOP;
    g_keyConversions[1].baseChar = '.';
    g_keyConversions[1].shiftChar = '>';

    g_keyConversions[2].VKCode = NULL;
    
    g_inputText = {};
    g_inputText.start = 1;
    g_inputText.position = g_inputText.start;
    g_inputText.length = 256;
    g_inputText.ptr = (char*)malloc(g_inputText.length);
    Win32_ResetDebugInput();
    
    // Init debug render elements
    RenderListItem* item;

    // Input Text BG
    item = &g_rendDebugItems[0];
    *item = {};
    item->transform.pos.x = 0;
    item->transform.pos.y = 1;
    item->transform.scale.x = 1;
    item->transform.scale.y = 0.05f;
    item->transform.scale.z = 1;
    RendObj_SetAsColouredQuad(&item->obj, 0, 0, 0);
    //RendObj_SetAsAsciCharArray(&item->obj, g_inputText.ptr, 256, 0.05f, 1, 1, 1, 1);

    
    // Input Text
    item = &g_rendDebugItems[1];
    *item = {};
    item->transform.pos.x = -1;
    item->transform.pos.y = 1;
    RendObj_SetAsAsciCharArray(&item->obj, g_inputText.ptr, 256, 0.05f, 1, 1, 1, 1);

    g_debugScene.settings.projectionMode = RENDER_PROJECTION_MODE_IDENTITY;
    g_debugScene.settings.fov = 90;
    g_debugScene.settings.orthographicHalfHeight = 8;
    g_debugScene.sceneItems = g_rendDebugItems;
    g_debugScene.numObjects = 2;
    g_debugScene.maxObjects = 2;


}

#endif