#pragma once

#include "win32_gl_module.cpp"

void R_ApplySceneSettings(RenderSceneSettings* settings)
{
    R_SetupProjection(settings);
	if (settings->lightBits == 0)
	{
		glDisable(GL_LIGHTING);
	}
	else
	{
		glEnable(GL_LIGHTING);
	}
}

void R_SetViewport(RViewPort* v)
{
	//printf("R Set viewport: %d, %d - %d, %d\n",
	//	v->viewPortX, v->viewPortY, v->viewPortWidth, v->viewPortHeight);
    glViewport(v->viewPortX, v->viewPortY, v->viewPortWidth, v->viewPortHeight);
}

void R_ExecSetProjection(f32* m4x4)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m4x4);
}

void R_Execute(RenderCommand* commands, i32 numCommands, PlatformTime* time)
{
    #if 1
    RenderSceneSettings* settings = NULL;
	for (i32 i = 0; i < numCommands; ++i)
	{
		RenderCommand* cmd = &commands[i];
		switch (cmd->type)
		{
            case REND_CMD_TYPE_DRAW:
			{
				// Settings must have been read before draw calls can be run
				Assert(settings)
				
				RenderListItem* item = &cmd->drawItem;
                R_RenderEntity(settings, &settings->cameraTransform, item);
			} break;
            
            case REND_CMD_TYPE_SETTINGS:
            {
                settings = &cmd->settings;
                R_ApplySceneSettings(settings);
            } break;

			case REND_CMD_TYPE_SET_VIEWPORT:
            {
                R_SetViewport(&cmd->viewPort);
            } break;

            case REND_CMD_TYPE_END:
            {

            } break;
			
			case REND_CMD_TYPE_PROJECTION:
			{
				R_ExecSetProjection(cmd->projection.cells);
			} break;
		}
	}
    #endif
}

