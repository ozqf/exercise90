#pragma once

#include "com_module.h"

////////////////////////////////////////////
// Ultra basic tracking of memory allocations
////////////////////////////////////////////
struct MallocItem
{
    void* ptr;
    // for debugging
    i32 capacity;
    char* label;
};

struct MallocList
{
    MallocItem* items;
    i32 next;
    i32 max;
    i32 totalBytes;
};

internal MallocList COM_InitMallocList(MallocItem* items, i32 capacity)
{
    MallocList list = {};
    list.items = items;
    list.next = 0;
    list.max = capacity;
    list.totalBytes = 0;
    return list;
}

internal void* COM_Malloc(MallocList* list, i32 capacity, char* label)
{
    MallocItem* a = &list->items[list->next++];
    a->ptr = malloc(capacity);
    Assert(a->ptr)
    a->capacity = capacity;
    a->label = label;
    list->totalBytes += capacity;
    return a->ptr;
}
#if 0
internal Alloc* COM_SetAlloc(MallocList* list, void* ptr, i32 capacity, char* label)
{
    Alloc* a = &list->items[list->next++];
    a->ptr = ptr;
    a->capacity = capacity;
    a->label = label;
    return a;
}
#endif
