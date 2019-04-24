#pragma once

#include "sim_internal.h"

internal i32 Sim_CreateRadialPattern(
	SimSpawnBase* event,
	SimSpawnPatternDef* def,
	SimSpawnPatternItem* items,
	i32 serial,
	i32 isLocal)
{
	i32 serialIncrement = isLocal ? -1 : 1;
	f32 radians = 0;
	f32 step = FULL_ROTATION_RADIANS / (f32)def->numItems;
	for(i32 i = 0; i < def->numItems; ++i)
	{
		// TODO 2D! Once new system is implemented make this work in 3D
		Vec3 dir =
		{
			cosf(radians),
			0,
			sinf(radians)
		};
		items[i].forward.x = dir.x;
		items[i].forward.y = dir.y;
		items[i].forward.z = dir.z;
		items[i].pos.x = event->pos.x + (dir.x * def->radius);
		items[i].pos.y = event->pos.y + (dir.y * def->radius);
		items[i].pos.z = event->pos.z + (dir.z * def->radius);
		items[i].entSerial = serial;

		radians += step;
		serial += serialIncrement;
	}
	return def->numItems;
}

internal i32 Sim_CreateScatterPattern(
	SimSpawnBase* event,
	SimSpawnPatternDef* def,
	SimSpawnPatternItem* items,
	i32 serial,
	i32 isLocal)
{
	i32 serialIncrement = isLocal ? -1 : 1;
	f32 radians = 0;
	i32 randomIndex = event->seedIndex;
	for(i32 i = 0; i < def->numItems; ++i)
	{
		radians = COM_Randf32(randomIndex++) * (2 * pi32);
		// TODO 2D! Once new system is implemented make this work in 3D
		Vec3 dir =
		{
			cosf(radians),
			0,
			sinf(radians)
		};
		items[i].forward.x = dir.x;
		items[i].forward.y = dir.y;
		items[i].forward.z = dir.z;
		items[i].pos.x = event->pos.x + (dir.x * def->radius);
		items[i].pos.y = event->pos.y + (dir.y * def->radius);
		items[i].pos.z = event->pos.z + (dir.z * def->radius);
		items[i].entSerial = serial;

		serial += serialIncrement;
	}
	return def->numItems;
}

internal i32 Sim_CreateSpreadPattern(
	SimSpawnBase* event,
	SimSpawnPatternDef* def,
	SimSpawnPatternItem* items,
	i32 serial,
	i32 isLocal)
{
	i32 serialIncrement = isLocal ? -1 : 1;
	f32 forwardRadians = atan2f(event->forward.z, event->forward.x);
	
	if (def->numItems == 1)
	{
		// just launch one straight forward...
		items[0].forward.x = event->forward.x;
		items[0].forward.y = event->forward.y;
		items[0].forward.z = event->forward.z;
		items[0].pos.x = event->pos.x + (event->forward.x * def->radius);
		items[0].pos.y = event->pos.y + (event->forward.y * def->radius);
		items[0].pos.z = event->pos.z + (event->forward.z * def->radius);
		items[0].entSerial = serial;
		return 1;
	}

    f32 arc = def->arc;
    
	// -1 items here to cover the full sweep. Otherwise the last angle
	// is missed off
    f32 step = arc / (def->numItems - 1);
    f32 radians = forwardRadians - (arc / 2.0f);
    for (i32 i = 0; i < def->numItems; ++i)
    {
		Vec3 dir =
		{
			cosf(radians),
			0,
			sinf(radians)
		};
        items[i].forward = dir;
		items[i].pos.x = event->pos.x + (dir.x * def->radius);
		items[i].pos.y = event->pos.y + (dir.y * def->radius);
		items[i].pos.z = event->pos.z + (dir.z * def->radius);
		items[i].entSerial = serial;

        radians += step;
		serial += serialIncrement;
    }
	return def->numItems;
}

/*
Returns number of results
Results array MUST have the capacity of items specified in the def
*/
internal i32 Sim_CreateSpawnPattern(
	SimSpawnBase* event,
	SimSpawnPatternDef* def,
	SimSpawnPatternItem* results,
	i32 firstSerial,
	i32 isLocal)
{
	COM_ASSERT(def->numItems > 0, "Pattern has zero items")
	switch (def->patternId)
	{
		case SIM_PATTERN_RADIAL:
		{
			//
			return Sim_CreateRadialPattern(
				event, def, results, firstSerial, isLocal);
		} break;

		case SIM_PATTERN_SCATTER:
		{
			return Sim_CreateScatterPattern(
				event, def, results, firstSerial, isLocal);
		} break;

		case SIM_PATTERN_SPREAD:
		{
			return Sim_CreateSpreadPattern(
				event, def, results, firstSerial, isLocal);
		} break;
		
		case SIM_PATTERN_NONE:
		{
			results[0].pos = event->pos;
			results[0].forward = event->forward;
			results[0].entSerial = event->firstSerial;
			return 1;
		} break;

		default:
		{
			COM_ASSERT(0, "Unknown Pattern type")
		} break;
	}
	return 0;
}
