#pragma once

#include "game.h"

void Game_UpdateAI(GameTime* time)
{
    for (i32 i = 0; i < g_num_brains; ++i)
    {
        EntComp_TestBrain brain = g_game_brains[i];
        Ent* ent = Game_FindEntityById(brain.id);
        Assert(ent != NULL);
        ent->transform.rot.y += 90 * time->deltaTime;
    }
}
