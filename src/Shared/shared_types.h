#pragma once

#include "com_primitive_types.h"
#include "com_vector.h"
#include "com_matrix.h"
#include "com_transform.h"

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

struct AngleVectors
{
    Vec3 forward;
    Vec3 up;
    Vec3 left;
};

struct Transform2
{
	Vec3 pos;
	Vec3 forward;
    Vec3 up;
    Vec3 left;
	Vec3 scale;
};
