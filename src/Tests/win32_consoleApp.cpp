/*******************************************
 * Windows Console application main for 
 * testing purposes
 ******************************************/

#pragma once

#include <stdio.h>

#include "../common/com_module.h"

// #include "../Platform/win32_system_include.h"

 #include "test_common_module.h"

// #include "test_heap.h"
// #include "test_zlib.h"
// #include "test_pak.h"
// #include "test_data.h"
// #include "test_multithreading.h"
// #include "test_read_mesh.h"
#include "test_state_file.h"
#include "test_base64.h"

// main function for everywhere except windows
int main(i32 argc, char* argv[])
{
    printf("*** Exercise 90 win32 tests ***\n");
    printf("Built on %s at %s, file: %s, line: %d\n", __DATE__, __TIME__, __FILE__, __LINE__);

    printf("\nChoose Action\n");
    printf("1. Test Common Library\n");
    printf("2. Read/Write state file \"testbox.lvl\"\n");
    printf("3. Test Base 64\n");
    printf("Choose...\n");

    char input = (char)getchar();

    printf("Oh %c... fair enough\n", input);

    switch (input)
    {
        case '1':
        {
            Test_Com_Run();
        } break;
        case '2':
        {
            Test_StateSaving();
        } break;
    
        case '3':
        {
            Test_Base64();
            printf("\nEnd of Tests\n");
        } break;

        default:
        {
            printf("That means jack to me...\n");
        } break;
    }
    
    //HeapTest();
    //Test_pak();
    //Test_Pack();
    //Test_ReadMesh();
    //Test_MultiThreading();
    
    return 0;
}

// Windows 'Main'
// int CALLBACK WinMain(
//     HINSTANCE hInstance,
//     HINSTANCE hPrevInstance,
//     LPSTR lpCmdLine,
//     int nCmdShow) { }
