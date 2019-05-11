
#pragma once

#include "com_defines.h"

#if 0
struct GL_Test_Input;
/****************************************************************
Graphics
****************************************************************/
struct MeshData;
/*****************************************************
SIMPLE TYPES
*****************************************************/
struct MemoryBlock;
struct CharBuffer;
struct GameTime;
struct AngleVectors;

struct ZLineSegment;
struct ZAABB;
#endif

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

union u64_union
{
    u64 value;
    u32 u32Values[2];
    u8 bytes[sizeof(u64)];
};

/////////////////////////////////////////////////////////////////////////////
// Vector
/////////////////////////////////////////////////////////////////////////////
struct Point
{
    union
    {
        struct
        {
            i32 x, y;
        };
        i32 parts[2];
    };
};

struct Vec3
{
    union 
    {
        struct
        {
            f32 x, y, z;
        };    
        f32 parts[3];
    };
    // f32 x, y, z, w;
    // // overload array operator to return a pointer to x + index
    // f32 &operator[](int index) { return ((&x)[index]); }
};

com_internal const Vec3 vec3_zero = { }; 

struct Vec4
{
    union 
    {
        struct
        {
            f32 x, y, z, w;
        };    
        f32 e[4];
    };
    // f32 x, y, z, w;
    // // overload array operator to return a pointer to x + index
    // f32 &operator[](int index) { return ((&x)[index]); }
};


/////////////////////////////////////////////////////////////////////////////
// MATRIX 3x3
/////////////////////////////////////////////////////////////////////////////
struct M3x3
{
    union
    {
        /* Careful! Column order!
            X   Y   Z   W
            0   3   6
            1   4   7
            2   5   8
            */
        struct
        {
            f32
            x0, x1, x2,
            y0, y1, y2,
            z0, z1, z2;
        };
        struct
        {
            f32
            // xAxisX, yAxisX, zAxisX, posX,
            // xAxisY, yAxisY, zAxisY, posY,
            // xAxisZ, yAxisZ, zAxisZ, posZ,
            // xAxisW, yAxisW, zAxisW, posW;

            xAxisX, xAxisY, xAxisZ,
            yAxisX, yAxisY, yAxisZ,
            zAxisX, zAxisY, zAxisZ;
        };
        struct
        {
            Vec3 xAxis;
            Vec3 yAxis;
            Vec3 zAxis;
        };
        f32 cells[9];
    };
};

/////////////////////////////////////////////////////////////////////////////
// MATRIX 4x4
/////////////////////////////////////////////////////////////////////////////
struct M4x4
{
    union
    {
        /* Careful! Column order!
            X   Y   Z   W
            0   4   8   12
            1   5   9   13 
            2   6   10  14
            3   7   11  15
            */
        struct
        {
            f32
            x0, x1, x2, x3,
            y0, y1, y2, y3,
            z0, z1, z2, z3,
            w0, w1, w2, w3;
        };
        struct
        {
            f32
            // xAxisX, yAxisX, zAxisX, posX,
            // xAxisY, yAxisY, zAxisY, posY,
            // xAxisZ, yAxisZ, zAxisZ, posZ,
            // xAxisW, yAxisW, zAxisW, posW;

            xAxisX, xAxisY, xAxisZ, xAxisW,
            yAxisX, yAxisY, yAxisZ, yAxisW,
            zAxisX, zAxisY, zAxisZ, zAxisW,
            posX, posY, posZ, posW;
        };
        struct
        {
            Vec4 xAxis;
            Vec4 yAxis;
            Vec4 zAxis;
            Vec4 wAxis;
        };
        f32 cells[16];
    };
};


/*****************************************************
Maths types
*****************************************************/



struct Transform
{
    Vec3 pos;
    Vec3 scale;
    M3x3 rotation;
};

/*****************************************************
SIMPLE TYPES
*****************************************************/
struct MemoryBlock
{
    void *ptrMemory;
    i32 size;
};

struct ByteBlock
{
    u8 *start;
    u8* end;
};

struct CharBuffer
{
    char* chars;
    char* cursor;
    i32 maxLength;

    void Reset()
    {
        cursor = chars;
    }
    
    void Set(char* charArray, i32 numChars)
    {
        chars = charArray;
        cursor = charArray;
        maxLength = numChars;
    }

    CharBuffer StartSubSection()
    {
        // 1 for data 1 for terminator
        COM_ASSERT(Space() > 2, "No space for substring")
        CharBuffer sub;
        sub.chars = cursor;
        sub.cursor = cursor;
        // -1 so EndSubSection can add null terminator
        sub.maxLength = Space() - 1;
        return sub;
    }

    // Set sub string's length to what has been written
    // so that we don't think we can come back and write into
    // it again
    void EndSubSection(CharBuffer* sub)
    {
        COM_ASSERT(Space() > 0, "No space for substring terminator")
        *sub->cursor = '\0';
        sub->cursor++;
        // move to end of sub-section
        cursor = sub->cursor;
        // cap subsection off
        sub->maxLength = (sub->cursor - sub->chars);
    }

    i32 Written()
    {
        return this->cursor - this->chars;
    }
    i32 Space()
    {
        return maxLength - (cursor - chars);
    }
};

struct PlatformTime
{
    f32 deltaTime;
    f32 sessionEllapsed;
    u32 frameNumber;
};

struct GameTime
{
    f32 deltaTime;
    f32 sessionEllapsed;
    u32 platformFrameNumber;
	u32 gameFrameNumber;
    f32 percentToNextFixedFrame;
    /*u32 fixedFrameNumber;
    f32 fixedDeltaTime;*/
    u32 ticks;
    u32 singleFrame;   // If true then print extra debug info
};

struct DateTime
{
    i32 year;
    i32 month;
    i32 dayOfTheMonth;
    i32 dayOfTheWeek;
    
    i32 hour;
    i32 minute;
    i32 second;
};

struct ZLineSegment
{
    Vec3 a;
    Vec3 b;
};

struct ZAABB
{
    Vec3 halfSize;
};
