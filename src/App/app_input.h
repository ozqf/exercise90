#pragma once

#include "app_module.cpp"


void Input_SetMouseMode(ZMouseMode mode)
{
    g_mouseMode = mode;
    platform.Platform_SetMouseMode(mode);
}

void Input_ToggleMouseMode()
{
    if (g_mouseMode == Free)
    {
        Input_SetMouseMode(Captured);
    }
    else
    {
        Input_SetMouseMode(Free);
    }
}

void App_InitInput(InputActionSet* actions)
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
    Input_InitAction(actions, Z_INPUT_CODE_MOUSE_1, "Attack 1");
    Input_InitAction(actions, Z_INPUT_CODE_MOUSE_2, "Attack 2");
}
