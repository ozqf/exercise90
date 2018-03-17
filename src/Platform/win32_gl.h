#pragma once

#include <gl/gl.h>

#include "../Shared/shared_types.h"

//////////////////////////////////////////////////////////////////////////
// Data
//////////////////////////////////////////////////////////////////////////

// Settings
global_variable f32 win32_aspectRatio = 0;

// assets
#define NUM_TEST_TEXTURES 128
GLuint g_textureHandles[NUM_TEST_TEXTURES];
global_variable u32 g_nextTexture = 0;

// Working matrix
f32 mat[TRANSFORM_MATRIX_SIZE];

//////////////////////////////////////////////////////////////////////////
// Test State vars
//////////////////////////////////////////////////////////////////////////
#define TEST_BUFFER_WIDTH 32
#define TEST_BUFFER_HEIGHT 32
#define TEST_BUFFER_PIXEL_COUNT (32 * 32)
#define TEST_BUFFER_MEM_SIZE (TEST_BUFFER_PIXEL_COUNT * 4)

#define NUM_GL_PRIMITIVE_MODES 3
global_variable i32 g_gl_primitive_mode = 0;
global_variable f32 g_gl_primitive_degrees_X = 0;
global_variable f32 g_gl_primitive_degrees_Y = 0;

//global_variable Transform cameraTransform;

global_variable Transform entityTransform;
global_variable Transform entityTransform2;
global_variable Transform entityTransform3;

global_variable int xOffset = 0;
global_variable int yOffset = 0;

win32_offscreen_buffer testBuffer;
i32 testBufferPixels[TEST_BUFFER_PIXEL_COUNT];

win32_offscreen_buffer testBuffer2;
i32 testBufferPixels2[TEST_BUFFER_PIXEL_COUNT];

win32_offscreen_buffer testBuffer3;
i32 testBufferPixels3[TEST_BUFFER_PIXEL_COUNT];

//////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////
#include "win32_gl_loading.h"
#include "win32_gl_primitives.h"
#include "win32_gl.cpp"
