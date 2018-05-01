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

internal i32 g_z_inf = 0x7F800000;
inline f32 ZINFINITY() { return *(float*)&g_z_inf; }

internal i32 g_z_nan = 0x7F800001;
inline f32 ZNaN() { return *(float*)&g_z_nan; }

#define ZABS(value) { if (value < ) return -value; }
#define ZMIN(x, y) ((x) < (y) ? (x) : (y))
#define ZMAX(x, y) ((x) > (y) ? (x) : (y))

inline f32 ZMAX3(f32 a, f32 b, f32 c)
{
    f32 result = a;
    if (b > result) { result = b; }
    if (c > result) { result = c;}
    return result;
}

inline f32 ZMIN3(f32 a, f32 b, f32 c)
{
    f32 result = a;
    if (b < result) { result = b; }
    if (c < result) { result = c;}
    return result;
}

inline void ZSWAPF(f32* x, f32* y)
{
    f32 temp = *x;
    *x = *y;
    *y = temp;
}

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

#define TRANSFORM_MATRIX_SIZE 16
#define X0 0
#define X1 1
#define X2 2
#define X3 3

#define Y0 4
#define Y1 5
#define Y2 6
#define Y3 7

#define Z0 8
#define Z1 9
#define Z2 10
#define Z3 11

#define W0 12
#define W1 13
#define W2 14
#define W3 15

/****************************************************************
Conversion
****************************************************************/
union f32_union
{
    f32 f;
    u8 bytes[sizeof(f32)];
};

union u32_union
{
    u32 value;
    u8 bytes[sizeof(u32)];
};
