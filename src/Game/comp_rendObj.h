#pragma once

#include "game.h"

RendObj* Ent_AddRenderObj(Ent* ent)
{
    RendObj* obj = NULL;
    for (u32 i = 0; i < R_MAX_RENDER_OBJECTS; ++i)
    {
        obj = &g_game_rendObjects[i];
        if (obj->inUse == 0)
        {
            obj->inUse = 1;
            obj->ownerIndex = ent->entId.index;
            ent->componentFlags |= ECOMP_FLAG_RENDOBJ;
            return obj;
        }
    }
    Assert(obj != NULL);
    return NULL;
}

void Ent_RemoveRenderObj(Ent* ent, RendObj* obj)
{
    Assert(ent != NULL);
    Assert(obj != NULL);
    Assert(ent->entId.index == obj->ownerIndex);
    ent->componentFlags &= ~ECOMP_FLAG_RENDOBJ;
    obj->inUse = 0;
}

void Game_BuildRenderList(RenderScene* scene)
{
    scene->numObjects = 0;
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
}
