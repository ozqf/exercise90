#pragma once

#include "../../common/com_module.h"

// TODO: Re-implement renderer using command buffer instead of scene object
// TODO: Call render from application...? (might requiring moving console into app layer...?)

#define REND_CMD_SENTINEL 0xDEADBEEF

#define REND_CMD_TYPE_NULL 0
#define REND_CMD_TYPE_SETTINGS 1
#define REND_CMD_TYPE_BEGIN 2
#define REND_CMD_TYPE_END 3
#define REND_CMD_TYPE_ENTITY 4

struct RenderCommand
{
    i32 sentinel;
    i32 type;
    i32 size;
};

struct RCmdSceneSettings
{
    RenderCommand header;
    Transform cameraTransform;
    RenderSceneSettings settings;
};

struct RCmdBeginFrame
{
    RenderCommand header;
};

struct RCmdEndFrame
{
    RenderCommand header;
};

struct RCmdRenderEntity
{
    RenderCommand header;
    Transform transform;
    RendObj obj;
};
