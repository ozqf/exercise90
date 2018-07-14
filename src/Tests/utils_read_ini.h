#pragma once

#include "../common/com_module.cpp"

#define INI_READ_MODE_NONE 0
#define INI_READ_MODE_SECTION_NAME 1
#define INI_READ_MODE_VARIABLE_NAME 2

#define MAX_VARS 256
Var g_vars[MAX_VARS];
i32 g_nextVar = 0;

char* Utils_EatIniLine(char* buffer)
{
    u8 reading = true;
    while (reading)
    {
        if (*buffer == '\n' || *buffer == EOF)
        {
            reading = false;
        }
        else
        {
            ++buffer;
        }
    }
    return buffer;
}

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

char* Utils_ReadIniVar(char* buffer)
{
    // look for '=' splitter and new line splitter.
	// make sure string lengths are capped!
    char* search = buffer;
    i32 nameLength = 0;
    i32 totalLength = -1;
    while (totalLength == -1)
    {
		//printf("%c", *search);
		if (*search == '\r' || *search == '\n' || *search == '\0' || *search == EOF)
		{
			totalLength = (search - buffer);
		}
        else if (*search == '=')
        {
            nameLength = (search - buffer) ;
        }
        search++;
    }
	if (nameLength <= 0 || totalLength <= 0 || nameLength >= totalLength)
	{
		//printf("variable has no nameLength, aborted... ");
	}
	else
	{
		// Reserve for null terminator
		if (nameLength > 30)
		{
			nameLength = 30;
		}

		i32 valueStart = nameLength + 1;
		i32 valueLength = totalLength - valueStart;
		if (valueLength > 30)
		{
			valueLength = 30;
		}
		//printf("Name length %d, value length: %d, total length %d ", nameLength, valueLength, totalLength);

		Var v = {};

		COM_CopyStringLimited(buffer, v.name, nameLength);
		v.name[nameLength] = 0;
		COM_CopyStringLimited(buffer + valueStart, v.value, valueLength);
		v.value[valueLength] = 0;
		//printf("%s: %s", v.name, v.value);
		Vars_SetVar(&v, g_vars, &g_nextVar, MAX_VARS);
	}
	// eat any remaining whitespace
    return Utils_EatIniLine(buffer);
}

char* Utils_LookForIniVar(char* buffer)
{
    char c = *buffer;
    if (*buffer > 32 && c < 127)
    {
        //printf("Variable: ");
        //char* varBuffer = buffer;
        //Utils_PrintToTerminator(buffer, '\n');
        buffer = Utils_ReadIniVar(buffer);
    }
    else
    {
        //printf("Blank");
        buffer = Utils_EatIniLine(buffer);
    }
    return buffer;
}

void Utils_ReadIniBuffer(char* buffer, u32 numBytes)
{
    printf("Reading settings from %d bytes\n", numBytes);
    char* end = buffer + numBytes;
    i32 mode = 0;
	i32 line = 1;
    while (buffer < end)
    {
		//printf("Line %d: ", line);

        // Each of these cases should consume an entire line
        if (*buffer == '#')
        {
            buffer = Utils_EatIniLine(buffer);
	    	//line++;
        }
        else if (*buffer == '[')
        {
	    	//printf("Section header: ");
	    	// move past opening bracket
	    	buffer++;
	    	//buffer = Utils_PrintToTerminator(buffer, ']');
	    	buffer = Utils_EatIniLine(buffer);
	    	//line++;
        }
	    else
	    {
	    	//printf("variable: ");
            buffer = Utils_LookForIniVar(buffer);
	    	//buffer = Utils_PrintToTerminator(buffer, '\n');
	    }
        line++;
		//printf("\n");
        buffer++;
    }
    //printf("%s\n", buffer);
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
        Utils_ReadIniBuffer(buffer, end);
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
