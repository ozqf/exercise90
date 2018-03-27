#pragma once

#include "game.h"

void Game_UpdateAI(GameTime* time)
{
    for (i32 i = 0; i < g_num_brains; ++i)
    {
        EntComp_TestBrain* brain = &g_game_brains[i];
        Ent* ent = Game_GetEntityByIndex(brain->entId.index);
        Assert(ent != NULL);
        ent->transform.rot.y += 90 * time->deltaTime;
    }
}
