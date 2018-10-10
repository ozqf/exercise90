#pragma once

#include "../common/com_module.h"
/*
=== Entity Component interface ===
Add
Remove
Find
Has
Update

Create Memory
Assign Memory
Clear Memory

*/
#define EC_COMP0_BIT (1 << 0)
#define EC_COMP1_BIT (1 << 1)
#define EC_COMP2_BIT (1 << 2)

struct EC_Component0
{
	i32 val0;
	i32 val1;
};

struct EC_Component1
{
	i32 val0;
	i32 val1;
	i32 val2;
};

struct EC_Component2
{
	char str[64];
};

struct EntityFullState
{
	u32 componentBits;
	EC_Component0 comp0;
	EC_Component1 comp1;
	EC_Component2 comp2;
};

typedef void (*EC_Update)(void* gs, GameTime* time);

struct EntityComponent
{
    i32 flag;
    EC_Update updateFunction;
};

void Test_PrintState(char* title, EntityFullState* state)
{
	printf("%s:\n %d, %d, %d, %d, \"%s\"\n",
		title,
		state->comp0.val0, state->comp0.val1,
		state->comp1.val0, state->comp1.val1,
		state->comp2.str
	);
}

// Create a bitmask of components that are different between these two states.
u32 Test_CompareStates(EntityFullState* a, EntityFullState* b)
{
	u32 diffBits = 0;
	i32 result;
	
	result = COM_CompareMemory(
		(u8*)&a->comp0,
		(u8*)&b->comp0,
		sizeof(EC_Component0));
	if (!result)
	{
		diffBits |= EC_COMP0_BIT;
	}
	
	result = COM_CompareMemory(
		(u8*)&a->comp1,
		(u8*)&b->comp1,
		sizeof(EC_Component1));
	if (!result)
	{
		diffBits |= EC_COMP1_BIT;
	}
	
	result = COM_CompareMemory(
		(u8*)&a->comp2,
		(u8*)&b->comp2,
		sizeof(EC_Component2));
	if (!result)
	{
		diffBits |= EC_COMP2_BIT;
	}
	
	return diffBits;
}

void Test_EntityComponentSystem()
{
	printf("Test ECS - Entity Component Diff detection\n");
	/*
	Idea behind this system is to reduce data requirements for spawn commands
	whilst allowing potentially any changes in spawn state.
	TLDR: Spawn commands are entId, templateId and differences from the template.
	
	
	The game spawns by processing a complete entity state structure to recreate
	every component exactly as specified (see Ent_ApplyStateData).
	
	Prefined entities are stored as templates, located via an id or label.
	These templates can be cloned to recreate a given entity.
	
	The diff bits tell the game which components match the template exactly and 
	thus do not require copying.
	
	When producing a spawn command: Locate the template, apply changes, calculate
	diffbits, then transmit the templateId + different component state.
	
	When processing a spawn command: Clone the template, use diffbits to read
	alterations and apply them directly to the clone. Then send this complete
	state to the entity factory.
	
	*/
	
	EntityFullState original = {};
	original.comp0.val0 = 123;
	original.comp0.val1 = 456;
	
	original.comp1.val0 = 789;
	original.comp1.val1 = 12;
	
	COM_CopyStringLimited("Some string here", original.comp2.str, 64);
	
	EntityFullState copy = original;
	copy.comp0.val1 = 5432;
	
	COM_CopyStringLimited("Some string there", copy.comp2.str, 64);
	
	Test_PrintState("Original", &original);
	Test_PrintState("Copy", &copy);
	
	u32 diffBits = Test_CompareStates(&original, &copy);
	printf("Diffbits:\n");
	COM_PrintBits((i32)diffBits, 1);
}
