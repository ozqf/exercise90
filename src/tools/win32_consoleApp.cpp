/*******************************************
 * Windows Console application main for 
 * testing purposes
 ******************************************/

#pragma once

#include <stdio.h>

#include "../common/com_module.h"
#include "tests/tests.h"

// #include "../Platform/win32_system_include.h"

//#include "tests/test_command_stream.h"

internal void Test_FatalError(char* message, char* heading)
{
    printf("!--- %s ---!\n%s", heading, message);
}

// main function for everywhere except windows
int main(i32 argc, char* argv[])
{
    COM_SetFatalError(Test_FatalError);
    Tests_Run(argc, argv);
    //TestCommandStream();
    //TestAcks();
    return 0;
}

// Windows 'Main'
// int CALLBACK WinMain(
//     HINSTANCE hInstance,
//     HINSTANCE hPrevInstance,
//     LPSTR lpCmdLine,
//     int nCmdShow) { }
