#pragma once

#include "game.h"

Ent* Game_CreateEntity()
{
    // Find a free slot in the entity list and return it with a new id assigned
    return NULL;
}

Ent* Game_FindEntityById(i32 id)
{
    for (i32 i = 0; i < g_num_entities; ++i)
    {
        if (g_entities[i].id == id)
        {
            return &g_entities[i];
        }
    }
    return NULL;
}

void Game_BuildRenderList(RenderScene* scene)
{
    
}
