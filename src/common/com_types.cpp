#pragma once

#include "com_module.h"

enum ZMouseMode { Free = 0, Captured = 1 };


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

struct ZLineSegment
{
    Vec3 a;
    Vec3 b;
};

struct ZAABB
{
    Vec3 halfSize;
};
