#pragma once

#include "win32_gl_module.cpp"

void R_Execute(RenderCommand* commands, i32 numCommands, PlatformTime* time)
{
    RenderSceneSettings* settings = NULL;
	for (i32 i = 0; i < numCommands; ++i)
	{
		RenderCommand* cmd = &commands[i];
		switch (cmd->type)
		{
            case REND_CMD_TYPE_DRAW:
			{
				RenderListItem* item = &cmd->drawItem;
                R_RenderEntity(settings, &settings->cameraTransform, item);
			} break;
            
            case REND_CMD_TYPE_SETTINGS:
            {
                settings = &cmd->settings;
            } break;

			case REND_CMD_TYPE_BEGIN:
            {
                
            } break;

            case REND_CMD_TYPE_END:
            {

            } break;
		}
	}
}

