#pragma once
/*
Server only functions
*/
#include "app_module.cpp"

/*
Server construction of a relevance list for a specific client.
> Returns the number of links created
> Ignore entities that are not in use (obviously)
> Ignore entities that are marked as 'static'. These entities are most likely
    world volumes that should never change
> Set priority here too, based on the ent's settings
> Future: Ignore entities based on location/distance/line of sight (not necessary
    whilst the whole game is one arena though!)

*/
i32 SV_BuildRelevanceList(Client* cl, EntityLink* links, i32 maxLinks, EntList* ents)
{
    i32 count = 0;
    for (u32 i = 0; i < ents->count; ++i)
    {
        Ent* e = & ents->items[i];
        if (e->inUse == ENTITY_STATUS_IN_USE)
        {
            links->entId = e->entId;
            links->priority = e->priority;
            links->importance = 0;

            ++links;
            ++count;
        }
    }
    return count;
}

void SV_IterateImportance(EntityLink* links, i32 numLinks)
{

    /*
    Network entity update priority queue:
    > Calculate a priority based on the 
        > entity types' inherent priority
            (projectiles are higher than regular enemies etc)
        > Distance from this client
        > Are they targetting this client directly? This could 
            be used to cancel the distance effect
    > Increase each entity link's importance by it's priority
    > Sort list by descending importance.
    > Iterate over sorted list, writing entity updates until
        packet is full. Reset importance to 0 for each entity
        written
    > Send packet of updates to client
    */
    for (i32 i = 0; i < numLinks; ++i)
    {
        Assert(links->priority > 0);
        links->importance += links->priority;
    }

    // Sort list by importance
}
