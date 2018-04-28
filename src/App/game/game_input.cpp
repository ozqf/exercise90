#pragma once

#include "game.h"

////////////////////////////////////////////////////////////////////////////
// Process Input
////////////////////////////////////////////////////////////////////////////
void Game_ApplyInputToTransform(InputTick* input, Transform* t, GameTime* time)
{
    if (input->reset)
    {
        t->pos = { 0, 0, 2 };
        t->rot = { 0, 0, 0 };
        t->scale = { 0, 0, 0};
        return;
    }

    Vec3 movement = {};
    Vec3 rotation = {}; // only used for constant rate keyboard rotation (roll atm)

	/////////////////////////////////////////////////
	// READ ROTATION
	/////////////////////////////////////////////////
	if (input->yawLeft) { rotation.y += 1; }
	if (input->yawRight) { rotation.y += -1; }

	if (input->pitchUp) { rotation.x += -1; }
	if (input->pitchDown) { rotation.x += 1; }

	if (input->rollLeft) { rotation.z += 1; }
	if (input->rollRight) { rotation.z += -1; }

	// x = pitch, y = yaw, z = roll
	f32 sensitivity = 0.1f;
	i8 inverted = -1;

#if 1
    // Disabled mouse input for debugging
	//t->rot.x += rotation.x * (PLAYER_ROTATION_SPEED * time->deltaTime);
	//t->rot.x -= (((f32)input->mouseMovement[1] * sensitivity) * time->deltaTime) * inverted;
    t->rot.x -= (((f32)input->mouseMovement[1] * sensitivity)) * inverted;
	t->rot.x = COM_CapAngleDegrees(t->rot.x);

	//t->rot.y += rotation.y * (PLAYER_ROTATION_SPEED * time->deltaTime);
	//t->rot.y -= ((f32)input->mouseMovement[0] * sensitivity) * time->deltaTime;
    t->rot.y -= ((f32)input->mouseMovement[0] * sensitivity);
	t->rot.y = COM_CapAngleDegrees(t->rot.y);

	t->rot.z += rotation.z * (PLAYER_ROTATION_SPEED * time->deltaTime);
	t->rot.z = COM_CapAngleDegrees(t->rot.z);
#endif

	if (input->moveLeft)
	{
		movement.x += -1;
	}
	if (input->moveRight)
	{
		movement.x += 1;
	}
	if (input->moveForward)
	{
		movement.z += -1;
	}
	if (input->moveBackward)
	{
		movement.z += 1;
	}

	if (input->moveDown)
	{
		movement.y += -1;
	}
	if (input->moveUp)
	{
		movement.y += 1;
	}

	/*
	Movement forward requires creating a vector in the direction of
	the object's "forward". Object in this case has rotation, so must convert
	rotation to a forward vector, and then scale this vector by desired speed
	to create a velocity change in the desired direction

	> Read rotation input and rotate angles
	> Calculate forward vector
	> Scale forward to desired speed and add to position
    */
	Vec3 frameMove = {};

	// Sideways: X Input
	// Vertical: Y Input
	// Forward: Z input

	// Quick hack to force movement to occur on a flat x/z plane only
	Vec3 groundRot = t->rot;
	groundRot.x = 0;
	groundRot.z = 0;

    AngleVectors angleVectors = {};

	Calc_AnglesToAxesZYX(&groundRot, &angleVectors.left, &angleVectors.up, &angleVectors.forward);

    // Set the cameras own
    // TODO: Clean this whole messy function up!
    AngleVectors* camVecs = &g_worldScene.cameraAngleVectors;
    Calc_AnglesToAxesZYX(&t->rot, &camVecs->left, &camVecs->up, &camVecs->forward);

	// If input is blank mag will be speed * 0?
	Vec3_SetMagnitude(&angleVectors.left, ((PLAYER_MOVE_SPEED * time->deltaTime) * movement.x));
	Vec3_SetMagnitude(&angleVectors.up, ((PLAYER_MOVE_SPEED * time->deltaTime) * movement.y));
	Vec3_SetMagnitude(&angleVectors.forward, ((PLAYER_MOVE_SPEED * time->deltaTime) * movement.z));

	frameMove.x = angleVectors.forward.x + angleVectors.up.x + angleVectors.left.x;
	frameMove.y = angleVectors.forward.y + angleVectors.up.y + angleVectors.left.y;
	frameMove.z = angleVectors.forward.z + angleVectors.up.z + angleVectors.left.z;

	t->pos.x += frameMove.x;
	t->pos.y += frameMove.y;
	t->pos.z += frameMove.z;
}
