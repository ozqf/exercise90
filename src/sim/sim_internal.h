#pragma once

#include "sim.h"
// Internal Sim module functions/data

internal SimEntity* Sim_FindEntityBySerialNumber(
    SimScene* scene, i32 serialNumber);
internal i32 Sim_FreeEntityBySerial(SimScene* scene, i32 serial);
internal i32 Sim_ReserveRemoteEntitySerial(SimScene* scene, i32 isLocal);
internal i32 Sim_FindFreeSlot(SimScene* scene, i32 forLocalEnt);
