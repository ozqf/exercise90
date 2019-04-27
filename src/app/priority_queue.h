#pragma once
/*
Management of the priority queue used to select unreliable updates

Every Tick:
> Priority is accumulated each frame (link.importance += link.priority)
> List is sorted in order of importance
During packet construction:
> Iterate over the priority queue. Write entities (and reset importance to 0)
	until no space is left.
Repeat
default priority is 1. If a type of object requires more synchronisation
than others, increase its priority value so it accumulates importance faster
*/
#include "../common/common.h"

#define ENT_LINK_STATUS_ACTIVE 0
#define ENT_LINK_STATUS_DEAD 1

struct SVEntityLink
{
    i32 id;             // Serial number of related entity
    u8 status;
    f32 basePriority;
    f32 priority;
    f32 importance;
    i32 lastPacketSent;
    i32 lastPacketAck;

    // used for calculating priority
    f32 distance;
};

struct SVEntityLinkArray
{
	SVEntityLink* links;
	i32 numLinks;
	i32 maxLinks;
};

internal i32 SV_CalcEntityLinkArrayBytes(i32 numEntities)
{
    return sizeof(SVEntityLink) * numEntities;
}

internal void SV_UpdateSyncAcks(
    SVEntityLinkArray* list, i32 packetSequence,
    i32* syncIds, i32 numSyncIds)
{
    //printf("Update sync acks (%d links, %d ids)\n",
    //    list->numLinks, numSyncIds);
    for (i32 i = list->numLinks - 1; i >= 0; --i)
    {
        SVEntityLink* link = &list->links[i];
        for (i32 j = 0; j < numSyncIds; ++j)
        {
            if (link->id == syncIds[j]
                && link->lastPacketAck < packetSequence)
            {
                link->lastPacketAck = packetSequence;
                if (link->lastPacketSent == link->lastPacketAck)
                {
                    printf("Link %d ack'd from packet %d\n",
                        link->id, link->lastPacketAck);
                }
            }
        }
    }
}

internal i32 SV_GetPriorityLinkIndexById(
    SVEntityLinkArray* list, i32 id)
{
	for (i32 i = 0; i < list->numLinks; ++i)
	{
		if (list->links[i].id == id)
		{
			return i;
		}
	}
	return -1;
}

internal void SV_AddPriorityLink(
    SVEntityLinkArray* list, i32 id, f32 priority)
{
	// Avoid duplicates
	if (SV_GetPriorityLinkIndexById(list, id) != -1)
	{
		printf("SV priority: id %d already linked!\n", id);
		return;
	}
	
	i32 i = list->numLinks;
	COM_ASSERT(i < list->maxLinks, "No free entity links")
	if (priority < 1)
	{
		priority = 1;
	}
	list->numLinks++;
    list->links[i] = {};
	list->links[i].id = id;
	list->links[i].priority = priority;
    list->links[i].basePriority = priority;
}

internal void SV_SwapEntityLinks(
    SVEntityLink* a,
    SVEntityLink* b)
{
    SVEntityLink temp;
    temp = *a;
    *a = *b;
    *b = temp;
	
}

internal void SV_RemovePriorityLinkByIndex(
    SVEntityLinkArray* list, i32 index)
{
	if (index == -1) { return; }
	i32 last = list->numLinks - 1;
	list->links[index] = {};
	SV_SwapEntityLinks(&list->links[index], &list->links[last]);
	list->numLinks -= 1;
}

internal void SV_FlagLinkAsDead(SVEntityLinkArray* list, i32 id)
{
    for (i32 i = 0; i < list->numLinks; ++i)
    {
        if (list->links[i].id == id)
        {
            list->links[i].status = ENT_LINK_STATUS_DEAD;
        }
    }
}

internal void SV_RemovePriorityLinkBySerial(
    SVEntityLinkArray* list, i32 id)
{
    for (i32 i = 0; i < list->numLinks; ++i)
    {
        if (list->links[i].id == id)
        {
            SV_RemovePriorityLinkByIndex(list, i);
        }
    }
}

internal i32 SV_CompareLink(SVEntityLink* a, SVEntityLink* b)
{
    if (a->importance < b->importance) { return -1; }
    if (a->importance > b->importance) { return 1; }
    return 0;
}

/**
 * The ultimate sorting algorithm
 */
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
            if (SV_CompareLink(a, b) < 0)
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
internal void SV_TickPriorityQueue(
    SVEntityLinkArray* list)
{
    for (i32 i = list->numLinks - 1; i >= 0; --i)
    {
        SVEntityLink* link = &list->links[i];
        link->importance += link->priority;
        if (link->status == ENT_LINK_STATUS_DEAD &&
            link->lastPacketAck >= link->lastPacketSent)
        {
            SV_RemovePriorityLinkByIndex(list, i);
        }
    }
	SV_BubbleSortPriorityQueue(list->links, list->numLinks);
}
