#pragma once

#include "../common/com_module.h"

// TODO: Re-implement renderer using command buffer instead of scene object
// TODO: Call render from application...? (might requiring moving console into app layer...?)

#define REND_CMD_TYPE_NULL 0
#define REND_CMD_TYPE_PROJECTION 1
#define REND_CMD_TYPE_MODELVIEW 2
#define REND_CMD_TYPE_DRAW 3
//#define REND_CMD_TYPE_SETTINGS 1
//#define REND_CMD_TYPE_SET_VIEWPORT 2
//#define REND_CMD_TYPE_END 3

struct RViewPort
{
    i32 viewPortX;
	i32 viewPortY;
	i32 viewPortWidth;
	i32 viewPortHeight;
};

struct RenderCommand
{
    i32 type;
	union
	{
		//RenderSceneSettings settings;
		//RViewPort viewPort;
		RenderListItem drawItem;
		M4x4 matrix;
	};
};
