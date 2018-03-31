#pragma once

#include "game.h"

/**
 * Load relevant entity renderers into rend object list
 * Returns number of items added to the list
 * 
 * requires:
 * > list of renderer components (GameState)
 * > list of entities they are attached to (GameState)
 * > list of RenderListItems to add to (RenderScene)
 * > maximum number of render items (RenderScene)
 * 
 */
inline void Game_BuildRenderList(GameState* gs, RenderScene* scene)
{
    for (u32 i = 0; i < gs->rendererList.max; ++i)
    {
        EC_Renderer* rend = &gs->rendererList.items[i];
        if (rend->inUse == 1)
        {
            Ent* ent = Ent_GetEntityByIndex(rend->entId.index);
            RenderListItem* item = &scene->sceneItems[scene->numObjects];
            scene->numObjects++;
            item->transform = ent->transform;
            item->obj = rend->rendObj;
        }
    }
}

