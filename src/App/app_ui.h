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
    scene->settings.projectionMode = 1;
    scene->settings.orthographicHalfHeight = 8;
    for (i32 i = 0; i < maxEntities; ++i)
    {
        UIEntity* ent = &ents[i];
        if (ent->inUse == 1)
        {
            RScene_AddRenderItem(scene, &ent->transform, &ent->rendObj);
			//Transform t = ent->transform;
            Transform t;
            Transform_SetToIdentity(&t);
            t.pos.z = 0.5f;
			t.scale.x = 1000;//0.5f;//ent->halfWidth;
			t.scale.y = 1000;//0.5f;//ent->halfHeight;
			t.scale.z = 1000;
			RScene_AddRenderItem(scene, &t, &ent->debugRend);
        }
    }
}
