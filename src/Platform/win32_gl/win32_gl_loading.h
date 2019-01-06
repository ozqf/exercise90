#pragma once

#include "win32_gl_module.cpp"

void Win32_GetExtensions(Extensions* ex)
{
    *ex = {};

	// Name of opengl extension when found in gl strings:
	//char* extensionName = "WGL_EXT_swap_control";
	#if 0
	ex->SwapInterval = (wgl_swap_interval_ext*)wglGetProcAddress(PROCNAME_SWAP_INTERVAL);
	if (ex->SwapInterval)
	{
		printf("REND Got proc Swap Interval\n");
		ex->SwapInterval(1);
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
void Win32_Platform_R_BindTexture(void* rgbaPixels, u32 width, u32 height, u32 textureIndex)
{
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
	printf("REND Binding texture index: %d - GLuint: %d.  Img size: %d / %d\n",
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
