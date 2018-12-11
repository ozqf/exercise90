#pragma once

//////////////////////////////////////////////////
// Preprocessor Test
//////////////////////////////////////////////////


//////////////////////////////////////////////////
// Testing expansion rules and strings
//////////////////////////////////////////////////

#define i32 unsigned int
struct IntrospectTable;
struct TestType;

#define ADD_FIELD(ptrTable, varIndex, ptrStruct, varType, varName, sizeInBytes) \
{ \
	char* varLabel = "##varName##"; \
    char* varLabel = varName \
    char* varLabel = #varName \
    char* varLabel = #varName# \
    char* varLabel = ##varName \
    
	i32 offsetInStruct = (i32)((u8*)&##ptrStruct##->##varName - (u8*)&##ptrStruct##); \
	Test_SetTableVar( \
		ptrTable##, varIndex##, varType##, sizeInBytes##, offsetInStruct, varLabel \
	); \
}


static void Test_BuildTestTypeIntrospectionTable(IntrospectTable* t)
{
	TestType item = {};
	TestType* ptrItem = &item;
	
	// Via macro
	ADD_FIELD(t, 0, ptrItem, INTROSPECTION_TYPE_BYTES, a, sizeof(i32))
}




#if 0
//////////////////////////////////////////////////
// Generated basic component functions
//////////////////////////////////////////////////
#define u32 unsigned int
#define u8 unsigned char
#define ASSERT(expr) if (!(expr)) { *(int *)0 = 0; }

#define MAX_FOO 2048
#define MAX_BAR 2048

#define NULL 0

//////////////////////////////////////////////////
// Buffer read/write test
//////////////////////////////////////////////////

void COM_CopyMemory(u8* source, u8* target, u32 numBytes)
{
	ASSERT(source != NULL);
	ASSERT(target != NULL);
    u32 progress = 0;
    while (progress < numBytes)
    {
        *target = *source;
        source++;
        target++;
		progress++;
    };
}

inline u32 COM_GetBufferSpace(u8* currentPosition, u8* bufferStart, u32 bufferSize)
{
	u8* end = bufferStart + bufferSize;
	return end - currentPosition;
}

inline u8 COM_CheckBufferHasSpace(u8* currentPosition, u8* bufferStart, u32 bufferSize, u32 objectSize)
{
	return (COM_GetBufferSpace(currentPosition, bufferStart, bufferSize) >= objectSize);
}

#ifndef COM_COPY
#define COM_COPY(ptrSrc, ptrDest, dataType) \
COM_CopyMemory((u8*)##ptrSrc##, (u8*)##ptrDest##, sizeof(##dataType##))
//ptrPosition = (u8*)
#endif

/*
To handle an event:
> type constant to check against
> Struct type
> Event handler function name (with common interface)
*/

#ifndef COM_HANDLE_EVENT_CASE
#define COM_HANDLE_EVENT_CASE(eventTypeInteger, structType, ptrBytes, ptrBufferStart, bufferSize, functionName) \
case eventTypeInteger##: \
{ \
	ASSERT(COM_CheckBufferHasSpace((u8*)##ptrBytes##, (u8*)ptrBufferStart##, bufferSize##, sizeof(##structType##))); \
    structType ev = {}; \
    COM_COPY(##ptrBytes##, &##ev##, structType##); \
    ptrBytes = ((u8*)##ptrBytes + sizeof(##structType##)); \
} break;
#endif
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
    u32 type;
    u32 data;
};

struct TestEvent_B
{
    u32 type;
    u32 data;
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

    u8 buffer[32];
    u8* pos = buffer;
    COM_COPY(ptrS1, buffer, TestEvent);

    COM_COPY(buffer, ptrS1, TestEvent);
	
	u32 val = 1;
	
	switch (val)
	{
		COM_HANDLE_EVENT_CASE(1, TestEvent, pos, buffer, 32, HandleTestEvent)
		COM_HANDLE_EVENT_CASE(2, TestEvent_B, pos, buffer, 32, HandleTestEvent_B)
	}
}

//////////////////////////////////////////////////
// Entity system test
//////////////////////////////////////////////////
union EntId
{
    struct
    {
        u32 index;
        u32 iteration;
    };
    u32 arr[2];
};

inline void EntId_Copy(EntId* source, EntId* target)
{
    target->index = source->index;
    target->iteration = source->iteration;
}

inline u8 EntId_Equals(EntId* a, EntId* b)
{
    return (a->index == b->index && a->iteration == b->iteration);
}

struct Ent
{
    EntId entId;
    u32 componentFlags;
};

struct Foo
{
    EntId entId;
    u8 inUse;
};

struct Bar
{
    EntId entId;
    u8 inUse;
};

struct AIController
{
    EntId entId;
    u8 inUse;
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////
// Define Entity Component Macros
//////////////////////////////////////////////////
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Define Component flags
//////////////////////////////////////////////////

#define COMP_FLAG_FOO 0x0001
#define COMP_FLAG_BAR 0x0002

//////////////////////////////////////////////////
// Define component list struct
// and Component Add/Remove/Has functions
//////////////////////////////////////////////////
#define DEFINE_ENT_COMPONENT_BASE(type, typeCamelCase, typeFlagDefine) \
\
struct type##List \
{ \
    type* items; \
    unsigned int count; \
}; \
\
static inline type* EntComp_Add##type##(Ent* ent, GameState* gs) \
{ \
    type* comp; \
    for (u32 i = 0; i < gs->##typeCamelCase##List.count; ++i) \
    { \
        comp = &gs->##typeCamelCase##List.items[i]; \
        if (comp->inUse == 0) \
        { \
            comp->inUse = 1; \
            comp->entId.index = ent->entId.index; \
            comp->entId.iteration = ent->entId.iteration; \
            ent->componentFlags |= typeFlagDefine; \
            return comp; \
        } \
    } \
    return NULL; \
}; \
\
static inline void EntComp_Remove##type##(Ent* ent, GameState* gs, type* comp) \
{ \
    ent->componentFlags &= ~typeFlagDefine; \
    comp->inUse = 0; \
}; \
\
static inline unsigned char Ent_Has##type##(Ent* ent) { return (ent->componentFlags & typeFlagDefine); } \

//////////////////////////////////////////////////
// Create Some dummy components
//////////////////////////////////////////////////

DEFINE_ENT_COMPONENT_BASE(AIController, aiController, COMP_FLAG_FOO)

//DEFINE_ENT_COMPONENT_BASE(Bar, bar, COMP_FLAG_BAR)

// Lists are constructed via macro
// struct GameState
// {
//     FooList fooList;
//     //BarList barList;
// };

#define GET_CUBOID(data) &data->cuboid;

GET_CUBOID(shape)
#endif
