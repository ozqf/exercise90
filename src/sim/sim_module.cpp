#pragma once

#include "../common/common.h"

#include "sim.h"

#include "sim_internal.h"

#include "sim_patterns.h"
#include "sim_collision.h"
#include "sim_projectiles.h"
#include "sim_entity_init.h"
#include "sim_enemy_defs.h"
#include "sim_factory.h"
#include "sim_external.h"
#include "sim_ticks.h"

// TODO: Uncouple me!
// This should be so that the sim module can access 
// the platform performance counter
#include "../app/app.h"
