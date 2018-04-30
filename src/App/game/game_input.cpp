#pragma once

#include "game.h"

#define GAME_INPUT_FULL_FREEDOM 0
#define GAME_INPUT_ON_FOOT 1

internal u8 GAME_INPUT_MODE = 0;

////////////////////////////////////////////////////////////////////////////
// Full freedom
// - rotates via direct manipulation of the camera matrix
// - gives full freedom
////////////////////////////////////////////////////////////////////////////
void Game_ApplyInputFullFreedom(InputTick* input, Transform* t, GameTime* time)
{
	if (input->reset)
	{
		M4x4_SetToIdentity(t->matrix.cells);
		return;
	}
	f32 sensitivity = 0.1f;
	i8 inverted = -1;

    Vec4 movement = {};
    Vec4 rotation = {}; // only used for constant rate keyboard rotation (roll atm)
	
	// if (input->yawLeft) { rotation.y += 1 * 90; }
	// if (input->yawRight) { rotation.y += -1 * 90; }
	// rotation.y *= time->deltaTime;

	// if (input->pitchUp) { rotation.x += -1 * 90; }
	// if (input->pitchDown) { rotation.x += 1 * 90; }
	// rotation.x *= time->deltaTime;
	
	// TODO: Reimplement mouse rotation!
	rotation.x -= (((f32)input->mouseMovement[1] * sensitivity)) * inverted;
	rotation.y -= (((f32)input->mouseMovement[0] * sensitivity));
	
	if (input->rollLeft) { rotation.z += 1 * 90; }
	if (input->rollRight) { rotation.z += -1 * 90; }
	rotation.z *= time->deltaTime;

	M4x4_RotateZ(t->matrix.cells, rotation.z);
	M4x4_RotateY(t->matrix.cells, rotation.y);
	M4x4_RotateX(t->matrix.cells, rotation.x);
	
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

	// Apply movement
	Vec4 left;
	Vec4 up;
	Vec4 forward;
	left.x = (t->matrix.xAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;
	left.y = (t->matrix.xAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;
	left.z = (t->matrix.xAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;

	up.x = (t->matrix.yAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;
	up.y = (t->matrix.yAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;
	up.z = (t->matrix.yAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;

	forward.x = (t->matrix.zAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;
	forward.y = (t->matrix.zAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;
	forward.z = (t->matrix.zAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;

	t->matrix.posX += (forward.x + left.x + up.x);
	t->matrix.posY += (forward.y + left.y + up.y);
	t->matrix.posZ += (forward.z + left.z + up.z);
}


////////////////////////////////////////////////////////////////////////////
// On Foot mode
// - rotates via incrementing input->degrees
// - DOES NOT WORK WITH ROLL!
////////////////////////////////////////////////////////////////////////////
void Game_ApplyInputOnFootMode(InputTick* input, Transform* t, GameTime* time)
{
	g_debugInput = *input;
	if (input->reset)
	{
		M4x4_SetToIdentity(t->matrix.cells);
		input->degrees = {};
		return;
	}

	////////////////////////////////////////////////////////////////////////
	// Rotation
	////////////////////////////////////////////////////////////////////////
	f32 sensitivity = 0.1f;
	i8 inverted = -1;

	f32 turnRate = 90;
	f32 turnStep = turnRate * time->deltaTime;

	// if (input->yawLeft) { input->degrees.y += turnStep; }
	// if (input->yawRight) { input->degrees.y += -turnStep; }
	// input->degrees.y = COM_CapAngleDegrees(input->degrees.y);

	input->degrees.y -= (((f32)input->mouseMovement[0] * sensitivity));

	//rotation.x = COM_CapAngleDegrees(t->rot.x);
	
	//if (input->pitchUp) { input->degrees.x += -turnStep; }
	//if (input->pitchDown) { input->degrees.x += turnStep; }

	input->degrees.x -= (((f32)input->mouseMovement[1] * sensitivity)) * inverted;

	if (input->degrees.x < -89)
	{
		input->degrees.x = -89;
	}
	if (input->degrees.x > 89)
	{
		input->degrees.x = 89;
	}
	
	// Roll doesn't work and shouldn't be used with this input setting
	if (input->rollLeft) { input->degrees.z += turnStep; }
	if (input->rollRight) { input->degrees.z += -turnStep; }
	input->degrees.z = COM_CapAngleDegrees(input->degrees.z);

	Vec4 pos = t->matrix.wAxis;
	// Construct camera matrix
	Transform_SetToIdentity(t);
	t->matrix.wAxis = pos;
	// This is order sensitivity! roll -> yaw -> pitch
	M4x4_RotateZ(t->matrix.cells, input->degrees.z);
	M4x4_RotateY(t->matrix.cells, input->degrees.y);
	M4x4_RotateX(t->matrix.cells, input->degrees.x);
	


	////////////////////////////////////////////////////////////////////////
	// Movement
	////////////////////////////////////////////////////////////////////////
	Vec4 movement = {};
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

	// Apply movement
	// TODO: Currently identical to Full freedom movement
	// Should be limited to moving based on yaw only!
	Vec4 left;
	Vec4 up;
	Vec4 forward;
	left.x = (t->matrix.xAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;
	left.y = (t->matrix.xAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;
	left.z = (t->matrix.xAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;

	up.x = (t->matrix.yAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;
	up.y = (t->matrix.yAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;
	up.z = (t->matrix.yAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;

	forward.x = (t->matrix.zAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;
	forward.y = (t->matrix.zAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;
	forward.z = (t->matrix.zAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;

	t->matrix.posX += (forward.x + left.x + up.x);
	t->matrix.posY += (forward.y + left.y + up.y);
	t->matrix.posZ += (forward.z + left.z + up.z);


}
void Game_ApplyInputToTransform(InputTick* input, Transform* t, GameTime* time)
{
	if (GAME_INPUT_MODE == GAME_INPUT_ON_FOOT)
	{
		Game_ApplyInputOnFootMode(input, t, time);
	}
	else
	{
		Game_ApplyInputFullFreedom(input, t, time);
	}
}
