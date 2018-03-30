#pragma once

//////////////////////////////////////////////////
// Construct component list struct
//////////////////////////////////////////////////
/*
eg:
DEFINE_ENT_COMPONENT_LIST(Foo)
struct FooList
{
    Foo* items;
    unsigned int count;
}
*/
#define DEFINE_ENT_COMPONENT_LIST(type) \
\
struct EC_##type##List \
{ \
    EC_##type* items; \
    unsigned int max; \
    unsigned int count; \
}; \

//////////////////////////////////////////////////
// Construct Component Add/Remove/Has functions.
//////////////////////////////////////////////////
/*
Functions:
EC_AddFoo
*/

#define DEFINE_ENT_COMPONENT_BASE(type, typeCamelCase, typeFlagDefine) \
\
static inline EC_##type* EC_Add##type##(Ent* ent, World* world) \
{ \
    EC_##type* comp; \
    for (u32 i = 0; i < world->##typeCamelCase##List.count; ++i) \
    { \
        comp = &world->##typeCamelCase##List.items[i]; \
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

#if 0
\
static inline void EC_Remove##type##(Ent* ent, World* world, type* comp) \
{ \
    ent->componentFlags &= ~typeFlagDefine; \
    comp->inUse = 0; \
}; \
\
static inline unsigned char Ent_Has##type##(Ent* ent) { return (ent->componentFlags & typeFlagDefine); } \

#endif
