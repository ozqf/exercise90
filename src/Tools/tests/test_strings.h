#pragma once

#include "../../common/com_module.h"

//////////////////////////////////////////////////////////////
// String tests
//////////////////////////////////////////////////////////////

i32 Test_ParseTestMapPath(char* path, i32 failureDefault)
{
	char* header = "TEST_";
	if (COM_MatchStringStart(path, header))
    {
		i32 index = COM_StripTrailingInteger(path, '_', failureDefault);
		return index;
    }
    else
    {
		return failureDefault;
    }
}

void Test_Strings()
{
    printf("String tests\n");
	i32 failureDefault = -2;
    TEST_ASSERT(Test_ParseTestMapPath("TEST_1", failureDefault) == 1, "Parse Map Path");
	TEST_ASSERT(Test_ParseTestMapPath("TEST_-1", failureDefault) == -1, "Parse Map Path == -1");
	TEST_ASSERT(Test_ParseTestMapPath("TEST_37", failureDefault) == 37, "Parse Map Path == 37");
	TEST_ASSERT(Test_ParseTestMapPath("LEVEL_15", failureDefault) == failureDefault, "Parse Map Path fails");
	
    char* thisStringShouldOverflow = "12345678901234567890";
    
	char buf8[8];
    COM_CopyStringLimited(thisStringShouldOverflow, buf8, 8);
	TEST_ASSERT(!COM_CompareStrings(buf8, "1234567"), "Copy string limited");

	char buf10[10];
	COM_CopyStringLimited(thisStringShouldOverflow, buf10, 10);
	TEST_ASSERT(!COM_CompareStrings(buf10, "123456789"), "Copy string limited");
    
}
