#pragma once

#include "../common/com_module.h"

///////////////////////////////////////////////////////
// Introspection data
///////////////////////////////////////////////////////

#define INTROSPECTION_TYPE_NULL 0
#define INTROSPECTION_TYPE_BYTES 1
#define INTROSPECTION_TYPE_INT32 2
#define INTROSPECTION_TYPE_FLOAT32 3
#define INTROSPECTION_TYPE_VECTOR3 4
#define INTROSPECTION_TYPE_NORMAL3 5

struct IntrospectVar
{
	// type describes how to read/write this value
	i32 type;
	// offset from start of structure to start of this value
	i32 offset;
	
	// for debugging
	char* label;
};

struct IntrospectTable
{
	char* label;
	IntrospectVar vars[32];
	i32 numVars;
};

///////////////////////////////////////////////////////
// Test structs
///////////////////////////////////////////////////////

struct TestType
{
	i32 a;
	i32 b;
	i32 c;
};

IntrospectTable TestType_Table;

void Test_PrintIntrospectionTable(IntrospectTable* table)
{
	printf("--- %s contents ---\n", table->label);
	for (i32 i = 0; i < table->numVars; ++i)
	{
		IntrospectVar* var = &table->vars[i];
		printf("%s: type %d offset %d\n", var->label, var->type, var->offset);
	}
}

void Test_PrintTestType(TestType* t)
{
	printf("%d, %d, %d\n", t->a, t->b, t->c);
}

i32 Test_CalcTestTypeDiff(TestType* item1, TestType* item2)
{
	i32 bits = 0;
	if (item1->a != item2->a) { bits |= (1 << 0); }
	if (item1->b != item2->b) { bits |= (1 << 1); }
	if (item1->c != item2->c) { bits |= (1 << 2); }
	return bits;
}

// returns bytes written
i32 Test_WriteTestTypeDelta(u8* itemBytes, i32 diffBits, u8* buffer)
{
	printf("Encoding...\n");
	u8* cursor = buffer;
	IntrospectTable* table = &TestType_Table;
	cursor += COM_WriteI32(diffBits, cursor);
	for (i32 i = 0; i < table->numVars; ++i)
	{
		i32 bit = (1 << i);
		if ((diffBits & bit) != 0)
		{
			IntrospectVar* var = &table->vars[i];
			switch (var->type)
			{
				case INTROSPECTION_TYPE_INT32:
				{
					i32 value = *(i32*)(itemBytes + var->offset);
					printf("Write field %d %s: %d\n", i, var->label, value);
					cursor += COM_WriteI32(value, cursor);
				} break;
			}
		}
	}
	return (cursor - buffer);
}

// returns bytes read
i32 Test_ReadTestTypeDelta(u8* itemBytes, u8* buffer)
{
	u8* cursor = buffer;
	IntrospectTable* table = &TestType_Table;
	i32 diffBits = COM_ReadI32(&cursor);
	for (i32 i = 0; i < table->numVars; ++i)
	{
		i32 bit = diffBits & (1 << i);
		if (bit)
		{
			IntrospectVar* var = &table->vars[i];
			switch (var->type)
			{
				case INTROSPECTION_TYPE_INT32:
				{
					i32 value = COM_ReadI32(&cursor);
					printf("Read field %d %s: %d\n", i, var->label, value);
					*(i32*)(itemBytes + var->offset) = value;
				}
			}
		}
	}
	return (cursor - buffer);
}

void Test_SetTableVar(IntrospectTable* table, i32 index, i32 type, i32 offset, char* label)
{
	// overwrite var count if necessary
	i32 newMaxVar = index + 1;
	if (table->numVars < newMaxVar)
	{
		table->numVars = newMaxVar;
	}
	IntrospectVar* v = &table->vars[index];
	v->offset = offset;
	v->type = type;
	v->label = label;
}

void Test_Introspection()
{
	TestType previousState;
	TestType* ptrPreviousState = &previousState;

	TestType_Table.label = "TestType";
	IntrospectVar* vars = TestType_Table.vars;

	IntrospectTable* t = &TestType_Table;
	Test_SetTableVar(
		t, 0,
		INTROSPECTION_TYPE_INT32,
		(i32)((u8*)&ptrPreviousState->a - (u8*)ptrPreviousState),
		"a");
	Test_SetTableVar(
		t, 1,
		INTROSPECTION_TYPE_INT32,
		(i32)((u8*)&ptrPreviousState->b - (u8*)ptrPreviousState),
		"b");
	Test_SetTableVar(
		t, 2,
		INTROSPECTION_TYPE_INT32,
		(i32)((u8*)&ptrPreviousState->c - (u8*)ptrPreviousState),
		"c");
	
	Test_PrintIntrospectionTable(&TestType_Table);
	
	previousState.a = 567;
	previousState.b = 81;
	previousState.c = -14;

	TestType currentState;
	currentState.a = -60;
	currentState.b = 81;
	currentState.c = -13;
	printf("Items:\n");
	Test_PrintTestType(&previousState);
	Test_PrintTestType(&currentState);

	i32 diffBits = Test_CalcTestTypeDiff(&previousState, &currentState);
	printf("Diff bits (%d):\n", diffBits);
	COM_PrintBits(diffBits, 1);
	u8 buffer[512];
	u8* cursor = buffer;
	cursor += Test_WriteTestTypeDelta((u8*)&currentState, diffBits, cursor);
	i32 written = (cursor - buffer);
	printf("Wrote %d bytes\n", written);
	COM_PrintBytes(buffer, (u16)written, 4);

	printf("Reconstruct from blank:\n");
	TestType result = {};
	cursor = buffer;
	cursor += Test_ReadTestTypeDelta((u8*)&result, cursor);
	Test_PrintTestType(&result);
}
