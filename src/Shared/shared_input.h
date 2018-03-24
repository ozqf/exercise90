#pragma once

#include "shared_types.h"

enum ZMouseMode { Free = 0, Captured = 1 };


struct InputTick
{
    i32 mouse[2];
    i32 mouseMovement[2];
    char moveLeft, moveRight, moveUp, moveDown, moveForward, moveBackward;
    char yawLeft, yawRight, pitchUp, pitchDown;
    char rollLeft, rollRight;
    char escape;
    char reset;
    char debug_cycleTexture;
};

struct GL_Test_Input
{
	// +z = forward, +y = up, +x = left
	Vec3 movement;
	Vec3 rotation;
	f32 speed;
    f32 rotSpeed;
};
