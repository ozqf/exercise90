#pragma once
//////////////////////////////////////////////////
// Preprocessor Test
// Generated basic component functions
//////////////////////////////////////////////////
#define u32 unsigned int
#define u8 unsigned char
#define ASSERT(expr) if (!(expr)) { *(int *)0 = 0; }

#define MAX_FOO 2048
#define MAX_BAR 2048

#define NULL 0




/////////////////////////////////////////////////////////////////////////////////
// Copy Struct
/////////////////////////////////////////////////////////////////////////////////
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

#ifndef COM_COPY
#define COM_COPY(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##))
#endif
// /COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##));
#ifndef COM_COPY_AND_MOVE
#define COM_COPY_AND_MOVE(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##)); \
ptrDestination = (u8*)(##ptrDestination + sizeof(##structType##));
#endif

struct Foo
{
    u32 value1;
    u32 value2;
};

void WriteFoo()
{
    Foo foo = {};
    Foo* ptrFoo = &foo;
    u8 buffer[64];
    // copy
    COM_COPY(ptrFoo, buffer, Foo);
    u8* b2 = buffer;
    // copy and move
    COM_COPY_AND_MOVE(ptrFoo, b2, Foo)

    COM_CopyMemory((unsigned char*)ptrFoo, (unsigned char*)b2, sizeof(Foo)); b2 = (unsigned char*)(b2 + sizeof(Foo));
}


/////////////////////////////////////////////////////////////////////////////////
// Entity system
/////////////////////////////////////////////////////////////////////////////////
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

DEFINE_ENT_COMPONENT_BASE(FooList, fooList, COMP_FLAG_FOO)

//DEFINE_ENT_COMPONENT_BASE(Bar, bar, COMP_FLAG_BAR)

// Lists are constructed via macro
struct GameState
{
    FooList fooList;
    //BarList barList;
};

#define GET_CUBOID(data) &data->cuboid;

//GET_CUBOID(shape)
