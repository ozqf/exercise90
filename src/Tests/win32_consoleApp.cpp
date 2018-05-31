/*******************************************
 * Windows Console application main for 
 * testing purposes
 ******************************************/

#pragma once

#include <stdio.h>
// #include "../Platform/win32_system_include.h"

// #include "test_common_module.h"

// #include "test_heap.h"
// #include "test_zlib.h"
// #include "test_pak.h"
// #include "test_data.h"
// #include "test_multithreading.h"
// #include "test_read_mesh.h"
#include "test_state_file.h"

// main function for everywhere except windows
int main(i32 argc, char* argv[])
{
    printf("*** Exercise 90 win32 tests ***\n");
    printf("Built on %s at %s, file: %s, line: %d\n", __DATE__, __TIME__, __FILE__, __LINE__);
    
    //HeapTest();
    //Test_Com_Run();

    //Test_pak();
    //Test_Pack();
    //Test_ReadMesh();
    //Test_MultiThreading();
    Test_StateSaving();

    printf("\nEnd of Tests\n");
    return 0;
}

// Windows 'Main'
// int CALLBACK WinMain(
//     HINSTANCE hInstance,
//     HINSTANCE hPrevInstance,
//     LPSTR lpCmdLine,
//     int nCmdShow) { }
