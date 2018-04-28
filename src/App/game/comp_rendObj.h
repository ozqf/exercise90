#pragma once

#include "game.h"

/**
 * Load relevant entity renderers into rend object list
 */
inline void Game_BuildRenderList(GameState* gs, RenderScene* scene)
{
    scene->cameraTransform = gs->cameraTransform;
    
    for (u32 i = 0; i < gs->rendererList.max; ++i)
    {
        EC_Renderer* rend = &gs->rendererList.items[i];
        if (rend->inUse == 1)
        {
            Ent* ent = Ent_GetEntityByIndex(&gs->entList, rend->entId.index);
            RScene_AddRenderItem(scene, &ent->transform, &rend->rendObj);
        }
    }
}

