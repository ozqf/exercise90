#pragma once

#include "com_module.h"

#define VAR_FIELD_LENGTH 32

struct Var
{
	char name[VAR_FIELD_LENGTH];
	char value[VAR_FIELD_LENGTH];
};

struct VarSet
{
	Var* vars = NULL;
	i32 numVars = 0;
	i32 maxVars = 0;
};

inline Var* COM_GetVar(Var* vars, i32 numVars, char* name)
{
	for (int i = 0; i < numVars; ++i)
	{
		if (COM_CompareStrings(vars[i].name, name) == 0)
		{
			return &vars[i];
		}
	}
	return NULL;
}

inline void COM_SetVar(Var* source, Var* vars, i32* numVars, i32 maxVars)
{
	Var* target = COM_GetVar(vars, *numVars, source->name);
	if (target == NULL)
	{
		printf("Creating var %s as %s\n", source->name, source->value);
		Assert(*numVars < maxVars);
		target = &vars[*numVars];
		*numVars += 1;
	}
	else
	{
		printf("Patching var %s to %s\n", source->name, source->value);
	}
	*target = *source;
}

inline void COM_SetVarByString(char* name, char* value, Var* vars, i32* numVars, i32 maxVars)
{
	Var v = {};
	COM_CopyStringLimited(name, v.name, VAR_FIELD_LENGTH);
	COM_CopyStringLimited(value, v.value, VAR_FIELD_LENGTH);
	COM_SetVar(&v, vars, numVars, maxVars);

}

///////////////////////////////////////////////////////////////
// Read ini file
///////////////////////////////////////////////////////////////
static char* COM_ParseVar(char* buffer, VarSet* varSet)
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
		COM_SetVar(&v, varSet->vars, &varSet->numVars, varSet->maxVars);
	}
	// eat any remaining whitespace
    return COM_RunToNewLine(buffer);
}

static char* COM_LookForVarInLine(char* buffer, VarSet* varSet)
{
    char c = *buffer;
    if (*buffer > 32 && c < 127)
    {
        //printf("Variable: ");
        //char* varBuffer = buffer;
        //Utils_PrintToTerminator(buffer, '\n');
        buffer = COM_ParseVar(buffer, varSet);
    }
    else
    {
        //printf("Blank");
        buffer = COM_RunToNewLine(buffer);
    }
    return buffer;
}

static void COM_ReadVariablesBuffer(char* buffer, u32 numBytes, VarSet* varSet)
{
    printf("Reading data vars from %d bytes\nVars loaded: %d, vars max: %d\n",
		numBytes,
		varSet->numVars,
		varSet->maxVars
	);
    char* end = buffer + numBytes;
    while (buffer < end)
    {
		// Each of these cases should consume an entire line
        if (*buffer == '#')
        {
            buffer = COM_RunToNewLine(buffer);
        }
        else if (*buffer == '[')
        {
	    	buffer++;
	    	buffer = COM_RunToNewLine(buffer);
        }
	    else
	    {
            buffer = COM_LookForVarInLine(buffer, varSet);
	    }
        buffer++;
    }
}

inline void COM_ReadVariablesBuffer(char* buffer, u32 numBytes, Var* vars, i32* numVars, i32 maxVars)
{
	VarSet set = {};
	set.vars = vars;
	set.numVars = *numVars;
	set.maxVars = maxVars;
	COM_ReadVariablesBuffer(buffer, numBytes, &set);
	*numVars = set.numVars;
}
