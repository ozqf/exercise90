#pragma once

#include "../common/com_module.h"

struct FileSegment
{
	u32 offset = 0;
	u32 count = 0;
	u32 size = 0;
};

struct StateSaveHeader
{
	char magic[4] = { 'S', 'A', 'V', 'E' };
	char baseFile[32];

	FileSegment staticEntities;
	FileSegment dynamicEntities;
	FileSegment frames;
};

struct CmdHeader
{
	u32 type;
	u32 size;
};

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

// void Input_InitAction(InputAction* action, u32 keyCode, char* label)
// {
//     action->keyCode = keyCode;
//     action->value = 0;
//     action->lastFrame = 0;
//     COM_CopyStringLimited(label, action->label, 16);
// }

void Input_InitAction(InputActionSet* actions, u32 keyCode, char* label)
{
    i32 index = actions->count++;
    actions->actions[index].keyCode = keyCode;
    actions->actions[index].value = 0;
    actions->actions[index].lastFrame = 0;
    COM_CopyStringLimited(label, actions->actions[index].label, 16);
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

inline i32 Input_GetActionValue(InputAction* actions, i32 numActions, char* actionName)
{
    InputAction* action = Input_FindAction(actions, numActions, actionName);
    Assert(action != NULL);
    return action->value;
}

inline i32 Input_GetActionValue(InputActionSet* actions, char* actionName)
{
    return Input_GetActionValue(actions->actions, actions->count, actionName);
}

u8 Input_CheckActionToggledOn(InputActionSet* actions, char* actionName, u32 frameNumber)
{
    InputAction* action = Input_FindAction(actions->actions, actions->count, actionName);
    Assert(action != NULL);
    
    return (action->value != 0 && action->lastFrame == frameNumber);
}

u8 Input_CheckActionToggledOff(InputActionSet* actions, char* actionName, u32 frameNumber)
{
    InputAction* action = Input_FindAction(actions->actions, actions->count, actionName);
    Assert(action != NULL);
    
    return (action->value == 0 && action->lastFrame == frameNumber);
}

// Test an input event vs actions array. Return an input if it has changed, NULL if nothing changed
InputAction* Input_TestForAction(InputActionSet* actions, i32 inputValue, u32 inputKeyCode, u32 frameNumber)
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
