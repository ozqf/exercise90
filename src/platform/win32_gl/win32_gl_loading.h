#pragma once

#include "win32_gl_module.cpp"

internal void Win32_GetExtensions(HDC windowContext, Extensions* ex)
{
    *ex = {};

	// Name of opengl extension when found in gl strings:
	//char* extensionName = "WGL_EXT_swap_control";
	#if 1
	ex->SwapInterval =
		(wgl_swap_interval_ext*)wglGetProcAddress(PROCNAME_SWAP_INTERVAL);
	ex->CreateContextAttribs =
		(wglCreateContextAttribsARB*)wglGetProcAddress(PROCNAME_CREATE_CONTEXT_ATTRIBS);
	if (ex->CreateContextAttribs)
	{
		// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
		// Modern Opengl - escalate context
		printf("REND Got create context attribs\n");

		// No shared context (yet...?)
		HGLRC shared = NULL;

		// params
		i32 openglMajorVersion = 3;
		i32 openglMinorVersion = 0;
		// Note: WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB flag causes
		// old fixed function pipeline stuff to be disabled
		i32 contextFlags =
			WGL_CONTEXT_DEBUG_BIT_ARB ;//| WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		i32 profileMask = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, openglMajorVersion,
			WGL_CONTEXT_MINOR_VERSION_ARB, openglMinorVersion,
			//WGL_CONTEXT_LAYER_PLANE_ARB, 0, // For overlays, not needed
			WGL_CONTEXT_FLAGS_ARB, contextFlags,
			WGL_CONTEXT_PROFILE_MASK_ARB, profileMask,
			0
		};
		HGLRC modernGLRC = ex->CreateContextAttribs(windowContext, shared, attribs);
		if (modernGLRC)
		{
			printf("\tOpengl Escalated\n");
			wglMakeCurrent(windowContext, modernGLRC);
			wglDeleteContext(g_openglRC);
			g_openglRC = modernGLRC;
		}
		else
		{
			// Boom
			g_platform.Error(
				"Failed to acquire opengl 3 context",
				"Opengl escalation failed");
		}
		
	}
	else
	{
		// antiquated opengl
	}

	if (ex->SwapInterval)
	{
		printf("REND Got proc Swap Interval\n");
		// 1 == vsync on, 0 off
		ex->SwapInterval(0);
	}
	else
	{
		printf("REND Failed to get proc Swap Interval\n");
	}
	#endif
}

/**
 * Load a texture into a specific texture index
 */
void R_BindTexture(
	void* rgbaPixels, u32 width, u32 height, u32 textureIndex, i32 isInternal)
{
	/*if (isInternal)
	{
		Assert(textureIndex <= TEX_LAST_EMBEDDED_TEXTURE_INDEX)
	}
	else
	{
		Assert(textureIndex > TEX_LAST_EMBEDDED_TEXTURE_INDEX)
	}*/
	
	/**
	--- setup textures ---
	> enable textures
	> generate texture ids, as many as required
	> loop through textures
		> load texture data and settings via
		glTexImage2D
		glTexParameteri
		glBindTexture -> set the current texture settings to the specified texture id
	*/

	//GLuint texID = g_textureHandles[g_nextTexture];
	GLuint texID = g_textureHandles[textureIndex];
	glBindTexture(GL_TEXTURE_2D, texID);
	
	// GL_BGRA_EXT or GL_RGBA?
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, rgbaPixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaPixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	#if 1
	COM_CALL_PRINT(g_platform.Log, 512,
		"REND Binding texture index: %d - GLuint: %d.  Img size: %d / %d\n",
		textureIndex, texID, width, height
	);
	#endif
	#if 0
    char buf[128];
        sprintf_s(buf, 128, "Binding texture index: %d - GLuint: %d.\n\tImg size: %d / %d\n",
        textureIndex, texID, width, height);
        OutputDebugString(buf);
	#endif
	//return g_nextTexture++;
}

void Win32_Platform_R_BindTexture(void* rgbaPixels, u32 width, u32 height, u32 textureIndex)
{
	R_BindTexture(rgbaPixels, width, height, textureIndex, 0);
}
