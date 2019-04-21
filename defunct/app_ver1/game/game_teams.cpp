#pragma once

#include "../../common/common.h"

u8 Game_AttackIsValid(i32 attackingTeam, i32 defendingTeam)
{
    if (defendingTeam == TEAM_NON_COMBATANT) { return 0; }
    if (attackingTeam == TEAM_FREE || defendingTeam == TEAM_FREE) { return 1; }
    if (attackingTeam == defendingTeam) { return 0; }
    return 1;
}
