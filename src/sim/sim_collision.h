#pragma once

#include "sim_internal.h"

/**
 * Returns number of overlapping entities
 */
extern "C"
i32 Sim_FindByAABB(
    SimScene* sim,
    Vec3 boundsMin,
    Vec3 boundsMax,
    i32 ignoreSerial,
    SimEntity** results,
    i32 maxResults
    )
{
    i32 resultIndex = 0;
    i32 count = 0;
    for (i32 i = 0; i < sim->maxEnts; ++i)
    {
        SimEntity* ent = &sim->ents[i];
        if (ent->status != SIM_ENT_STATUS_IN_USE
            || ent->id.serial == ignoreSerial)
        { continue; }
        if (ent->entType != SIM_ENT_TYPE_WANDERER)
        { continue; }
        // expand bounds by entity size and
        // then point test
        Vec3 min;
        min.x = boundsMin.x - 0.5f;
        min.y = boundsMin.y - 0.5f;
        min.z = boundsMin.z - 0.5f;
        Vec3 max;
        max.x = boundsMax.x + 0.5f;
        max.y = boundsMax.y + 0.5f;
        max.z = boundsMax.z + 0.5f;
        Vec3* p = &ent->t.pos;
        if (p->x < min.x || p->x > max.x) { continue; }
        if (p->y < min.y || p->y > max.y) { continue; }
        if (p->z < min.z || p->z > max.z) { continue; }
        count++;
        if (results)
        {
            results[resultIndex] = ent;
            if (count >= maxResults)
            {
                break;
            }
        }
    }
    return count;
}
