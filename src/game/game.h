#ifndef GAME_H
#define GAME_H

#include "../shared_types.h"

struct IGame
{
    void *FixedUpdate(AppTime time);
};

#endif