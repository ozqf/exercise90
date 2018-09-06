#pragma once

#include "../../common/com_module.h"

#define TEAM_FREE 0
#define TEAM_ENEMIES 1
#define TEAM_PLAYERS 2
#define TEAM_NON_COMBATANT 3

inline u8 Game_AttackIsValid(i32 attackingTeam, i32 defendingTeam)
{
    if (defendingTeam == TEAM_NON_COMBATANT) { return 0; }
    if (attackingTeam == TEAM_FREE || defendingTeam == TEAM_FREE) { return 1; }
    if (attackingTeam == defendingTeam) { return 0; }
    return 1;
}
