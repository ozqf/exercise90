#pragma once

#include "../common/com_module.h"

#include "sim_types.h"

i32 Sim_AddEntity();
void Sim_GetEntityList(Sim_Entity** ptr, i32* max);
