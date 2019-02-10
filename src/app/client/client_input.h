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

internal void CL_InputCheckButton(InputActionSet* actions, char* inputName, u32* flags, u32 buttonFlag)
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

    CL_InputCheckButton(actions, "Move Forward", &flags, ACTOR_INPUT_MOVE_FORWARD);
    CL_InputCheckButton(actions, "Move Backward", &flags, ACTOR_INPUT_MOVE_BACKWARD);
    /*if (Input_GetActionValue(actions, "Move Forward"))
    {
        input->buttons |= ACTOR_INPUT_MOVE_FORWARD;
    }
    if (Input_GetActionValue(actions, "Move Backward"))
    {
        input->buttons |= ACTOR_INPUT_MOVE_FORWARD;
    }
    if (Input_GetActionValue(actions, "Move Left"))
    {
        
    }
    if (Input_GetActionValue(actions, "Move Right"))
    {
        
    }*/
    input->buttons = flags;
}
