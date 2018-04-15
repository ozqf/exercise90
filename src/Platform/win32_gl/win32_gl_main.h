#pragma once

#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>

#include "../../Shared/shared_types.h"

//////////////////////////////////////////////////////////////////////////
// Data
//////////////////////////////////////////////////////////////////////////

// Settings
global_variable f32 win32_aspectRatio = 0;

// assets
#define NUM_TEST_TEXTURES 128
GLuint g_textureHandles[NUM_TEST_TEXTURES];

//////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////
#include "win32_gl_loading.h"
#include "win32_gl_primitives.h"
#include "win32_gl_text.h"
#include "win32_gl_main.cpp"