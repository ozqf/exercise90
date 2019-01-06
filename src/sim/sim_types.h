#pragma once

#include "../common/com_module.h"

#define SIM_ENT_STATUS_FREE 0
#define SIM_ENT_STATUS_RESERVED 1
#define SIM_ENT_STATUS_IN_USE 2

struct Sim_EntityId;

struct Sim_Entity
{
    i32 status;
    i32 id;
    Transform t;
};
