#pragma once
//////////////////////////////////////////////////////////////
// String tests
//////////////////////////////////////////////////////////////

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

void Test_Strings()
{
    printf("String tests\n");
    Test_ParseTestMapPath("TEST_1");
	Test_ParseTestMapPath("TEST_-1");
	Test_ParseTestMapPath("TEST_37");
	Test_ParseTestMapPath("LEVEL_15");
    char* thisStringShouldOverflow = "12345678901234567890";
    char buf[8];

    COM_CopyStringLimited(thisStringShouldOverflow, buf, 8);
	printf("Copy limited by 8: ");
    printf("%s -> %s\n", thisStringShouldOverflow, buf);
    
}
