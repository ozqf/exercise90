#pragma once

/**
 * Iterate over a set of entity links, calculating their
 * priority to the given avatar
 */
#include "../../sim/sim.h"
#include "../priority_queue.h"

internal void SVP_CalculatePriorities(
    SimScene* sim, SimEntity* subject, SVEntityLink* links, i32 numLinks)
{
    Vec3 pos = subject->body.t.pos;
    f32 closest = ZINFINITY(), furthest = 0;
    // Scan over links, finding nearest and furthest
    // Do in reverse so links can be removed
    for (i32 i = numLinks - 1; i >= 0; --i)
    {
        SVEntityLink* link = &links[i];
        SimEntity* ent = Sim_GetEntityBySerial(sim, link->id);
        COM_ASSERT(ent, "Linked entity is null")
        Vec3 entPos = ent->body.t.pos;
        f32 dist = Vec3_Distance(pos, entPos);
        link->distance = dist;
        if (dist > furthest) { furthest = dist; }
        if (dist < closest) { closest = dist; }
    }

    // Find each link's place in the group
    for (i32 i = numLinks - 1; i >= 0; --i)
    {
        SVEntityLink* link = &links[i];
        SimEntity* ent = Sim_GetEntityBySerial(sim, link->id);
        COM_ASSERT(ent, "Linked entity is null")

        // Calculate range based priority by comparing to distance
        // of other entities.
        f32 dist = link->distance;
        const f32 maxDist = 15;
		f32 f = ((SIM_NET_MAX_PRIORITY - 1) * (1 - (dist / maxDist)));
        #if 1
        if (dist > maxDist)
        {
            link->priority = SIM_NET_MIN_PRIORITY;
			COM_ASSERT(
				link->priority > 0,
				"Priority == 0")
        }
        else
        {
            link->priority = (u8)(1.0f + f);
			if (link->priority == 0)
			{
				link->priority = 1;
			}
			//COM_ASSERT(f > 6, "f <= 6")
        }
        #endif
        #if 0
        if (dist > 10)
        {
            link->priority = SIM_NET_MIN_PRIORITY;
            //link->priority = SIM_NET_MAX_PRIORITY;
            ent->priority = SIM_NET_MIN_PRIORITY;
        }
        else
        {
            link->priority = SIM_NET_MAX_PRIORITY;
            ent->priority = SIM_NET_MAX_PRIORITY;
        }
        #endif
        ent->priority = link->priority;
    }
}
