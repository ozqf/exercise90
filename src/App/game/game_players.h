#pragma once

#include "game.h"

#define PLAYER_STATUS_FREE 0
#define PLAYER_STATUS_ASSIGNED 1
//#define PLAYER_STATUS_RESERVED 1
//#define PLAYER_STATUS_OBSERVING 2
//#define PLAYER_STATUS_PLAYING 3

Player* SV_FindPlayerByClientId(PlayerList* plyrs, i32 clientId)
{
    i32 l = plyrs->max;
    for (i32 i = 0; i < l; ++i)
    {
        if (plyrs->items[i].sv_clientId == clientId)
        {
            return &plyrs->items[i];
        }
    }
    ILLEGAL_CODE_PATH
    return NULL;
}

// Create a new player, assigning the given client Id and filling out
// a state update
// returns true if a player was created
u8 SV_CreateNewPlayer(PlayerList* plyrs, i32 clientId, Cmd_PlayerState* result)
{
    Assert(plyrs != NULL);
    Assert(result != NULL);

    Player* plyr = SV_FindPlayerByClientId(plyrs, clientId);
    if (plyr == NULL)
    {
        i32 l = plyrs->max;
        for (i32 i = 0; i < l; ++i)
        {
            if (plyrs->items[i].state != PLAYER_STATUS_FREE) { continue; }

            plyr->playerId = i;
            plyr = &plyrs->items[i];
            plyr->state = PLAYER_STATUS_ASSIGNED;
            plyr->sv_clientId = clientId;

            result->playerId = i;
            result->state = PLAYER_STATUS_ASSIGNED;
        }
    }
    // erm?
    ILLEGAL_CODE_PATH
    return 0;
}

Player* Game_FindPlayerById(PlayerList* plyrs, i32 id)
{
    i32 l = plyrs->max;
    for (i32 i = 0; i < l; ++i)
    {
        if (plyrs->items[i].state == PLAYER_STATUS_FREE) { continue; }
        if (plyrs->items[i].playerId == id) { return &plyrs->items[i]; }
    }
    ILLEGAL_CODE_PATH
    return NULL;
}

void Exec_PlayerState(GameState* gs, Cmd_PlayerState* cmd)
{
    printf("GAME Exec player state\n");
    
}
