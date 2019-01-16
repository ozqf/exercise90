#pragma once

#include "../common/com_module.h"

#define INI_READ_MODE_NONE 0
#define INI_READ_MODE_SECTION_NAME 1
#define INI_READ_MODE_VARIABLE_NAME 2

#define MAX_VARS 256
Var g_vars[MAX_VARS];
i32 g_nextVar = 0;

char* Utils_PrintLine(char* buffer)
{
    while (*buffer != '\n')
    {
        printf("%c", *buffer);
        buffer++;
    }
    return buffer;
}

char* Utils_PrintToTerminator(char* buffer, char terminator)
{
	while (*buffer != terminator)
	{
		printf("%c", *buffer);
		buffer++;
	}
	return buffer;
}

void Utils_ReadIniFile(char* fileName)
{
    printf("Read Ini file \"%s\"\n", fileName);
    FILE* f;
    fopen_s(&f, fileName, "rb");
    if (f == NULL)
    {
        printf("  Couldn't open %s\n", fileName);
        return;
    }

    fseek(f, 0, SEEK_END);
    u32 end = ftell(f);
    fseek(f, 0, SEEK_SET);
    //printf("  %d bytes\n", end);

    char* buffer;
	// add space for terminators
	i32 bufferSize = end + 2;
    buffer = (char*)malloc(bufferSize);
    if (buffer == NULL)
    {
        printf("Malloc failed!\n");
    }
    else
    {
        fread(buffer, end, 1, f);
		buffer[bufferSize - 1] = NULL;
		buffer[bufferSize - 2] = '\n';
        COM_ReadVariablesBuffer(buffer, end, g_vars, &g_nextVar, MAX_VARS);
    }

    free(buffer);
    fclose(f);
}

void Test_ListVars()
{
	printf("*** Vars (%d) ***\n", g_nextVar);
	for (int i = 0; i < g_nextVar; ++i)
	{
		printf("%d: %s = %s\n", i, g_vars[i].name, g_vars[i].value);
	}
}

void Test_ReadIniFile()
{
    Utils_ReadIniFile("base\\config.ini");
	Test_ListVars();
    
}
