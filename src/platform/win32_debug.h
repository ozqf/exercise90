#ifndef WIN32_DEBUG_CPP
#define WIN32_DEBUG_CPP

#define WIN32_DEBUG_ENABLED 0

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "win32_system_include.h"

#include "win32_module.cpp"

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

KeyConversion g_keyConversions[32];

#define VK_CODE_BACKSPACE 8
#define VK_CODE_ENTER 13
#define VK_CODE_SHIFT 16
#define VK_CODE_SPACE 32

#define VK_CODE_0 48
#define VK_CODE_1 49
#define VK_CODE_2 50
#define VK_CODE_3 51
#define VK_CODE_4 52
#define VK_CODE_5 53
#define VK_CODE_6 54
#define VK_CODE_7 55
#define VK_CODE_8 56
#define VK_CODE_9 57

#define VK_CODE_SEMICOLON 186
#define VK_CODE_EQUALS 187
#define VK_CODE_COMMA 188
#define VK_CODE_DASH 189
#define VK_CODE_FULL_STOP 190
#define VK_CODE_SQUARE_FORWARDSLASH 191
#define VK_CODE_SINGLE_QUOTE 192
#define VK_CODE_SQUARE_BRACKET_LEFT 219
#define VK_CODE_SQUARE_BACKSLASH 220
#define VK_CODE_SQUARE_BRACKET_RIGHT 221
#define VK_CODE_HASH 222

u32 Win32_ConvertVKCode(u32 VKCode, u16 shift)
{
    KeyConversion* k = &g_keyConversions[0];
    while (k->VKCode != NULL)
    {
        if (k->VKCode == VKCode)
        {
			u32 result = shift > 1 ? k->shiftChar : k->baseChar;
			//printf("Converted %d to %d\n", VKCode, result);
			//return result;
            return shift > 1 ? k->shiftChar : k->baseChar;
        }
        ++k;
    }
    return VKCode;
}

Win32_TextInput g_inputText;
u8 g_debugPrintKeycodes = 0;
u8 g_textBufferAwaitingProcessing = 0;
u8 g_debugInputActive = 0;

#define WIN32_NUM_DEBUG_ITEMS 2
// 0 = background, 1 = console text over background
RenderListItem g_rendDebugItems[WIN32_NUM_DEBUG_ITEMS];
RenderScene g_debugScene;

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

void Win32_ToggleDebugInput()
{
    g_debugInputActive = !g_debugInputActive;
    
    if (g_debugInputActive == 1)
    {
        Win32_ResetDebugInput();
    }
}

void Win32_DebugReadKey(u32 VKCode, WPARAM wParam, LPARAM lParam)
{
    if (g_textBufferAwaitingProcessing) { return; }
    u16 shift = GetKeyState(VK_CODE_SHIFT);
    if (g_debugPrintKeycodes)
    {
        u16 word;
        u8 c = (u8)ToAscii(VKCode, 0, 0, &word, 0);
        printf("%d to asci %d\n", VKCode, c);
    }
    
    
    // translate specific keys
    VKCode = (i32)Win32_ConvertVKCode(VKCode, shift);

    //printf("Shift state: %d\n", shift);
    if (VKCode == VK_CODE_ENTER)
    {
        // Flag buffer as locked and awaiting reading
        //g_textBufferAwaitingProcessing = 1;
        Win32_ToggleDebugInput();
        Win32_EnqueueTextCommand(g_inputText.ptr + 1);
        return;
    }
    // skip unwanted keys
    else if (VKCode == VK_CODE_SHIFT)
    {
        //printf(" PLAT Shift pressed\n");
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
        //printf("PLAT read keycode %d wParam: %d\n", c, wParam);
        // if (VKCode == 190)
        // {
        //     c = '.';
        // }
        *(g_inputText.ptr + g_inputText.position) = c;
        g_inputText.position++;
        *(g_inputText.ptr + g_inputText.position) = 0;
    }
	//printf("Input pos: %d\n", g_inputText.position);

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

    RendObj_SetAsAsciCharArray(&g_rendDebugItems[1].obj, g_inputText.ptr, g_inputText.position, 0.05f, TEXT_ALIGNMENT_TOP_LEFT, 1, 1, 1, 1);
}


//////////////////////////////////////////////////////////////////
// Attempts to handle C runtime errors. At least, according to
// MSDN. Doesn't seem to help with game's errors atm
//////////////////////////////////////////////////////////////////
/*
// https://msdn.microsoft.com/en-us/library/a9yf33zb.aspx
void _invalid_parameter(  
   const wchar_t * expression,  
   const wchar_t * function,   
   const wchar_t * file,   
   unsigned int line,  
   uintptr_t pReserved  
);
*/
void Win32_invalid_parameter(  
   const wchar_t * expression,  
   const wchar_t * function,   
   const wchar_t * file,   
   unsigned int line,  
   uintptr_t pReserved  
)
{
    DebugBreak();
    Win32_Error("An invalid parameter was pasted to the C runtime library", "C runtime error");
}

