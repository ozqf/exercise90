#pragma once

#include "app_module.cpp"

UIEntity* UI_GetFreeEntity(UIEntity* list, i32 max)
{
    for (i32 i = 0; i < max; ++i)
    {
        if (list[i].inUse == 0)
        {
            list[i] = {};
            list[i].inUse = 1;
            list[i].id = i;
            return &list[i];
        }
    }
    return NULL;
}

void UI_BuildUIRenderScene(RenderScene* scene, UIEntity* ents, i32 maxEntities)
{
    for (i32 i = 0; i < maxEntities; ++i)
    {
        UIEntity* ent = &ents[i];
        if (ent->inUse == 1)
        {
            RScene_AddRenderItem(scene, &ent->transform, &ent->rendObj);
			Transform t = ent->transform;
			t.scale.x = ent->width;
			t.scale.y = ent->height;
			t.scale.z = 1;
			RScene_AddRenderItem(scene, &t, &ent->debugRend);
        }
    }
}
