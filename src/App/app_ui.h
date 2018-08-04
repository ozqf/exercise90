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
			//Transform_SetToIdentity(&ent->transform);
            //ent->transform.scale.x = 4;
            //ent->transform.scale.y = 4;
            //ent->transform.scale.z = 4;
            //Transform_SetRotationDegrees(&ent->transform, 0, 180, 0);
            RScene_AddRenderItem(scene, &ent->transform, &ent->rendObj);
            #if 1
			Transform t = ent->transform;
            t.scale.x = ent->halfWidth;
            t.scale.y = ent->halfHeight;
            t.pos.z += 4;
            RScene_AddRenderItem(scene, &t, &ent->debugRend);
            #endif
            #if 0
            Transform t;
            Transform_SetToIdentity(&t);
            Transform_SetRotationDegrees(&t, 0, 180, 0);
            t.pos.x = 4;
            t.pos.y = 0;
            t.pos.z = -2;//0.5f;
			t.scale.x = 4;//1000;//0.5f;//ent->halfWidth;
			t.scale.y = 4;//1000;//0.5f;//ent->halfHeight;
			t.scale.z = 4;//1000;
			RScene_AddRenderItem(scene, &t, &ent->debugRend);
            #endif
        }
    }
}
