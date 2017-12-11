#ifndef GAME_CPP
#define GAME_CPP

#include "../Shared/shared.h"

struct Transform
{
    f32 pos[3];
    f32 rot[3];
    f32 scale[3];
};

struct Entity
{
    i32 id;
    Transform transform;
    i32 type;
};

struct CompTrigger
{
    i32 id;
    char name[16];
};


#endif