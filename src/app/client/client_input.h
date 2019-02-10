#pragma once

#include "client.h"

internal void CL_InitInputs(InputActionSet* actions)
{
    Input_InitAction(actions, Z_INPUT_CODE_V, "Cycle Debug");
    Input_InitAction(actions, Z_INPUT_CODE_R, "Reset");
    Input_InitAction(actions, Z_INPUT_CODE_ESCAPE, "Menu");

    Input_InitAction(actions, Z_INPUT_CODE_A, "Move Left");
    Input_InitAction(actions, Z_INPUT_CODE_D, "Move Right");
    Input_InitAction(actions, Z_INPUT_CODE_W, "Move Forward");
    Input_InitAction(actions, Z_INPUT_CODE_S, "Move Backward");
    Input_InitAction(actions, Z_INPUT_CODE_SPACE, "Move Up");
    Input_InitAction(actions, Z_INPUT_CODE_CONTROL, "Move Down");
    Input_InitAction(actions, Z_INPUT_CODE_SHIFT, "MoveSpecial1");
    Input_InitAction(actions, Z_INPUT_CODE_Q, "Roll Left");
    Input_InitAction(actions, Z_INPUT_CODE_E, "Roll Right");

    Input_InitAction(actions, Z_INPUT_CODE_G, "Spawn Test");
    Input_InitAction(actions, Z_INPUT_CODE_H, "Spawn Test 2");
    Input_InitAction(actions, Z_INPUT_CODE_J, "Spawn Test 3");
    Input_InitAction(actions, Z_INPUT_CODE_P, "Pause");

    Input_InitAction(actions, Z_INPUT_CODE_MOUSE_POS_X, "Mouse Pos X");
    Input_InitAction(actions, Z_INPUT_CODE_MOUSE_POS_Y, "Mouse Pos Y");
    Input_InitAction(actions, Z_INPUT_CODE_MOUSE_MOVE_X, "Mouse Move X");
    Input_InitAction(actions, Z_INPUT_CODE_MOUSE_MOVE_Y, "Mouse Move Y");
    Input_InitAction(actions, Z_INPUT_CODE_MOUSE_1, "Attack1");
    Input_InitAction(actions, Z_INPUT_CODE_MOUSE_2, "Attack2");
}

internal void CL_InputCheckButton(
    InputActionSet* actions,
    char* inputName,
    u32* flags,
    u32 buttonFlag)
{
    if (Input_GetActionValue(actions, inputName))
    {
        *flags |= buttonFlag;
    }
}

internal void CL_UpdateActorInput(InputActionSet* actions, SimActorInput* input)
{
    // Clear buttons and rebuild. Keep mouse position values
    u32 flags = 0;

    f32 mouseMoveMultiplier = 1;
    f32 mouseInvertedMultiplier = -1;

    CL_InputCheckButton(actions, "Move Forward", &flags, ACTOR_INPUT_MOVE_FORWARD);
    CL_InputCheckButton(actions, "Move Backward", &flags, ACTOR_INPUT_MOVE_BACKWARD);
    CL_InputCheckButton(actions, "Move Left", &flags, ACTOR_INPUT_MOVE_LEFT);
    CL_InputCheckButton(actions, "Move Right", &flags, ACTOR_INPUT_MOVE_RIGHT);

    f32 val;

    val = (f32)Input_GetActionValue(actions, "Mouse Move X") * mouseMoveMultiplier;
    input->degrees.y -= val;
    input->degrees.y = COM_CapAngleDegrees(input->degrees.y);

    // original:
    //input->degrees.y -= (((f32)Input_GetActionValue(actions, "Mouse Move X") * mouseMoveMultiplier));
    //input->degrees.y = COM_CapAngleDegrees(input->degrees.y);

    input->degrees.x -= (((f32)Input_GetActionValue(actions, "Mouse Move Y")
		* mouseMoveMultiplier))
        * mouseInvertedMultiplier;
    
	if (input->degrees.x < -89)
	{
		input->degrees.x = -89;
	}
	if (input->degrees.x > 89)
	{
		input->degrees.x = 89;
	}

    input->buttons = flags;

    //printf("Mouse pos %d, %d\n",
    //    Input_GetActionValue(actions, "Mouse Pos X"),
    //    Input_GetActionValue(actions, "Mouse Pos Y")
    //);
}
