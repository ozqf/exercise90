#pragma once

#include <math.h>
#include <stdint.h>

/*****************************************************
PRIMITIVE TYPES
*****************************************************/
#define f32 float
#define f64 double

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define u8 uint8_t
#define uChar uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define bool32 uint32_t


/*****************************************************
MACROS
*****************************************************/
#define internal static
#define local_persist static 
#define global_variable static

#define pi32 3.14159265359f
#define DEG2RAD 3.141593f / 180.0f
#define RAD2DEG 57.2958f


#define ZABS(value) { if (value < ) return -value; }
#define ZMIN(x, y) ((x) < (y) ? (x) : (y))
#define ZMAX(x, y) ((x) > (y) ? (x) : (y))


#define ArrayCount(array) (sizeof(array) / sizeof(array)[0]))

// Quick cross platform assert. Read from address zero if expression is false
// TODO: Message box assert with a print of __FILE__, __LINE__ and __TIME__ possible?
#if PARANOID
#define Assert(expression) if(!(expression)) {*(int *)0 = 0; }
#else
#define Assert(expression)
#endif
#define AssertAlways(expression) if(!(expression)) { *(int *)0 = 0; }

#define IllegalCodePath if (true) { *(int *)0 = 0; }

#define KiloBytes(bytes) ((bytes) * 1024LL)
#define MegaBytes(bytes) (KiloBytes(bytes) * 1024LL)

#define DLL_EXPORT __declspec(dllexport)

#define VEC_X 0
#define VEC_Y 1
#define VEC_Z 2
#define VEC_W 3

/* Matrix use
OpenGL uses column major, y/x matrices
/   0   4   8   12  \
|   1   5   9   13  |
|   2   6   10  14  |
\   3   7   11  15  /

*/

// Access Matrix Arrays
#define M3x3_X0 0
#define M3x3_X1 1
#define M3x3_X2 2

#define M3x3_Y0 3
#define M3x3_Y1 4
#define M3x3_Y2 5

#define M3x3_Z0 6
#define M3x3_Z1 7
#define M3x3_Z2 8



#define M4x4_X0 0
#define M4x4_X1 1
#define M4x4_X2 2
#define M4x4_X3 3

#define M4x4_Y0 4
#define M4x4_Y1 5
#define M4x4_Y2 6
#define M4x4_Y3 7

#define M4x4_Z0 8
#define M4x4_Z1 9
#define M4x4_Z2 10
#define M4x4_Z3 11

#define M4x4_W0 12
#define M4x4_W1 13
#define M4x4_W2 14
#define M4x4_W3 15


#include "com_types.h"
#include "com_maths.h"
#include "com_transform.h"
#include "com_memory_utils.h"
#include "com_render_types.h"
#include "com_collision.h"
#include "com_heap/com_heap.h"
#include "com_assets.h"

///////////////////////////////////////////////////////////////////////
// Buffer macros
///////////////////////////////////////////////////////////////////////
#ifndef COM_COPY
#define COM_COPY(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##))
#endif

#ifndef COM_COPY_STEP
#define COM_COPY_STEP(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##)); \
ptrDestination = (u8*)((u32)ptrDestination + (u32)sizeof(##structType##))
#endif

/*
Single line for if case matches n, read a struct from a buffer and call a function with it
*/
#ifndef COM_HANDLE_EVENT_CASE
#define COM_HANDLE_EVENT_CASE(eventTypeInteger, structType, ptrBytes, ptrBufferStart, bufferSize, functionName) \
case eventTypeInteger##: \
{ \
	ASSERT(COM_CheckBufferHasSpace((u8*)##ptrBytes##, (u8*)ptrBufferStart##, bufferSize##, sizeof(##structType##))); \
    structType ev = {}; \
    COM_COPY(##ptrBytes##, &##ev##, structType##); \
    ptrBytes = ((u8*)##ptrBytes + sizeof(##structType##)); \
} break;
#endif
