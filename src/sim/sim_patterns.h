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
		//
		items[i].pos.x = event->pos.x + cosf(radians) * def->radius;
		items[i].pos.z = event->pos.z + sinf(radians) * def->radius;
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
	switch (def->pattern)
	{
		case SIM_PROJECTILE_PATTERN_RADIAL:
		{
			//
			return Sim_CreateRadialPattern(event, def, results,
				firstSerial, isLocal);
		} break;
		
		case SIM_PROJ_TYPE_NONE:
		{
			//
		} break;

		default:
		{
			//
		} break;
	}
	return 0;
}