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
#include "test_ecs2.h"
#include "test_misc.h"

#include "test_network_win32.h"


void Test_StripTrailingInt(char* txt)
{
	i32 val = COM_StripTrailingInteger(txt, '_', -1);
	printf("%s val: %d\n", txt, val);
}

void Test_MatchStringStart(char* a, char* b)
{
	i32 result = COM_MatchStringStart(a, b);
	printf("%s starts with %s? %d\n", a, b, result);
}

void Test_ParseTestMapPath(char* path)
{
	char* header = "TEST_";
	if (COM_MatchStringStart(path, header))
    {
		i32 index = COM_StripTrailingInteger(path, '_', 0);
        printf("Path %s index: %d\n", path, index);
    }
    else
    {
        printf("Path %s does not start with %s\n", path, header);
    }
}

// main function for everywhere except windows
int main(i32 argc, char* argv[])
{
    Test_Win32(argc, argv);
	//Test_EntityComponentSystem();
    #if 0
	Test_ParseTestMapPath("TEST_1");
	Test_ParseTestMapPath("TEST_-1");
	Test_ParseTestMapPath("TEST_37");
	Test_ParseTestMapPath("LEVEL_15");
    #endif
	
#if 0
    printf("*** Exercise 90 win32 tests ***\n");
    printf("Built on %s at %s, file: %s, line: %d\n", __DATE__, __TIME__, __FILE__, __LINE__);
	for (int i = 0; i < argc; ++i)
	{
		printf("%d: %s\n", i, argv[i]);
	}
    printf("\n");

	if (argc > 1)
	{
		if (!COM_CompareStrings(argv[1], "state"))
		{
			Test_StateSaving(argc, argv);
		}
        if (!COM_CompareStrings(argv[1], "tokenise"))
        {
            Test_Tokenise();
        }
	}
	else
	{
		printf("EX90 Test/Debug utils:\nTODO: Instructions here!\n");
	}
#endif
	
    #if 0
	//HeapTest();
	//Test_pak();
	//Test_Pack();
	//Test_ReadMesh();
	//Test_MultiThreading();
    //Test_Syntax();
    //Test_Bitpacking();

    //Test_PatchStateFile();
    Test_StateSaving();
    //Test_ReadIniFile();
    //Test_ECS2();
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
