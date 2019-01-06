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

Var* COM_GetVar(Var* vars, i32 numVars, char* name);
void COM_SetVar(Var* source, Var* vars, i32* numVars, i32 maxVars);
void COM_SetVarByString(char* name, char* value, Var* vars, i32* numVars, i32 maxVars);
///////////////////////////////////////////////////////////////
// Read ini file
///////////////////////////////////////////////////////////////
com_internal char* COM_ParseVar(char* buffer, VarSet* varSet);
com_internal char* COM_LookForVarInLine(char* buffer, VarSet* varSet);
com_internal void COM_ReadVariablesBuffer(char* buffer, u32 numBytes, VarSet* varSet);
void COM_ReadVariablesBuffer(char* buffer, u32 numBytes, Var* vars, i32* numVars, i32 maxVars);
