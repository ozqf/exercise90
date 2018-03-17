#pragma once

#include "../Shared/shared_types.h"

#include <gl/gl.h>

u32 Win32_R_RegisterTexture(void* rgbaPixels, u32 width, u32 height)
{
	GLuint texID = g_textureHandles[g_nextTexture];
	glBindTexture(GL_TEXTURE_2D, texID);

	// GL_BGRA_EXT or GL_RGBA?
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, rgbaPixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaPixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    char buf[128];
        sprintf_s(buf, 128, "Binding texture index: %d - GLuint: %d. Next: %d\n\tImg size: %d / %d\n",
        g_nextTexture, texID, g_textureHandles[g_nextTexture + 1], width, height);
        OutputDebugString(buf);

	return g_nextTexture++;
}
