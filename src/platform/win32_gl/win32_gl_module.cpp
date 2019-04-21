#pragma once
#ifndef WIN32_GL_MODULE_CPP
#define WIN32_GL_MODULE_CPP

/*
Reading material

// Tutorials
http://www.songho.ca/opengl/
http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
https://www.wikihow.com/Make-a-Cube-in-OpenGL

// API Reference
https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml
http://docs.gl/
https://msdn.microsoft.com/en-us/library/windows/desktop/dd318361(v=vs.85).aspx

*/

#include "../win32_system_include.h"
#include <stdio.h>
#include <gl/gl.h>

#include "../../common/common.h"
#include "win32_gl_interface.h"

//////////////////////////////////////////////////////////////////////////
// Types
//////////////////////////////////////////////////////////////////////////
struct CharArrayGeometryData
{
	char* charArray;
	i32 sheetWidthPixels;
	i32 alignmentMode;
	f32 screenOriginX;
	f32 screenOriginY;
	f32 charSize;
	f32 aspectRatio;
    f32 red;
    f32 green;
    f32 blue;
};

#include "win32_gl_extension_defs.h"

//////////////////////////////////////////////////////////////////////////
// Data
//////////////////////////////////////////////////////////////////////////

// HGLRC "HandleOpenGLRenderingContext"
internal HGLRC g_openglRC = NULL;
internal HWND g_window = NULL;
internal ScreenInfo g_screenInfo;
internal RendererPlatform g_platform;

// Settings
internal f32 win32_aspectRatio = 0;
internal Extensions g_extensions = {};

// assets
#define NUM_TEST_VERTEX_BUFFER_OBJECTS 128
GLuint g_vboHandles[NUM_TEST_VERTEX_BUFFER_OBJECTS];

//#define TEX_LAST_EMBEDDED_TEXTURE_INDEX 0

GLuint g_textureHandles[REND_NUM_TEXTURE_HANDLES];

#define TEX_CHARSET_NAME "charset_128x128_bw"
internal Texture2DHeader* tex_charset;

//////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////
void R_SetupLights(RenderSceneSettings* settings, Transform* model);

#include "win32_gl_loading.h"
#include "win32_gl_primitives.h"
#include "win32_gl_embed.h"
#include "win32_gl_text.h"
#include "win32_gl_matrix.h"
#include "win32_gl_draw.h"
#include "win32_gl_execute.h"
#include "win32_gl_main.cpp"
#include "win32_gl_dll_export.h"

#endif
