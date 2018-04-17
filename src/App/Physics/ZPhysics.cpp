#pragma once

#include "../../Shared/shared_types.h"

// header, contains data structures and function definitions
#include "ZPhysics_interface.h"
/**
 * !NO BULLET PHYSICS LIBRARY ABOVE THIS POINT!
 * 
 */
#include "../../../lib/bullet/btBulletCollisionCommon.h"
#include "../../../lib/bullet/btBulletDynamicsCommon.h"

// For debug output ONLY
#include <windows.h>

// Internal data structures used to interact with Bullet Physics
#include "ZPhysics_internal_types.h"

// Global variables used by the rest of the internal system
#include "ZPhysics_globals.h"

// Implement public interface
#include "ZPhysics_external_functions.cpp"

// Internal functions



