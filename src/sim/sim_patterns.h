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
	f32 radians = 0;
	i32 randomIndex = event->seedIndex;
	i32 i = 0;
	if (def->numItems % 2 != 0)
	{
		// Odd number spread, one pellet goes dead centre
		items[i].pos.x = event->pos.x;
		items[i].pos.y = event->pos.y;
		items[i].pos.z = event->pos.z;
		items[i].forward.x = event->forward.x;
		items[i].forward.y = 0;
		items[i].forward.z = event->forward.z;
		items[i].entSerial = serial;
		serial += serialIncrement;
		i++;
	}
	f32 forwardRadians = 0;//atan2f(event->forward.z, event->forward.x);
	// step 1 / (5 + 1) = 
	f32 step = def->radius / (def->numItems + 1);
	radians = forwardRadians - (def->radius / 2);
	for(; i < def->numItems; ++i)
	{
		items[i].pos.x = event->pos.x;
		items[i].pos.y = event->pos.y;
		items[i].pos.z = event->pos.z;
		items[i].forward.x = cosf(radians);
		items[i].forward.y = 0;
		items[i].forward.z = sinf(radians);
		items[i].entSerial = serial;

		serial += serialIncrement;
		radians += step;
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
