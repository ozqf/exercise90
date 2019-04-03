#pragma once

#include "sim_internal.h"

/*
Results array MUST have the capacity of items specified in the def
*/
internal void Sim_CreateSpawnPattern(
	: : : :SimProjectileSpawnDef* event, // Don't like this. Shouldn't be specific to projectiles!
	SimSpawnPatternDef* def,
	SimSpawnPatternItem* results,
	i32 firstSerial)
{
	
}
