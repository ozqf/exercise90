#pragma once

#include "game.h"

#define GAME_INPUT_FULL_FREEDOM 0
#define GAME_INPUT_ON_FOOT 1

internal u8 GAME_INPUT_MODE = 1;

////////////////////////////////////////////////////////////////////////////
// Full freedom
// - rotates via direct manipulation of the camera matrix
// - gives full freedom
////////////////////////////////////////////////////////////////////////////
void Game_ApplyInputFullFreedom(ClientTick* input, Transform* t, GameTime* time)
{
	if (input->reset)
	{
		Transform_SetToIdentity(t);
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
	
	rotation.x -= (((f32)input->mouseMovement[1] * sensitivity)) * inverted;
	rotation.y -= (((f32)input->mouseMovement[0] * sensitivity));
	
	if (input->rollLeft) { rotation.z += 1 * PLAYER_ROTATION_SPEED; }
	if (input->rollRight) { rotation.z += -1 * PLAYER_ROTATION_SPEED; }
	//rotation.z *= DEG2RAD;
	rotation.z *= time->deltaTime;
	
	Transform_RotateZ(t, rotation.z * DEG2RAD);
	Transform_RotateY(t, rotation.y * DEG2RAD);
	Transform_RotateX(t, rotation.x * DEG2RAD);
	
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
	left.x = (t->rotation.xAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;
	left.y = (t->rotation.xAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;
	left.z = (t->rotation.xAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.x;

	up.x = (t->rotation.yAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;
	up.y = (t->rotation.yAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;
	up.z = (t->rotation.yAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.y;

	forward.x = (t->rotation.zAxisX * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;
	forward.y = (t->rotation.zAxisY * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;
	forward.z = (t->rotation.zAxisZ * PLAYER_MOVE_SPEED * time->deltaTime) * movement.z;

	t->pos.x += (forward.x + left.x + up.x);
	t->pos.y += (forward.y + left.y + up.y);
	t->pos.z += (forward.z + left.z + up.z);
}


////////////////////////////////////////////////////////////////////////////
// On Foot mode
// - rotates via incrementing input->degrees
// - DOES NOT WORK WITH ROLL!
////////////////////////////////////////////////////////////////////////////
void Game_ApplyInputOnFootMode(InputActionSet* actions, Vec3* degrees, Transform* t, f32 dt, u32 frame)
{
	//g_debugInput = *input;
	//if (input->reset)
	if (Input_CheckActionToggledOn(actions, "Reset", frame))
	{
		Transform_SetToIdentity(t);
		*degrees = {};
		return;
	}

	////////////////////////////////////////////////////////////////////////
	// Rotation
	////////////////////////////////////////////////////////////////////////
	f32 sensitivity = 0.1f;
	i8 inverted = -1;

	f32 turnRate = 90;
	f32 turnStep = turnRate * dt;
	// if (input->yawLeft) { degrees->y += turnStep; }
	// if (input->yawRight) { degrees->y += -turnStep; }
	// degrees->y = COM_CapAngleDegrees(degrees->y);
	
	degrees->y -= (((f32)Input_GetActionValue(actions, "Mouse Move X") * sensitivity));

	//rotation.x = COM_CapAngleDegrees(t->rot.x);
	
	//if (input->pitchUp) { degrees->x += -turnStep; }
	//if (input->pitchDown) { degrees->x += turnStep; }

	degrees->x -= (((f32)Input_GetActionValue(actions, "Mouse Move Y") * sensitivity)) * inverted;

#if 1
	if (degrees->x < -89)
	{
		degrees->x = -89;
	}
	if (degrees->x > 89)
	{
		degrees->x = 89;
	}
#endif
	
	// Roll doesn't work and shouldn't be used with this input setting
	//if (input->rollLeft) { degrees->z += turnStep; }
	//if (input->rollRight) { degrees->z += -turnStep; }
	degrees->z = COM_CapAngleDegrees(degrees->z);


	////////////////////////////////////////////////////////////////////////
	// Reset rotation and apply based on new absolute angles
	// This is order sensitivity! roll -> yaw -> pitch
	// NOTE AGAIN...: roll is unusable in this system
	Transform_ClearRotation(t);
	Transform_RotateZ(t, degrees->z * DEG2RAD);
	Transform_RotateY(t, degrees->y * DEG2RAD);
	Transform_RotateX(t, degrees->x * DEG2RAD);

	////////////////////////////////////////////////////////////////////////
	// Movement
	////////////////////////////////////////////////////////////////////////
	Vec4 movement = {};
	if (Input_GetActionValue(actions, "Move Left"))
	{
		movement.x += -1;
	}
	if (Input_GetActionValue(actions, "Move Right"))
	{
		movement.x += 1;
	}
	if (Input_GetActionValue(actions, "Move Forward"))
	{
		movement.z += -1;
	}
	if (Input_GetActionValue(actions, "Move Backward"))
	{
		movement.z += 1;
	}

	if (Input_GetActionValue(actions, "Move Down"))
	{
		movement.y += -1;
	}
	if (Input_GetActionValue(actions, "Move Up"))
	{
		movement.y += 1;
	}

	// Apply movement
	// TODO: Currently identical to Full freedom movement
	// Should be limited to moving based on yaw only!
	Vec4 left;
	Vec4 up;
	Vec4 forward;
	left.x = (t->rotation.xAxisX * PLAYER_MOVE_SPEED * dt) * movement.x;
	left.y = (t->rotation.xAxisY * PLAYER_MOVE_SPEED * dt) * movement.x;
	left.z = (t->rotation.xAxisZ * PLAYER_MOVE_SPEED * dt) * movement.x;

	up.x = (t->rotation.yAxisX * PLAYER_MOVE_SPEED * dt) * movement.y;
	up.y = (t->rotation.yAxisY * PLAYER_MOVE_SPEED * dt) * movement.y;
	up.z = (t->rotation.yAxisZ * PLAYER_MOVE_SPEED * dt) * movement.y;

	forward.x = (t->rotation.zAxisX * PLAYER_MOVE_SPEED * dt) * movement.z;
	forward.y = (t->rotation.zAxisY * PLAYER_MOVE_SPEED * dt) * movement.z;
	forward.z = (t->rotation.zAxisZ * PLAYER_MOVE_SPEED * dt) * movement.z;

	t->pos.x += (forward.x + left.x + up.x);
	t->pos.y += (forward.y + left.y + up.y);
	t->pos.z += (forward.z + left.z + up.z);


}

void Game_ApplyInputToTransform(InputActionSet* actions, ClientTick* input, Transform* t, GameTime* time)
{
	if (GAME_INPUT_MODE == GAME_INPUT_ON_FOOT)
	{
		Game_ApplyInputOnFootMode(actions, &input->degrees, t, time->deltaTime, time->frameNumber);
	}
	else
	{
		Game_ApplyInputFullFreedom(input, t, time);
	}
}
