#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

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

/*****************************************************
MATHS TYPES
*****************************************************/

struct v2
{
    union
    {
        struct
        {
            f32 X, Y;
        };
        f32 E[2];
    };
};

struct Vec3
{
    f32 x, y, z, w;
    // overload array operator to return a pointer to x + index
    f32 &operator[](int index) { return ((&x)[index]); }
    // union
    // {
    //     f32 x, y, z, w;
    //     //f32 axes[4];
    // };
};

struct M4x4
{
    union
    {
        f32
        x0, y0, z0, w0,
        x1, y1, z1, w1,
        x2, y2, z2, w2,
        x3, y3, z3, w3;
        f32 cells[16];
    };
};

/*****************************************************
SIMPLE TYPES
*****************************************************/
struct MemoryBlock
{
    void *ptrMemory;
    i32 size;
};

struct GameTime
{
    f32 deltaTime;
    u32 frameNumber;
    u32 fixedFrameNumber;
    f32 fixedDeltaTime;
    u32 ticks;
};

struct InputTick
{
    i32 mouse[2];
    char left;
    char right;
    char up;
    char down;
};

struct GameBlock
{
    i32 ID;
    f32 pos[3];
    f32 vel[3];
    f32 halfWidth;
    f32 halfHeight;
    f32 speed;
};

struct Transform
{
	i32 ID;
	f32 pos[3];
	f32 rot[3];
	f32 scale[3];
};


/****************************************************************
Global Functions
****************************************************************/

inline u32 SafeTruncateUInt64(u64 value)
{
	// TODO: Defines for max value
	Assert(value <= 0xFFFFFFFF);
	u32 result = (u32)value;
	return result;
}

#endif