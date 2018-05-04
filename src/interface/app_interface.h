#pragma once

#include "../common/com_defines.h"
#include "../common/com_types.h"
#include "platform_interface.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Input codes
//////////////////////////////////////////////////////////////////////
#define Z_INPUT_CODE_MOUSE_1                1
#define Z_INPUT_CODE_MOUSE_2                2
#define Z_INPUT_CODE_MOUSE_3                3
#define Z_INPUT_CODE_MOUSE_4                4
#define Z_INPUT_CODE_MOUSE_5                5
#define Z_INPUT_CODE_MWHEELUP               6
#define Z_INPUT_CODE_MWHEELDOWN             7
#define Z_INPUT_CODE_A                      8
#define Z_INPUT_CODE_B                      9
#define Z_INPUT_CODE_C                      10
#define Z_INPUT_CODE_D                      11
#define Z_INPUT_CODE_E                      12
#define Z_INPUT_CODE_F                      13
#define Z_INPUT_CODE_G                      14
#define Z_INPUT_CODE_H                      15
#define Z_INPUT_CODE_I                      16
#define Z_INPUT_CODE_J                      17
#define Z_INPUT_CODE_K                      18
#define Z_INPUT_CODE_L                      19
#define Z_INPUT_CODE_M                      20
#define Z_INPUT_CODE_N                      21
#define Z_INPUT_CODE_O                      22
#define Z_INPUT_CODE_P                      23
#define Z_INPUT_CODE_Q                      24
#define Z_INPUT_CODE_R                      25
#define Z_INPUT_CODE_S                      26
#define Z_INPUT_CODE_T                      27
#define Z_INPUT_CODE_U                      28
#define Z_INPUT_CODE_V                      29
#define Z_INPUT_CODE_W                      30
#define Z_INPUT_CODE_X                      31
#define Z_INPUT_CODE_Y                      32
#define Z_INPUT_CODE_Z                      33
#define Z_INPUT_CODE_SPACE                  34
#define Z_INPUT_CODE_LEFT_SHIFT             35
#define Z_INPUT_CODE_RIGHT_SHIFT            36
#define Z_INPUT_CODE_LEFT_SHIFT             35
#define Z_INPUT_CODE_RIGHT_SHIFT            36
#define Z_INPUT_CODE_ESCAPE                 37
#define Z_INPUT_CODE_RETURN                 38
#define Z_INPUT_CODE_ENTER                  39
#define Z_INPUT_CODE__0                     40
#define Z_INPUT_CODE__1                     41
#define Z_INPUT_CODE__2                     42
#define Z_INPUT_CODE__3                     43
#define Z_INPUT_CODE__4                     44
#define Z_INPUT_CODE__5                     45
#define Z_INPUT_CODE__6                     46
#define Z_INPUT_CODE__7                     47
#define Z_INPUT_CODE__8                     48
#define Z_INPUT_CODE__9                     49
#define Z_INPUT_CODE_MOUSE_AXIS_X           50
#define Z_INPUT_CODE_MOUSE_AXIS_Y           51

//////////////////////////////////////////////////////////////////////
// System event types
//////////////////////////////////////////////////////////////////////
#define EV_CODE_NULL                        0
#define EV_CODE_INPUT                       1

struct InputEvent
{
    u32 type = EV_CODE_INPUT;
    u32 inputID = 0;
    i32 value = 0;
};

// struct EventHeader
// {
//     u32 type;
//     u32 data;
// };

// Contains pointers to App functions
struct AppInterface
{
    i32 isvalid;
    i32 (*AppInit)();
    i32 (*AppShutdown)();
    i32 (*AppRendererReloaded)();
    void (*AppUpdate)(GameTime* time, ByteBuffer commands, InputTick* tick);
    //void (*AppFixedUpdate)(GameTime* time, InputTick* tick);
};

/*****************************************************
Platform calls DLL with platform function pointers, DLL should return pointers
*****************************************************/
typedef AppInterface (Func_LinkToApp)(PlatformInterface);

// eg. in SDL:
// Func_LinkToApp* linkToApp = (Func_LinkToApp *)SDL_LoadFunction(gameModule, "LinkToApp");
