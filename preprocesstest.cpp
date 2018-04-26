#line 1 "preprocess/preprocesstest.h"
#pragma once
//////////////////////////////////////////////////
// Preprocessor Test
// Generated basic component functions
//////////////////////////////////////////////////









//////////////////////////////////////////////////
// Buffer read/write test
//////////////////////////////////////////////////

void COM_CopyMemory(unsigned char* source, unsigned char* target, unsigned int numBytes)
{
	if (!(source != 0)) { *(int *)0 = 0; };
	if (!(target != 0)) { *(int *)0 = 0; };
    unsigned int progress = 0;
    while (progress < numBytes)
    {
        *target = *source;
        source++;
        target++;
		progress++;
    };
}

inline unsigned int COM_GetBufferSpace(unsigned char* currentPosition, unsigned char* bufferStart, unsigned int bufferSize)
{
	unsigned char* end = bufferStart + bufferSize;
	return end - currentPosition;
}

inline unsigned char COM_CheckBufferHasSpace(unsigned char* currentPosition, unsigned char* bufferStart, unsigned int bufferSize, unsigned int objectSize)
{
	return (COM_GetBufferSpace(currentPosition, bufferStart, bufferSize) >= objectSize);
}




//ptrPosition = (u8*)
#line 49 "preprocess/preprocesstest.h"

/*
To handle an event:
> type constant to check against
> Struct type
> Event handler function name (with common interface)
*/










#line 67 "preprocess/preprocesstest.h"
/*
#ifndef COM_HANDLE_EVENT_CASE
#define COM_HANDLE_EVENT_CASE(eventTypeInteger, structType, ptrBytes, ptrBufferStart, bufferSize, functionName) \
case eventTypeInteger##: \
{ \
	ASSERT(COM_CheckBufferHasSpace((u8*)##ptrBytes##, (u8*)ptrBufferStart##, bufferSize##, sizeof(##structType##))); \
	structType ev = {}; \
	COM_COPY((u8*)##ptrBytes##, (u8*)##&##ev##, structType##); \
	ptrBytes = (u8*)(##ptrBytes + sizeof(##structType##)); \
	functionName##(&##ev##); \
} break;
#endif
*/

struct TestEvent
{
    unsigned int type;
    unsigned int data;
};

struct TestEvent_B
{
    unsigned int type;
    unsigned int data;
};

void HandleTestEvent(TestEvent* ev)
{
	
}

void HandleTestEvent_B(TestEvent_B* ev)
{
	
}

void SomeFunction()
{
    TestEvent s1 = {};
    TestEvent* ptrS1 = &s1;

    unsigned char buffer[32];
    unsigned char* pos = buffer;
    COM_CopyMemory((unsigned char*)ptrS1, (unsigned char*)buffer, sizeof(TestEvent));

    COM_CopyMemory((unsigned char*)buffer, (unsigned char*)ptrS1, sizeof(TestEvent));
	
	unsigned int val = 1;
	
	switch (val)
	{
		case 1:
        {
            if (!(COM_CheckBufferHasSpace((unsigned char*)pos, (unsigned char*)buffer, 32, sizeof(TestEvent)))) { *(int *)0 = 0; };
            TestEvent ev = {};
            COM_CopyMemory((unsigned char*)pos, (unsigned char*)&ev, sizeof(TestEvent));
            pos = ((unsigned char*)pos + sizeof(TestEvent)); } break;
		case 2: { if (!(COM_CheckBufferHasSpace((unsigned char*)pos, (unsigned char*)buffer, 32, sizeof(TestEvent_B)))) { *(int *)0 = 0; }; TestEvent_B ev = {}; COM_CopyMemory((unsigned char*)pos, (unsigned char*)&ev, sizeof(TestEvent_B)); pos = ((unsigned char*)pos + sizeof(TestEvent_B)); } break;
	}
}

//////////////////////////////////////////////////
// Entity system test
//////////////////////////////////////////////////
union EntId
{
    struct
    {
        unsigned int index;
        unsigned int iteration;
    };
    unsigned int arr[2];
};

inline void EntId_Copy(EntId* source, EntId* target)
{
    target->index = source->index;
    target->iteration = source->iteration;
}

inline unsigned char EntId_Equals(EntId* a, EntId* b)
{
    return (a->index == b->index && a->iteration == b->iteration);
}

struct Ent
{
    EntId entId;
    unsigned int componentFlags;
};

struct Foo
{
    EntId entId;
    unsigned char inUse;
};

struct Bar
{
    EntId entId;
    unsigned char inUse;
};

struct AIController
{
    EntId entId;
    unsigned char inUse;
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////
// Define Entity Component Macros
//////////////////////////////////////////////////
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Define Component flags
//////////////////////////////////////////////////




//////////////////////////////////////////////////
// Define component list struct
// and Component Add/Remove/Has functions
//////////////////////////////////////////////////


































//////////////////////////////////////////////////
// Create Some dummy components
//////////////////////////////////////////////////

struct AIControllerList { AIController* items; unsigned int count; }; static inline AIController* EntComp_AddAIController(Ent* ent, GameState* gs) { AIController* comp; for (unsigned int i = 0; i < gs->aiControllerList.count; ++i) { comp = &gs->aiControllerList.items[i]; if (comp->inUse == 0) { comp->inUse = 1; comp->entId.index = ent->entId.index; comp->entId.iteration = ent->entId.iteration; ent->componentFlags |= 0x0001; return comp; } } return 0; }; static inline void EntComp_RemoveAIController(Ent* ent, GameState* gs, AIController* comp) { ent->componentFlags &= ~0x0001; comp->inUse = 0; }; static inline unsigned char Ent_HasAIController(Ent* ent) { return (ent->componentFlags & 0x0001); }

//DEFINE_ENT_COMPONENT_BASE(Bar, bar, COMP_FLAG_BAR)

// Lists are constructed via macro
// struct GameState
// {
//     FooList fooList;
//     //BarList barList;
// };



&shape->cuboid;
