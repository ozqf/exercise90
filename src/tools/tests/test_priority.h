#pragma once

#include "../../common/com_module.h"
#include "../../app/server/server_priority.h"

internal void ListPriority(SVEntityLink* links, i32 numLinks)
{
    printf("Priority\n");
    for (i32 i = 0; i < numLinks; ++i)
    {
        printf("Link importance: %.3f\n", links[i].importance );
    }
}

internal void Test_Priority()
{
    printf("Test Priority\n");
    const i32 numLinks = 4;
    SVEntityLink links[numLinks];
    links[0] = {};
    links[0].importance = 8;
    links[1] = {};
    links[1].importance = 6;
    links[2] = {};
    links[2].importance = 5;
    links[3] = {};
    links[3].importance = 7;

    ListPriority(links, numLinks);
    SV_BubbleSortPriorityQueue(links, numLinks);
    ListPriority(links, numLinks);
}
