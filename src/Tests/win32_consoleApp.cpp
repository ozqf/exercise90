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
#include "test_tokenise.h"
#include "test_bitpacking.h"
#include "test_syntax.h"
#include "test_patch_state_file.h"
#include "utils_read_ini.h"
// main function for everywhere except windows
int main(i32 argc, char* argv[])
{
    printf("*** Exercise 90 win32 tests ***\n");
    printf("Built on %s at %s, file: %s, line: %d\n", __DATE__, __TIME__, __FILE__, __LINE__);

    #if 1
	//HeapTest();
	//Test_pak();
	//Test_Pack();
	//Test_ReadMesh();
	//Test_MultiThreading();
	//Test_Tokenise();
    //Test_Syntax();
    //Test_Bitpacking();

    //Test_PatchStateFile();
    //Test_StateSaving();
    Test_ReadIniFile();
    #endif

    #if 0
    printf("\n--- Options ---\n");
    printf("1. Test Common Library\n");
    printf("2. Test Common Library\n");
    printf("3. Read/Write state file \"testbox.lvl\"\n");
    printf("4. Test Base 64\n");
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
            Test_Tokenise();
        } break;

        case '3':
        {
            Test_StateSaving();
        } break;
    
        case '4':
        {
            Test_Base64();
            printf("\nEnd of Tests\n");
        } break;

        default:
        {
            printf("That means jack to me...\n");
        } break;
    }
    #endif
    
    return 0;
}

// Windows 'Main'
// int CALLBACK WinMain(
//     HINSTANCE hInstance,
//     HINSTANCE hPrevInstance,
//     LPSTR lpCmdLine,
//     int nCmdShow) { }
