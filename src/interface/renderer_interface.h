#pragma once

#include "../../common/com_module.h"

// TODO: Re-implement renderer using command buffer instead of scene object
// TODO: Call render from application...? (might requiring moving console into app layer...?)

#define REND_CMD_TYPE_NULL 0
#define REND_CMD_TYPE_SETTINGS 1
#define REND_CMD_TYPE_BEGIN 2
#define REND_CMD_TYPE_END 3
#define REND_CMD_TYPE_DRAW 4

struct RCmdSceneSettings
{
    Transform cameraTransform;
    RenderSceneSettings settings;
};

struct RCmdBeginFrame
{
    f32 vewPortX;
	f32 vewPortY;
	f32 vewPortWidth;
	f32 vewPortHeight;
};

struct RCmdEndFrame
{
    i32 foo;
};

struct RCmdDraw
{
    Transform transform;
    RendObj obj;
};

struct RenderCommand
{
    i32 sentinel;
    i32 type;
	
	union
	{
		RCmdSceneSettings settings;
		RCmdBeginFrame begin;
		RCmdEndFrame end;
		RCmdDraw draw;
	};
};
