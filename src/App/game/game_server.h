#pragma once

#include "app_module.cpp"

void SV_IterateImportance(EntityLink* links, i32 numLinks)
{
    /*
    Network entity update priority queue:
    > Increase each entity link's importance by it's priority
    > Sort list by descending importance.
    > Iterate over sorted list, writing entity updates until
        packet is full. Reset importance to 0 for each entity
        written
    > Send packet of updates to client
    */
    for (i32 i = 0; i < numLinks; ++i)
    {
        links->important += priority;
    }

    // Sort list
}
