#pragma once

#include "game.h"
#include "../common/com_module.h"

// true if a 'tock' was triggered
inline u8 Game_TockThinker(EC_Thinker* thinker, f32 deltaTime)
{
    if (thinker->ticker.tick <= 0)
    {
        thinker->ticker.tick = thinker->ticker.tickMax;
        return 1;
    }
    else
    {
        thinker->ticker.tick -= deltaTime;
        return 0;
    }
}

void Game_UpdateThinkers(GameState* gs, GameTime* time)
{
    for (u32 i = 0; i < gs->thinkerList.max; ++i)
    {
        EC_Thinker* thinker = &gs->thinkerList.items[i];
        if (thinker->header.inUse == 0) { continue; }

        switch (thinker->brainType)
        {
            case EC_BRAIN_SPAWNER:
            {
                if (!Game_TockThinker(thinker, time->deltaTime)) { continue; }

                Cmd_EntityState cmd = {};
		        cmd.entityId = Ent_ReserveFreeEntity(&gs->entList);
                cmd.factoryType = ENTITY_TYPE_RIGIDBODY_CUBE;
                cmd.pos = Game_RandomSpawnOffset(10, 0, 10);
                printf("Spawn test: %d/%d\n", cmd.entityId.iteration, cmd.entityId.index);
                //cmd.pos.y += 10;
            
                //App_WriteGameCmd((u8*)&cmd, CMD_TYPE_ENTITY_STATE, sizeof(Cmd_EntityState));
                APP_WRITE_CMD(0, CMD_TYPE_ENTITY_STATE, 0, cmd);
            } break;
        }
    }
}
