#pragma once

#include "../../common/com_module.h"
#include "../../app/priority_queue.h"

internal void ListPriority(SVEntityLinkArray* list)
{
    printf("Priority list (%d/%d items)\n",
		list->numLinks, list->maxLinks);
    for (i32 i = 0; i < list->numLinks; ++i)
    {
        printf("%d Link id %d priority %d importance: %d\n",
			i, 
			list->links[i].id,
			list->links[i].priority,
			list->links[i].importance);
    }
}

internal void ResetTopPriorityItems(SVEntityLinkArray* list, i32 numToClear)
{
	if (numToClear > list->numLinks)
	{
		numToClear = list->numLinks;
	}
	for (i32 i = 0; i < numToClear; ++i)
	{
		list->links[i].importance = 0;
	}
}

internal void Test_Priority()
{
    printf("Test Priority\n");
	
	SVEntityLinkArray list = {};
	i32 maxLinks = 8;
	i32 bytes = sizeof(SVEntityLink) * maxLinks;
	list.links = (SVEntityLink*)malloc(bytes);
	COM_ZeroMemory((u8*)list.links, bytes);
	
	list.maxLinks = 8;
	
	SV_AddPriorityLink(&list, 1, 2);
	SV_AddPriorityLink(&list, 2, 1);
	SV_AddPriorityLink(&list, 3, 4);
	SV_AddPriorityLink(&list, 4, 3);
	SV_AddPriorityLink(&list, 5, 2);
	SV_AddPriorityLink(&list, 6, 4);
	SV_AddPriorityLink(&list, 7, 5);
	SV_AddPriorityLink(&list, 7, 5);
	printf("Empty list:\n");
	ListPriority(&list);
	for (i32 i = 0; i < 10; ++i)
	{
		SV_TickPriorityQueue(list.links, list.numLinks);
	}
	SV_RemovePriorityLinkByIndex(
		&list,
		SV_GetPriorityLinkIndexById(&list, 4)
	);
	SV_RemovePriorityLinkByIndex(
		&list,
		SV_GetPriorityLinkIndexById(&list, 2)
	);
	ResetTopPriorityItems(&list, 3);
	SV_TickPriorityQueue(list.links, list.numLinks);
	printf("Ticked list:\n");
	ListPriority(&list);
	/*
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
	*/
}
