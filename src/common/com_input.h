#pragma once

#include "com_module.h"


//////////////////////////////////////////////////////////////////////
// Input codes
//////////////////////////////////////////////////////////////////////
#define Z_INPUT_CODE_NULL                   0
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
#define Z_INPUT_CODE_SHIFT                  35
#define Z_INPUT_CODE_RIGHT_SHIFT            36
#define Z_INPUT_CODE_CONTROL                37
#define Z_INPUT_CODE_RIGHT_CONTROL          38
#define Z_INPUT_CODE_ESCAPE                 39
#define Z_INPUT_CODE_RETURN                 40
#define Z_INPUT_CODE_ENTER                  41
#define Z_INPUT_CODE_0                      42
#define Z_INPUT_CODE_1                      43
#define Z_INPUT_CODE_2                      44
#define Z_INPUT_CODE_3                      45
#define Z_INPUT_CODE_4                      46
#define Z_INPUT_CODE_5                      47
#define Z_INPUT_CODE_6                      48
#define Z_INPUT_CODE_7                      49
#define Z_INPUT_CODE_8                      50
#define Z_INPUT_CODE_9                      51
#define Z_INPUT_CODE_UP                     52
#define Z_INPUT_CODE_DOWN                   53
#define Z_INPUT_CODE_LEFT                   54
#define Z_INPUT_CODE_RIGHT                  55
#define Z_INPUT_CODE_MOUSE_POS_X            56
#define Z_INPUT_CODE_MOUSE_POS_Y            57
#define Z_INPUT_CODE_MOUSE_MOVE_X           58
#define Z_INPUT_CODE_MOUSE_MOVE_Y           59
#define Z_INPUT_CODE_F1                     60
#define Z_INPUT_CODE_F2                     61
#define Z_INPUT_CODE_F3                     62
#define Z_INPUT_CODE_F4                     63
#define Z_INPUT_CODE_F5                     64
#define Z_INPUT_CODE_F6                     65
#define Z_INPUT_CODE_F7                     66
#define Z_INPUT_CODE_F8                     67
#define Z_INPUT_CODE_F9                     68
#define Z_INPUT_CODE_F10                    69
#define Z_INPUT_CODE_F11                    70
#define Z_INPUT_CODE_F12                    71


////////////////////////////////////////////////////////////////////
// Data structures
////////////////////////////////////////////////////////////////////
struct InputAction
{
    u32 keyCode;
    i32 value;
    u32 lastFrame;
    char label[16];
};

struct InputActionSet
{
    InputAction* actions;
    i32 count;
};

internal void Input_InitAction(InputActionSet* actions, u32 keyCode, char* label)
{
    i32 index = actions->count++;
    actions->actions[index].keyCode = keyCode;
    actions->actions[index].value = 0;
    actions->actions[index].lastFrame = 0;
    COM_CopyStringLimited(label, actions->actions[index].label, 16);
}

// Find an action... duh
internal InputAction* Input_FindAction(InputAction* actions, i32 numActions, char* name)
{
    for (i32 i = 0; i < numActions; ++i)
    {
        InputAction* action = &actions[i];
        if (!COM_CompareStrings(action->label, name))
        {
            return action;
        }
    }
    return NULL;
}

internal i32 Input_GetActionValue(InputAction* actions, i32 numActions, char* actionName)
{
    InputAction* action = Input_FindAction(actions, numActions, actionName);
    Assert(action != NULL);
    return action->value;
}

internal i32 Input_GetActionValue(InputActionSet* actions, char* actionName)
{
    return Input_GetActionValue(actions->actions, actions->count, actionName);
}

internal u8 Input_CheckActionToggledOn(InputActionSet* actions, char* actionName, u32 frameNumber)
{
    InputAction* action = Input_FindAction(actions->actions, actions->count, actionName);
    Assert(action != NULL);
    
    return (action->value != 0 && action->lastFrame == frameNumber);
}

internal u8 Input_CheckActionToggledOff(InputActionSet* actions, char* actionName, u32 frameNumber)
{
    InputAction* action = Input_FindAction(actions->actions, actions->count, actionName);
    Assert(action != NULL);
    
    return (action->value == 0 && action->lastFrame == frameNumber);
}

// Test an input event vs actions array. Return an input if it has changed, NULL if nothing changed
internal InputAction* Input_TestForAction(InputActionSet* actions, i32 inputValue, u32 inputKeyCode, u32 frameNumber)
{
	for (i32 i = 0; i < actions->count; ++i)
    {
        InputAction* action = &actions->actions[i];
        if (action->keyCode == inputKeyCode && action->value != inputValue)
        {
            action->value = inputValue;
            action->lastFrame = frameNumber;
            return action;
        }
    }
    return NULL;
}

