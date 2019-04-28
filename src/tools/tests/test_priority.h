#pragma once

#include "../../common/common.h"
#include "../../app/priority_queue.h"

internal void ListPriority(PriorityLinkSet* list)
{
    printf("Priority list (%d/%d items)\n",
		list->numLinks, list->maxLinks);
    for (i32 i = 0; i < list->numLinks; ++i)
    {
        printf("%d Link id %d priority %.3f importance: %.3f\n",
			i, 
			list->links[i].id,
			list->links[i].priority,
			list->links[i].importance);
    }
}

internal void ResetTopPriorityItems(PriorityLinkSet* list, i32 numToClear)
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
	
	PriorityLinkSet list = {};
	i32 maxLinks = 8;
	i32 bytes = sizeof(PriorityLink) * maxLinks;
	list.links = (PriorityLink*)malloc(bytes);
	COM_ZeroMemory((u8*)list.links, bytes);
	
	list.maxLinks = 8;
	
	Priority_AddLink(&list, 1, 2);
	Priority_AddLink(&list, 2, 1);
	Priority_AddLink(&list, 3, 4);
	Priority_AddLink(&list, 4, 3);
	Priority_AddLink(&list, 5, 2);
	Priority_AddLink(&list, 6, 4);
	Priority_AddLink(&list, 7, 5);
	Priority_AddLink(&list, 7, 5);
	printf("Empty list:\n");
	ListPriority(&list);
	for (i32 i = 0; i < 10; ++i)
	{
		Priority_TickQueue(&list);
	}
	Priority_FlagLinkAsDead(
		&list,
		Priority_GetLinkIndexById(&list, 4)
	);
	Priority_FlagLinkAsDead(
		&list,
		Priority_GetLinkIndexById(&list, 2)
	);
	ResetTopPriorityItems(&list, 3);
	Priority_TickQueue(&list);
	printf("Ticked list:\n");
	ListPriority(&list);
	/*
    const i32 numLinks = 4;
    PriorityLink links[numLinks];
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
