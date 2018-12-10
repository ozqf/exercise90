#pragma once

#include "../common/com_module.h"

///////////////////////////////////////////////////////
// Introspection data
///////////////////////////////////////////////////////

#define INTROSPECTION_TYPE_NULL 0
#define INTROSPECTION_TYPE_INT32 1
#define INTROSPECTION_TYPE_FLOAT32 2
#define INTROSPECTION_TYPE_VECTOR3 3
#define INTROSPECTION_TYPE_NORMAL3 4

struct IntrospectVar
{
	// offset from start of structure to start of this value
	i32 offset;
	// type describes how to read/write this value
	i32 type;
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
i32 Test_WriteTestTypeDelta(TestType* item, i32 diffBits, u8* buffer)
{
	printf("Encoding...\n");
	u8* itemBytes = (u8*)item;
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
i32 Test_ReadTestTypeDelta(TestType* item, u8* buffer)
{
	u8* itemBytes = (u8*)item;
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

void Test_Introspection()
{
	TestType previousState;
	TestType* ptrPreviousState = &previousState;

	TestType_Table.label = "TestType";
	IntrospectVar* vars = TestType_Table.vars;

	vars[0].offset = (i32)((u8*)&ptrPreviousState->a - (u8*)ptrPreviousState);
	vars[0].type = INTROSPECTION_TYPE_INT32;
	vars[0].label = "a";
	
	vars[1].offset = (i32)((u8*)&ptrPreviousState->b - (u8*)ptrPreviousState);
	vars[1].type = INTROSPECTION_TYPE_INT32;
	vars[1].label = "b";
	
	vars[2].offset = (i32)((u8*)&ptrPreviousState->c - (u8*)ptrPreviousState);
	vars[2].type = INTROSPECTION_TYPE_INT32;
	vars[2].label = "c";

	TestType_Table.numVars = 3;
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
	cursor += Test_WriteTestTypeDelta(&currentState, diffBits, cursor);
	i32 written = (cursor - buffer);
	printf("Wrote %d bytes\n", written);
	COM_PrintBytes(buffer, (u16)written, 4);

	printf("Reconstruct from blank:\n");
	TestType result = {};
	cursor = buffer;
	cursor += Test_ReadTestTypeDelta(&result, cursor);
	Test_PrintTestType(&result);
}
