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
            t.scale.x = ent->halfWidth;
            t.scale.y = ent->halfHeight;
            t.pos.z += 4;
            RScene_AddRenderItem(scene, &t, &ent->debugRend);
        }
    }
    for (i32 i = 0; i < HUD_MAX_RING_ITEMS; ++i)
    {
        HudRingItem* item = &g_hudRingItems[i];
        if (item->state == 1)
        {
            Transform t;
            Transform_SetToIdentity(&t);
            Transform_SetRotationDegrees(&t, 0, 0, item->degrees);
            t.pos.x = item->screenPos.x;
            t.pos.y = item->screenPos.y;
            t.pos.z = item->screenPos.z;
            t.scale.x = 0.1f * item->scale;
            t.scale.y = 0.1f * item->scale;
            t.scale.z = 0.1f * item->scale;
            RScene_AddRenderItem(scene, &t, &item->rendObj);
        }
        
    }
    

    // g_hudRingRend
}
