#if 1
#pragma once

#include "com_defines.h"

//enum ZMouseMode;

struct InputTick;
struct GL_Test_Input;
/****************************************************************
Graphics
****************************************************************/
struct Mesh;
/*****************************************************
SIMPLE TYPES
*****************************************************/
struct MemoryBlock;
struct ZStringHeader;
struct GameTime;
struct AngleVectors;

struct ZLineSegment;
struct ZAABB;
#endif
#pragma once

#include "com_module.h"

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

enum ZMouseMode { Free = 0, Captured = 1 };

struct Vec3
{
    union 
    {
        struct
        {
            f32 x, y, z;
        };    
        f32 e[3];
    };
    // f32 x, y, z, w;
    // // overload array operator to return a pointer to x + index
    // f32 &operator[](int index) { return ((&x)[index]); }
};

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


struct RendObj_Primitive
{
    i32 primitiveType;
    f32 red;
    f32 green;
    f32 blue;
    f32 alpha;
    f32 sizeX;
    f32 sizeY;
    f32 sizeZ;
};

struct RendObj_Billboard
{
    i32 textureIndex;
    f32 uvLeft;
    f32 uvRight;
    f32 uvBottom;
    f32 uvTop;
    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

struct RendObj_Sprite
{
    i32 mode;
    i32 textureIndex;
    f32 width;
    f32 height;
    f32 uvLeft;
    f32 uvRight;
    f32 uvBottom;
    f32 uvTop;
    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

struct RendObj_Line
{
    Vec3 a;
    Vec3 b;
    Vec3 colourA;
    Vec3 colourB;
};

struct RendObj_AsciChar
{
    u8 asciChar;
};

struct RendObj_AsciCharArray
{
    char* chars;
    i32 numChars;
    f32 size;
    f32 r;
    f32 g;
    f32 b;
};

struct RendObj_ColouredMesh
{
    Mesh* mesh;
    f32 r, g, b;
    i32 textureIndex;
};

union RendObj_Union
{
    RendObj_ColouredMesh mesh;
    RendObj_Sprite sprite;
    RendObj_Primitive primitive;
    RendObj_Line line;

    // TODO: Remove these, billboard replced by sprite, ascichar removed
    RendObj_Billboard billboard;
    RendObj_AsciChar asciChar;
    RendObj_AsciCharArray charArray;
};

// Individual render item
struct RendObj
{
    u16 ownerIndex;
    u16 inUse;
    u16 type;
    u32 flags;
    RendObj_Union data;
};

struct RenderListItem
{
    Transform transform;
    RendObj obj;
};

// Scene container for render call each frame
struct RenderScene
{    
    Transform cameraTransform;
    
    i32 projectionMode;
    f32 orthographicHalfHeight;
    i32 fov;

    RenderListItem* sceneItems;
    u32 numObjects;
    u32 maxObjects;
};

struct InputTick
{
    i32 mouse[2];
    i32 mouseMovement[2];
    char attack1, attack2;
    char moveLeft, moveRight, moveUp, moveDown, moveForward, moveBackward;
    char yawLeft, yawRight, pitchUp, pitchDown;
    char rollLeft, rollRight;
    char escape;
    char reset;
    char debug_cycleTexture;
    char debug_break_game;

    Vec3 degrees = {};
};

struct GL_Test_Input
{
	// +z = forward, +y = up, +x = left
	Vec3 movement;
	Vec3 rotation;
	f32 speed;
    f32 rotSpeed;
};

/****************************************************************
Graphics
****************************************************************/
struct Mesh
{
	u32 id;

	u32 numVerts;

	f32* verts;
	f32* uvs;
    f32* normals;
};

struct Texture2DHeader
{
	i32 index;
	char name[32];
	i32 width;
	i32 height;
	u32 *ptrMemory;
};

/*****************************************************
SIMPLE TYPES
*****************************************************/
struct MemoryBlock
{
    void *ptrMemory;
    i32 size;
};

struct ZStringHeader
{
    char* chars;
    i32 length;
    i32 maxLength;
};

struct GameTime
{
    f32 deltaTime;
    u32 frameNumber;
    u32 fixedFrameNumber;
    f32 fixedDeltaTime;
    u32 ticks;
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
