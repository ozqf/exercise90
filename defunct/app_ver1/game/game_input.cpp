#pragma once

#include "game.h"

#define GAME_INPUT_FULL_FREEDOM 0
#define GAME_INPUT_ON_FLY 1
#define GAME_INPUT_ON_FOOT 2

#define MOUSE_SENSITIVITY_MULTIPLIER 0.0025f
#define MOUSE_SENSITIVITY_DEFAULT 50

internal u8 GAME_INPUT_MODE = 2;

static i32 Mouse_InvertedMultiplier()
{
	i32 val = App_GetVarAsI32("i.inverted");
	if (val == 0) { return 1; } else { return -1; }
}

static f32 Mouse_MoveMultiplier()
{
	i32 sensitivity = App_GetVarAsI32("i.sensitivity");
	if (sensitivity <= 0) { sensitivity = MOUSE_SENSITIVITY_DEFAULT; }
	return (f32)sensitivity * MOUSE_SENSITIVITY_MULTIPLIER;
}

////////////////////////////////////////////////////////////////////////////
// Full freedom
// - rotates via direct manipulation of the camera matrix
// - gives full freedom
////////////////////////////////////////////////////////////////////////////
void Game_ApplyInputFullFreedom(InputActionSet* actions, Vec3* degrees, Transform* t, f32 dt, u32 frame)
{
	if (Input_GetActionValue(actions, "Reset"))
	{
		Transform_SetToIdentity(t);
		return;
	}
	Vec4 movement = {};
    Vec4 rotation = {}; // only used for constant rate keyboard rotation (roll atm)
	
	rotation.x -= (((f32)Input_GetActionValue(actions, "Mouse Move Y") * Mouse_MoveMultiplier())) * Mouse_InvertedMultiplier();
	rotation.y -= (((f32)Input_GetActionValue(actions, "Mouse Move X") * Mouse_MoveMultiplier()));
	
	if (Input_GetActionValue(actions, "Roll Left")) { rotation.z += 1 * PLAYER_ROTATION_SPEED; }
	if (Input_GetActionValue(actions, "Roll Right")) { rotation.z += -1 * PLAYER_ROTATION_SPEED; }
	rotation.z *= dt;
	
	Transform_RotateZ(t, rotation.z * DEG2RAD);
	Transform_RotateY(t, rotation.y * DEG2RAD);
	Transform_RotateX(t, rotation.x * DEG2RAD);
	
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


////////////////////////////////////////////////////////////////////////////
// Fly mode - 3D movement but locked around static Y axis
// - rotates via incrementing input->degrees
// - DOES NOT WORK WITH ROLL!
////////////////////////////////////////////////////////////////////////////
void Game_ApplyInputFlyMode(InputActionSet* actions, Vec3* degrees, Transform* t, f32 dt, u32 frame)
{
	if (Input_CheckActionToggledOn(actions, "Reset", frame))
	{
		Transform_SetToIdentity(t);
		*degrees = {};
		return;
	}

	////////////////////////////////////////////////////////////////////////
	// Rotation
	////////////////////////////////////////////////////////////////////////

	f32 turnRate = 90;
	f32 turnStep = turnRate * dt;
	
	degrees->y -= (((f32)Input_GetActionValue(actions, "Mouse Move X") * Mouse_MoveMultiplier()));

	degrees->x -= (((f32)Input_GetActionValue(actions, "Mouse Move Y") * Mouse_MoveMultiplier())) * Mouse_InvertedMultiplier();

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

////////////////////////////////////////////////////////////////////////////
// On Foot mode - view locked around static Y, main movement is on X/Z plane
// - rotates via incrementing input->degrees
// - DOES NOT WORK WITH ROLL!
////////////////////////////////////////////////////////////////////////////
void Game_ApplyInputOnFootMode(InputActionSet* actions, Vec3* degrees, Transform* t, f32 dt, u32 frame)
{
	if (Input_CheckActionToggledOn(actions, "Reset", frame))
	{
		Transform_SetToIdentity(t);
		*degrees = {};
		return;
	}

	////////////////////////////////////////////////////////////////////////
	// Rotation
	////////////////////////////////////////////////////////////////////////

	f32 turnRate = 90;
	f32 turnStep = turnRate * dt;
	
	degrees->y -= (((f32)Input_GetActionValue(actions, "Mouse Move X") * Mouse_MoveMultiplier()));

	//rotation.x = COM_CapAngleDegrees(t->rot.x);

	//if (input->pitchUp) { degrees->x += -turnStep; }
	//if (input->pitchDown) { degrees->x += turnStep; }

	degrees->x -= (((f32)Input_GetActionValue(actions, "Mouse Move Y") * Mouse_MoveMultiplier())) * Mouse_InvertedMultiplier();

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

	f32 radiansForward = degrees->y * DEG2RAD;
	f32 radiansLeft = (degrees->y + 90) * DEG2RAD;
	
	Vec4 left = {};
	Vec4 up = {};
	Vec4 forward = {};

	forward.x = (sinf(radiansForward) * PLAYER_MOVE_SPEED) * dt * movement.z;
	forward.y = 0;
	forward.z = (cosf(radiansForward) * PLAYER_MOVE_SPEED) * dt * movement.z;

	left.x = (sinf(radiansLeft) * PLAYER_MOVE_SPEED) * dt * movement.x;
	left.y = 0;
	left.z = (cosf(radiansLeft) * PLAYER_MOVE_SPEED) * dt * movement.x;

	up.y = PLAYER_MOVE_SPEED * dt * movement.y;

	t->pos.x += (forward.x + left.x + up.x);
	t->pos.y += (forward.y + left.y + up.y);
	t->pos.z += (forward.z + left.z + up.z);
}


////////////////////////////////////////////////////////////////////////////
// Local input actions -> Actor Input Struct
////////////////////////////////////////////////////////////////////////////
void Game_CreateClientInput(InputActionSet* actions, ActorInput* input)
{
	// Clear buttons and rebuild
	input->buttons = 0;
	// Movement
	if (Input_GetActionValue(actions, "Move Forward"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_MOVE_FORWARD;
	}
	if (Input_GetActionValue(actions, "Move Backward"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_MOVE_BACKWARD;
	}

	if (Input_GetActionValue(actions, "Move Left"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_MOVE_LEFT;
	}
	if (Input_GetActionValue(actions, "Move Right"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_MOVE_RIGHT;
	}

	if (Input_GetActionValue(actions, "Move Up"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_MOVE_UP;
	}
	if (Input_GetActionValue(actions, "Move Down"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_MOVE_DOWN;
	}
	
	if (Input_GetActionValue(actions, "MoveSpecial1"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_MOVE_SPECIAL1;
	}

	if (Input_GetActionValue(actions, "Attack1"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_ATTACK;
	}
	
	if (Input_GetActionValue(actions, "Attack2"))
	{
		input->buttons = input->buttons | ACTOR_INPUT_ATTACK2;
	}
	
	// Orientation
	
	input->degrees.y -= (((f32)Input_GetActionValue(actions, "Mouse Move X") * Mouse_MoveMultiplier()));

	input->degrees.y = COM_CapAngleDegrees(input->degrees.y);

	input->degrees.x -= (((f32)Input_GetActionValue(actions, "Mouse Move Y") * Mouse_MoveMultiplier())) * Mouse_InvertedMultiplier();

	if (input->degrees.x < -89)
	{
		input->degrees.x = -89;
	}
	if (input->degrees.x > 89)
	{
		input->degrees.x = 89;
	}
}

void Game_ApplyInputToTransform(InputActionSet* actions, Vec3* lookDegrees, Transform* t, f32 deltaTime, u32 frameNumber)
{
	switch (GAME_INPUT_MODE)
	{
		case GAME_INPUT_FULL_FREEDOM:
		{
			Game_ApplyInputFullFreedom(actions, lookDegrees, t, deltaTime, frameNumber);
		} break;

		case GAME_INPUT_ON_FOOT:
		{
			Game_ApplyInputOnFootMode(actions, lookDegrees, t, deltaTime, frameNumber);
		} break;

		default:
		{
			Game_ApplyInputFlyMode(actions, lookDegrees, t, deltaTime, frameNumber);
		} break;
	}
}