void Win32_AttachErrorHandlers()
{
	_set_invalid_parameter_handler(Win32_invalid_parameter);
	_set_thread_local_invalid_parameter_handler(Win32_invalid_parameter);
}

void InitDebug()
{
	Win32_AttachErrorHandlers();

    g_keyConversions[0].VKCode = VK_CODE_DASH;
    g_keyConversions[0].baseChar = '-';
    g_keyConversions[0].shiftChar = '_';

    g_keyConversions[1].VKCode = VK_CODE_FULL_STOP;
    g_keyConversions[1].baseChar = '.';
    g_keyConversions[1].shiftChar = '>';

    g_keyConversions[2].VKCode = VK_CODE_COMMA;
    g_keyConversions[2].baseChar = ',';
    g_keyConversions[2].shiftChar = '<';

    g_keyConversions[3].VKCode = VK_CODE_SQUARE_BRACKET_LEFT;
    g_keyConversions[3].baseChar = '[';
    g_keyConversions[3].shiftChar = '{';

    g_keyConversions[4].VKCode = VK_CODE_SQUARE_BRACKET_RIGHT;
    g_keyConversions[4].baseChar = ']';
    g_keyConversions[4].shiftChar = '}';

    g_keyConversions[5].VKCode = VK_CODE_HASH;
    g_keyConversions[5].baseChar = '#';
    g_keyConversions[5].shiftChar = '~';

    g_keyConversions[6].VKCode = VK_CODE_SEMICOLON;
    g_keyConversions[6].baseChar = ';';
    g_keyConversions[6].shiftChar = ':';

    g_keyConversions[7].VKCode = VK_CODE_SINGLE_QUOTE;
    g_keyConversions[7].baseChar = '\'';
    g_keyConversions[7].shiftChar = '@';

    g_keyConversions[8].VKCode = VK_CODE_1;
    g_keyConversions[8].baseChar = '1';
    g_keyConversions[8].shiftChar = '!';

    g_keyConversions[9].VKCode = VK_CODE_2;
    g_keyConversions[9].baseChar = '2';
    g_keyConversions[9].shiftChar = '"';

    g_keyConversions[10].VKCode = VK_CODE_3;
    g_keyConversions[10].baseChar = '3';
    g_keyConversions[10].shiftChar = '3';

    g_keyConversions[11].VKCode = VK_CODE_4;
    g_keyConversions[11].baseChar = '4';
    g_keyConversions[11].shiftChar = '$';

    g_keyConversions[12].VKCode = VK_CODE_5;
    g_keyConversions[12].baseChar = '5';
    g_keyConversions[12].shiftChar = '%';

    g_keyConversions[13].VKCode = VK_CODE_6;
    g_keyConversions[13].baseChar = '6';
    g_keyConversions[13].shiftChar = '^';

    g_keyConversions[14].VKCode = VK_CODE_7;
    g_keyConversions[14].baseChar = '7';
    g_keyConversions[14].shiftChar = '&';

    g_keyConversions[15].VKCode = VK_CODE_8;
    g_keyConversions[15].baseChar = '8';
    g_keyConversions[15].shiftChar = '*';

    g_keyConversions[16].VKCode = VK_CODE_9;
    g_keyConversions[16].baseChar = '9';
    g_keyConversions[16].shiftChar = '(';

    g_keyConversions[17].VKCode = VK_CODE_0;
    g_keyConversions[17].baseChar = '0';
    g_keyConversions[17].shiftChar = ')';

    g_keyConversions[18].VKCode = VK_CODE_EQUALS;
    g_keyConversions[18].baseChar = '=';
    g_keyConversions[18].shiftChar = '+';

    g_keyConversions[19].VKCode = VK_CODE_SQUARE_FORWARDSLASH;
    g_keyConversions[19].baseChar = '/';
    g_keyConversions[19].shiftChar = '?';

    g_keyConversions[20].VKCode = VK_CODE_SQUARE_BACKSLASH;
    g_keyConversions[20].baseChar = '\\';
    g_keyConversions[20].shiftChar = '|';

    // Make sure end is null
    g_keyConversions[21].VKCode = NULL;

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
    RendObj_SetAsAsciCharArray(&item->obj, g_inputText.ptr, 256, 0.05f, TEXT_ALIGNMENT_TOP_LEFT, 1, 1, 1, 1);

    g_debugScene.settings.projectionMode = RENDER_PROJECTION_MODE_IDENTITY;
    g_debugScene.settings.fov = 90;
    g_debugScene.settings.orthographicHalfHeight = 8;
    g_debugScene.sceneItems = g_rendDebugItems;
    g_debugScene.numObjects = 2;
    g_debugScene.maxObjects = 2;


}

#endif