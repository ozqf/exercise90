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
    unsigned int count; \
    unsigned int max; \
}; \

//////////////////////////////////////////////////
// Construct common component interface
//////////////////////////////////////////////////
/*
type is prefixed with EC_
GameScene must have a matching list, EC_FooList

Functions:
u32		Ent_HasFoo			(Ent*);
void	EC_AddFoo			(GameScene*, Ent*);
void	EC_SetFooInactive	(EC_Foo, u8);
EC_Foo	EC_FindFoo			(GameScene*, EntId*);
EC_Foo	EC_FindFoo			(GameScene*, Ent*);
void	EC_RemoveFoo		(Ent*)
*/

#define DEFINE_ENT_COMPONENT_BASE(type, typeCamelCase, typeFlagDefine) \
\
static u32 Ent_Has##type##(Ent* ent) { return (u32)(ent->componentBits & typeFlagDefine); } \
\
static EC_##type* EC_Add##type##(GameScene* gs, Ent* ent) \
{ \
    EC_##type* comp; \
    for (u32 i = 0; i < gs->##typeCamelCase##List.count; ++i) \
    { \
        comp = &gs->##typeCamelCase##List.items[i]; \
        if (comp->header.inUse == 0) \
        { \
            comp->header.inUse = 1; \
            comp->header.entId.index = ent->entId.index; \
            comp->header.entId.iteration = ent->entId.iteration; \
            ent->componentBits |= typeFlagDefine; \
            return comp; \
        } \
    } \
    return NULL; \
}; \
\
static void EC_Set##type##Inactive(EC_##type##* comp, u8 isInActive) \
{ \
	if (isInActive) { comp->state.flags |= EC_STATE_FLAG_INACTIVE; } \
	else { comp->state.flags &= ~EC_STATE_FLAG_INACTIVE; } \
} \
\
static EC_##type* EC_Find##type##(GameScene* gs, Ent* ent) \
{ \
    if (!Ent_Has##type##(ent)) { return 0; } \
    EC_##type* comp; \
    for (u32 i = 0; i < gs->##typeCamelCase##List.count; ++i) \
    { \
        comp = &gs->##typeCamelCase##List.items[i]; \
        if (comp->header.inUse == 1 && ent->entId.value == comp->header.entId.value) \
        { \
            return comp; \
        } \
    } \
    return 0; \
} \
static EC_##type* EC_Find##type##(GameScene* gs, EntId* id) \
{ \
    EC_##type* comp; \
    for (u32 i = 0; i < gs->##typeCamelCase##List.count; ++i) \
    { \
        comp = &gs->##typeCamelCase##List.items[i]; \
        if (comp->header.inUse == 1 && id->value == comp->header.entId.value) \
        { \
            return comp; \
        } \
    } \
    return 0; \
} \
\
\
static void EC_Remove##type##(GameScene* gs, Ent* ent) \
{ \
    EC_##type* comp = EC_Find##type##(gs, ent); \
    if (comp == 0) { return; } \
    ent->componentBits &= ~typeFlagDefine; \
    comp->header.inUse = 0; \
}; \
\

