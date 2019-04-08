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
	switch (def->patternId)
	{
		case SIM_PATTERN_RADIAL:
		{
			//
			return Sim_CreateRadialPattern(event, def, results,
				firstSerial, isLocal);
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
			//
		} break;
	}
	return 0;
}
