#ifndef ZPHYSICS_MODULE_CPP
#define ZPHYSICS_MODULE_CPP

#include <stdio.h>
#include "../../common/com_defines.h"
// header, contains data structures and function definitions
#include "ZPhysics_interface.h"
/**
 * !NO BULLET PHYSICS LIBRARY ABOVE THIS POINT!
 * 
 */
#include "../../../lib/bullet/btBulletCollisionCommon.h"
#include "../../../lib/bullet/btBulletDynamicsCommon.h"

#include "../../../lib/bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
//#include "btGhostObject.h"

// For debug output ONLY
#include <windows.h>

// Internal data structures used to interact with Bullet Physics
//#include "ZPhysics_internal_types.h"
#include "ZPhysics_internal_interface.h"

// Global variables used by the rest of the internal system
#include "ZPhysics_globals.h"

// Implement public interface

#define PHYS_DEFAULT_FRICTION 1.0
#define PHYS_DEFAULT_RESTITUTION 0.0

// Handling public interface
#include "ZPhysics_external_functions.cpp"

#include "ZPhysics_query.h"
#include "ZPhysics_factory.h"
#include "ZPhysics_main.h"

#endif