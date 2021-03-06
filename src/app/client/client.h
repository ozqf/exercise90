#pragma once

#include "../user.h"
#include "../../common/com_defines.h"
#include "../app.h"
#include "../commands.h"
#include "../stream.h"
#include "../../interface/renderer_interface.h"

#define CL_MAX_SENT_INPUT_COMMANDS 60

void    CL_Init(ZNetAddress serverAddress);
void    CL_Shutdown();
i32     CL_IsRunning();
void	CL_CopyCameraTransform(Transform* target);
void    CL_Tick(
			ByteBuffer* sysEvents,
			f32 deltaTime,
			u32 platformFrame);
void    CL_PopulateRenderScene(
			Transform* cam,
			RenderScene* scene,
			i32 maxObjects,
			i32 texIndex,
			f32 interpolateTime);
void    CL_GetRenderCommands(
			RenderCommand** cmds,
			i32* numCommands,
			i32 texIndex,
			f32 interpolateTime);
void    CL_SetLocalUser(UserIds ids);
void    CL_WriteDebugString(CharBuffer* str);
u8      CL_ParseCommandString(
			char* str,
			char** tokens,
			i32 numTokens);
