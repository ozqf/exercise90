#pragma once

#include "../../common/com_module.h"

// TODO: Re-implement renderer using command buffer instead of scene object
// TODO: Call render from application...? (might requiring moving console into app layer...?)

#define REND_CMD_TYPE_NULL 0
#define REND_CMD_TYPE_SETTINGS 1
#define REND_CMD_TYPE_BEGIN 2
#define REND_CMD_TYPE_END 3
#define REND_CMD_TYPE_DRAW 4

struct RViewPort
{
    f32 vewPortX;
	f32 vewPortY;
	f32 vewPortWidth;
	f32 vewPortHeight;
};

struct RenderCommand
{
    i32 type;
	
	union
	{
		RenderSceneSettings settings;
		RViewPort begin;
		RenderListItem drawItem;
	};
};
