#pragma once

#include "../../common/com_defines.h"

struct SVEntityLink
{
    i32 inUse;
    i32 id;
    f32 priority;
    f32 importance;
};

internal void SV_SwapEntityLinks(
    SVEntityLink* a,
    SVEntityLink* b)
{
    SVEntityLink temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

internal i32 SV_CompareLink(SVEntityLink* a, SVEntityLink* b)
{
    if (a->importance < b->importance) { return -1; }
    if (a->importance > b->importance) { return 1; }
    return 0;
}

// Bubble Sort
internal void SV_BubbleSortPriorityQueue(
    SVEntityLink* links, i32 numLinks)
{
    i32 swapped;
    do
    {
        swapped = 0;
        for (i32 i = 0; i < numLinks - 1; ++i)
        {
            SVEntityLink* a = &links[i];
            SVEntityLink* b = &links[i + 1];
            if (SV_CompareLink(a, b) > 0)
            {
                SV_SwapEntityLinks(a, b);
                swapped = 1;
            }
        }
    } while (swapped);
}

/*
Run every tick
> Increment priority levels
> Sort Priority Queue
*/
internal void SV_TickPriorityQueue(SVEntityLink* links, i32 numLinks)
{
	for (i32 i = 0; i < numLinks; ++i)
    {
        SVEntityLink* link = &links[i];
        if (!link->inUse) { continue; }
        link->importance += link->priority;
    }
}
