#pragma once

#include "../common/com_module.h"

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

void Input_InitAction(InputAction* action, u32 keyCode, char* label)
{
    action->keyCode = keyCode;
    action->value = 0;
    action->lastFrame = 0;
    COM_CopyStringLimited(label, action->label, 16);
}

// Find an action... duh
inline InputAction* Input_FindAction(InputAction* actions, i32 numActions, char* name)
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

i32 Input_CheckAction(InputAction* actions, i32 numActions, char* actionName, u32 frameNumber)
{
    InputAction* action = Input_FindAction(actions, numActions, actionName);
    Assert(action != NULL);
    return action->value;
}

u8 Input_CheckActionToggledOn(InputAction* actions, i32 numActions, char* actionName, u32 frameNumber)
{
    InputAction* action = Input_FindAction(actions, numActions, actionName);
    Assert(action != NULL);
    
    return (action->value != 0 && action->lastFrame == frameNumber);
}

u8 Input_CheckActionToggledOff(InputAction* actions, i32 numActions, char* actionName, u32 frameNumber)
{
    InputAction* action = Input_FindAction(actions, numActions, actionName);
    Assert(action != NULL);
    
    return (action->value == 0 && action->lastFrame == frameNumber);
}

// Test an input event vs actions array. Return an input if it has changed, NULL if nothing changed
InputAction* Input_TestForAction(InputAction* actions, i32 numActions, i32 inputValue, u32 inputKeyCode, u32 frameNumber)
{
    for (i32 i = 0; i < numActions; ++i)
    {
        InputAction* action = &actions[i];
        if (action->keyCode == inputKeyCode && action->value != inputValue)
        {
            action->value = inputValue;
            action->lastFrame = frameNumber;
            return action;
        }
    }
    return NULL;
}
