#pragma once

#include "../common/com_module.h"
/*
=== Entity Component interface ===
Add
Remove
Find
Has
Update

Create Memory
Assign Memory
Clear Memory

*/

typedef void (*EC_Update)(void* gs, GameTime* time);

struct EntityComponent
{
    i32 flag;
    EC_Update updateFunction;
};
