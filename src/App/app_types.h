#pragma once

#include "../common/com_module.h"

#define MAX_TEXTURES 128

struct TextureHandles
{
    Texture2DHeader textureHeaders[MAX_TEXTURES];
    BlockRef blockRefs[MAX_TEXTURES]; // IF allocated on the heap, find the reference here
    u32 numTextures = 0;
    u32 maxTextures = MAX_TEXTURES;
};

struct FileSegment
{
	u32 offset = 0;
	u32 count = 0;
    // if size is 0, nothing was written, if size is negative, writing was started
    // but not completed (ie it crashed), so size of data is unknown
    // try reading regardless, for debugging!
	i32 size = 0;
};

struct StateSaveHeader
{
	char magic[4] = { 'S', 'A', 'V', 'E' };
	char baseFile[32];
    
    /* File Sections:
    > static entities are skipped if a baseFile was specified
    > Dynamic entities are skipped if this file is a 'base' file
    > Frames are written into replay demo files.
    */

    
	FileSegment staticCommands;
    
	FileSegment dynamicCommands;
    
	FileSegment frames;
};

enum ReplayMode
{
	NoReplayMode,
	RecordingReplay,
	PlayingReplay
};

/**
 * Game Command Header
 * 
 * !Do not access members directly, use attached functions!
 * 
 * Notes;
 * > Describes the type of command and the number of bytes used to store it
 * > data is accessed via getter/setter to allow struct internals to be changed
 *      more easily
 * > Storing size allows free stepping through command buffers without knowledge of
 *      commands themselves.
 * > In future will store some bit flags for meta data about the command itself
 *      (eg how it is encoded)
 */
struct CmdHeader
{
    u32 data1;
    //u16 flags;
    u32 data2;
    /* 4 bytes?
    u8 type;
    u8 flags;
	u16 size;
    */

    //u32 data1;
	//u32 data2;
    
    inline u8 GetType() { return (u8)data1; }
    inline void SetType(u8 newType) { this->data1 = newType; }
    inline u16 GetSize() { return (u16)data2; }
    inline void SetSize(u16 newSize) { this->data2 = newSize; }

    inline u16 Read(u8* ptr)
    {
        COM_COPY(ptr, this, sizeof(CmdHeader));
        return sizeof(CmdHeader);
    }

    inline u16 Write(u8* ptr)
    {
        COM_COPY(this, ptr, sizeof(CmdHeader));
        return sizeof(CmdHeader);
    }
};

u32 Cmd_WriteHeader(u8 type)
{

    return 0;
}


/**
 * Write cmd boilerplate
 * > ptrWrite will be incremented to the next
 *   write position
 * > Header must be written after the actual command
 *   so that the header can record bytes written
 * > Therefore header must have fixed size
 * > Header and Command structs must have a
 *   u32 Write(u8*) function, returning bytes writtten
 */
#ifndef APP_WRITE_CMD_TO_BUFFER
#define APP_WRITE_CMD_TO_BUFFER(u8ptr2ptr_write, u8_cmdType, u8_cmdFlags, cmdObject) \
{ \
u8* ptrOrigin = *u8ptr2ptr_write##; \
 \
*u8ptr2ptr_write += sizeof(CmdHeader); \
 \
u16 cmdBytesWritten = cmdObject##.WriteRaw(*##u8ptr2ptr_write##); \
 \
*u8ptr2ptr_write += cmdBytesWritten; \
CmdHeader newCmdHeader = {}; \
newCmdHeader.SetType(##u8_cmdType##); \
newCmdHeader.SetSize(cmdBytesWritten); \
newCmdHeader.Write(ptrOrigin); \
}
#endif

#ifndef APP_WRITE_CMD
#define APP_WRITE_CMD(i32_bufferId, u8_cmdType, u8_cmdFlags, cmdObject) \
{ \
	GameTime* ptrGameTime = GetAppTime(); \
	if (ptrGameTime->singleFrame)\
	{\
		char* label = App_GetBufferName(g_appWriteBuffer->ptrStart); \
		printf("Writing type %d to %s\n", u8_cmdType, label);\
	}\
\
    u8** ptrToWritePtr = &g_appWriteBuffer->ptrWrite; \
    APP_WRITE_CMD_TO_BUFFER(ptrToWritePtr, u8_cmdType, u8_cmdFlags, cmdObject) \
}
#endif
/** function requirements:
> local scope command struct var
> macro
> command header info
> target buffer
> 
*/

//////////////////////////////////////////////////////

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

struct ReplayFrameHeader
{
    u32 frameNumber;
    u32 size;
	char label[16];
};

//union EntId; // HACK

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
