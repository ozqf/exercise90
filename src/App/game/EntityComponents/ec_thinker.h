#pragma once

#include "game.h"
#include "../common/com_module.h"

// true if a 'tock' was triggered
inline u8 Game_TockThinker(EC_Thinker* thinker, f32 deltaTime)
{
    #if 1
    if (thinker->state.ticker.tick <= 0)
    {
        thinker->state.ticker.tick = thinker->state.ticker.tickMax;
        return 1;
    }
    else
    {
        thinker->state.ticker.tick -= deltaTime;
        return 0;
    }
    #endif
}

void Game_UpdateThinkers(GameState* gs, GameTime* time)
{
    #if 1
    for (u32 i = 0; i < gs->thinkerList.max; ++i)
    {
        EC_Thinker* thinker = &gs->thinkerList.items[i];
        if (thinker->header.inUse == 0) { continue; }

        switch (thinker->state.type)
        {
            case EC_THINKER_SPAWNER:
            {
                if (!Game_TockThinker(thinker, time->deltaTime)) { continue; }
                if (thinker->state.count >= thinker->state.max) { continue; }
                // Stop spawning if no players are around
                EntId id = AI_FindNearestPlayer(gs, { 0, 0, 0 });
                if (id.value == 0)
                {
                    return;
                }
                
                EntitySpawnOptions options = {};
                options.source = thinker->header.entId;
                EC_Transform* ect = EC_FindTransform(gs, &thinker->header.entId);
                options.pos = ect->t.pos;
                printf("SPAWN enemy at %.2f, %.2f, %.2f\n",
                    options.pos.x, options.pos.y, options.pos.z
                
                );
                thinker->state.count++;
                Ent_WriteSpawnCmd(gs, ENTITY_TYPE6_ENEMY, &options);
            } break;
        }
    }
    #endif
}
