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
u32 Game_BuildRenderList(World* world, RenderScene* scene)
{
    u32 count = 0;
    for (u32 i = 0; i < world->rendererList.max; ++i)
    {
        EC_Renderer* rend = &world->rendererList.items[i];
        if (rend->inUse == 1)
        {
            Ent* ent = Ent_GetEntityByIndex(rend->entId.index);
            RenderListItem* item = &scene->sceneItems[count];
            item->transform = ent->transform;
            item->obj = rend->rendObj;
            count++;
        }
    }
    return count;
    #if 0
    int j = 0;
    for (u32 i = 0; i < R_MAX_RENDER_OBJECTS; ++i)
    {
        RendObj* obj = &g_game_rendObjects[i];
		if (obj->inUse == 1)
		{
			Ent* e = Ent_GetEntityByIndex(obj->ownerIndex);
			RenderListItem* item = &scene->sceneItems[i];
			item->transform = e->transform;
			item->obj = *obj;
			scene->numObjects++;
		}
    }
    #endif
}

