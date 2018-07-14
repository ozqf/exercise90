#pragma once

#include "com_module.h"

struct Var
{
	char name[32];
	char value[32];
};

inline Var* Vars_GetVar(Var* vars, i32 numVars, char* name)
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

inline void Vars_SetVar(Var* source, Var* vars, i32* numVars, i32 maxVars)
{
	Var* target = Vars_GetVar(vars, *numVars, source->name);
	if (target == NULL)
	{
		Assert(*numVars < maxVars);
		target = &vars[*numVars];
		*numVars += 1;
	}
	*target = *source;
}
