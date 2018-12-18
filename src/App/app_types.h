#pragma once

#include "../common/com_module.h"

////////////////////////////////////////////////////////////////////
// Network
////////////////////////////////////////////////////////////////////

#define STREAM_MSG_HEADER_SENTINEL_SIZE 4
struct StreamMsgHeader
{
    u32 id;
    i32 size;
    f32 resendTime;
    f32 resendMax;
	char sentinel[STREAM_MSG_HEADER_SENTINEL_SIZE];
};

// Big-endian
//#define NET_DESERIALISE_CHECK 0xDEADBEEF
// Little-endian
#define NET_DESERIALISE_CHECK 0xEFBEADDE
#define NET_DESERIALISE_CHECK_LABEL "0xDEADBEEF"

// (u16 - num reliable bytes) (u32 - first message Id)
#define NET_SIZE_OF_RELIABLE_HEADER (sizeof(u16) + sizeof(u32))

// (u16 -num reliable bytes)
#define NET_SIZE_OF_UNRELIABLE_HEADER (sizeof(u16))

#define MAX_PACKET_TRANSMISSION_MESSAGES 64
struct TransmissionRecord
{
	u32 sequence;
	u32 numReliableMessages;
	u32 reliableMessageIds[MAX_PACKET_TRANSMISSION_MESSAGES];
};

#define MAX_TRANSMISSION_RECORDS 33
struct NetStream
{
    // latest reliable command from remote executed here
    u32 inputSequence;
    // id of next reliable message sent to remote
    u32 outputSequence;
    // the most recented acknowledged message Id
    u32 ackSequence;

    ByteBuffer inputBuffer;
    ByteBuffer outputBuffer;
    
    TransmissionRecord transmissions[MAX_TRANSMISSION_RECORDS];
};

struct PacketData
{
    u8* ptr;
    u16 size;

    struct Timing
    {
        u32 frame;
        f32 totalTime;
    };
    
    struct ReliableSection
    {
        u8* start;
        u16 numBytes;
        u32 firstSequence;
    };

    u32 syncCheck;

    struct UnreliableSection
    {
        u8* start;
        u16 numBytes;
    };
};


////////////////////////////////////////////////////////////////////
// UI
////////////////////////////////////////////////////////////////////
struct UIEntity
{
    i32 id;
    u8 inUse;
    u8 isInteractive;
    i32 state;
    Transform transform;
    f32 halfWidth;
    f32 halfHeight;
    RendObj rendObj;
	RendObj debugRend;		// for drawing collision boxes
    char name[16];
};

struct HudRingItem
{
    i32 state;
    f32 radius;
    Vec3 screenPos;
    f32 scale;

    f32 startScale;
    f32 endScale;
    f32 tick;
    f32 tickMax;
    
    f32 degrees;
    Vec3 worldPos;

    RendObj rendObj;
};

////////////////////////////////////////////////////////////////////
// ASSETS
////////////////////////////////////////////////////////////////////
#define MAX_TEXTURES 128
#define MAX_MESHES 128

struct TextureHandles
{
    Texture2DHeader textureHeaders[MAX_TEXTURES];
    BlockRef blockRefs[MAX_TEXTURES]; // IF allocated on the heap, find the reference here
    u32 numTextures = 0;
    u32 maxTextures = MAX_TEXTURES;
};

struct MeshHandle
{
    i32 id;
    char name[50];
    i32 loaded;
    MeshData data;
};

struct MeshHandles
{
    MeshHandle meshes[MAX_MESHES];
    u32 nextMesh = 0;
    u32 maxMeshes = MAX_MESHES;
};

enum ReplayMode
{
	NoReplayMode,
	RecordingReplay,
	PlayingReplay
};

////////////////////////////////////////////////////////////////////
// INPUT
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
