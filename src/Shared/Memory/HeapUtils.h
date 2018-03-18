#pragma once

#include "../shared.h"
#include "Heap.h"

i32 Heap_AllocString(Heap* heap, BlockRef* ref, i32 maxStringCapacity)
{
    // length of string + 1 for null terminator
    u32 totalSize = maxStringCapacity + 1;
    return Heap_Allocate(heap, ref, totalSize, "ZString");
}
