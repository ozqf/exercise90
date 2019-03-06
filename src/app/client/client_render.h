#pragma once

#include "client.h"


void CL_PopulateRenderScene(
	Transform* cam,
	RenderScene* scene,
	i32 maxObjects,
	i32 texIndex,
	f32 interpolateTime)
{
    Transform t;
    Transform_SetToIdentity(&t);
    RendObj obj = {};
    MeshData* cube = COM_GetCubeMesh();
	
	
    // Hit test
    #if 0
	i32 mouseScrX = Input_GetActionValue(
		&g_inputActions, "Mouse Pos X");
	i32 mouseScrY = Input_GetActionValue(
		&g_inputActions, "Mouse Pos Y");
	ScreenInfo scr = App_GetScreenInfo();
	
	// screen pos in 0 to 1 range
	f32 percentX = (f32)mouseScrX / (f32)scr.width;
	f32 percentY = (f32)mouseScrY / (f32)scr.height;
	
	// flip y axis
	percentY = (1 - percentY);
	
	// convert to -1 to +1 range
	percentX = (percentX * 2.0f) - 1.0f;
	percentY = (percentY * 2.0f) - 1.0f;
	
	//printf("Mouse scr Percent %.3f, %.3f\n",
	//	percentX, percentY);
	
    Vec3 forward = Transform_GetForward(cam);
    //forward.x *= 0.5f;
    //forward.y *= 0.5f;
    //forward.z *= 0.5f;
	
    Vec3 left = Transform_GetLeft(cam);
    left.x *= (percentX * scr.aspectRatio);
    left.y *= (percentX * scr.aspectRatio);
    left.z *= (percentX * scr.aspectRatio);

    Vec3 up = Transform_GetUp(cam);
    up.x *= percentY;
    up.y *= percentY;
    up.z *= percentY;

    t.pos = 
    {
        cam->pos.x + forward.x,
        cam->pos.y + forward.y,
        cam->pos.z + forward.z
    };
	t.scale = { 0.2f, 0.2f, 0.2f };
	t.pos.x += left.x;
	t.pos.y += left.y;
	t.pos.z += left.z;
	
	t.pos.x += up.x;
	t.pos.y += up.y;
	t.pos.z += up.z;
    
    Transform t2 = t;
    t2.pos.x += forward.x;
    t2.pos.y += forward.y;
    t2.pos.z += forward.z;

    Transform t3 = t2;
    t3.pos.x += forward.x;
    t3.pos.y += forward.y;
    t3.pos.z += forward.z;
    f32 m[16];
    M4x4_SetToIdentity(m);
    COM_SetupDefault3DProjection(m, scr.aspectRatio);
    t3.pos = Vec3_MultiplyByM4x4(&t3.pos, m);
    g_matrix = *(M4x4*)m;
	
    RendObj_SetAsMesh(
		&obj, *cube, 1, 0, 1, texIndex);
    //t.pos = g_testHitPos;

    RScene_AddRenderItem(scene, &t, &obj);
    RScene_AddRenderItem(scene, &t2, &obj);
    RScene_AddRenderItem(scene, &t3, &obj);
    #endif
    //////////////////////////////////////////////////////////////////////
    // Sim Entities
    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        if (scene->numObjects >= scene->maxObjects) { break; }

        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        
		switch (ent->entType)
		{
			case SIM_ENT_TYPE_WORLD:
			{
				RendObj_SetAsMesh(
					&obj, *cube, 0.2f, 0.2f, 0.2f, texIndex);
			} break;

            case SIM_ENT_TYPE_PROJECTILE:
			{
				RendObj_SetAsMesh(
					&obj, *cube, 1, 1, 0, texIndex);
			} break;
			
			case SIM_ENT_TYPE_ACTOR:
			{
				RendObj_SetAsMesh(
					&obj, *cube, 0.2f, 1, 0.2f, texIndex);
			} break;

            case SIM_ENT_TYPE_LINE_TRACE:
            {
                Vec3* a = &ent->t.pos;
                Vec3* b = &ent->destination;
                // move ray up slightly out of the floor
                RendObj_SetAsLine(&obj,
                    a->x, a->y + 0.2f, a->z, b->x, b->y + 0.2f, b->z,
                    1, 0, 1, 1, 0, 1
                );
                Transform_SetToIdentity(&t);
                RScene_AddRenderItem(scene, &t, &obj);
            } break;
			
			default:
			{
				RendObj_SetAsMesh(
					&obj, *cube, 0.3f, 0.3f, 1, texIndex);
			} break;
		}
        
        t = ent->t;
        if (g_interpolateRenderScene)
        {
            RendObj_InterpolatePosition(
                &t.pos,
                &ent->previousPos,
                &ent->t.pos,
                interpolateTime);
        }
        else
        {
            t.pos = ent->t.pos;
        }
        RScene_AddRenderItem(scene, &t, &obj);
    }
}

void CL_CopyCameraTransform(Transform* target)
{
	Transform_SetToIdentity(target);
    target->pos.z = 18;
    target->pos.y = 12;
    Transform_SetRotation(
        target,
        -(45 * DEG2RAD),
        0,
        0
    );
}

void CL_GetRenderCommands(
	RenderCommand** cmds,
	i32* numCommands,
	i32 texIndex,
	f32 interpolateTime)
{
    #if 0
    *cmds = g_renderCommands;
    *numCommands = 0;

    i32 nextCommand = 0;

    RenderCommand* cmd;
    ScreenInfo info = App_GetScreenInfo();

    // setup scene
    cmd = &g_renderCommands[nextCommand++];
    cmd->type = REND_CMD_TYPE_SETTINGS;
    RenderSceneSettings* s = &cmd->settings;
    s->fov = 90;
    s->projectionMode = RENDER_PROJECTION_MODE_3D;
    s->orthographicHalfHeight = 8;
    
	CL_CopyCameraTransform(&cmd->settings.cameraTransform);

    // setup projection - must be done after settings
    cmd = &g_renderCommands[nextCommand++];
    cmd->type = REND_CMD_TYPE_PROJECTION;
    COM_SetupDefault3DProjection(cmd->projection.cells, 1);//info.aspectRatio);
	
    /*Transform_SetToIdentity(&cmd->settings.cameraTransform);
    s->cameraTransform.pos.z = 18;
    s->cameraTransform.pos.y = 12;
    Transform_SetRotation(
        &cmd->settings.cameraTransform,
        -(45 * DEG2RAD),
        0,
        0
    );*/
	
	// Set viewport
	cmd = &g_renderCommands[nextCommand++];
	cmd->type = REND_CMD_TYPE_SET_VIEWPORT;
	cmd->viewPort.viewPortX = 0;//info.width / 2;
	cmd->viewPort.viewPortY = 0;
	cmd->viewPort.viewPortWidth = info.width;// / 2;
	cmd->viewPort.viewPortHeight = info.height;
	
    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        if (nextCommand == CL_MAX_RENDER_COMMANDS)
        {
            break;
        }
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        
        RendObj obj = {};
        //MeshData* cube = COM_GetCubeMesh();
        //RendObj_SetAsMesh(
        //    &obj, *cube, 0.3f, 0.3f, 1, texIndex);
		
		RendObj_SetAsAABB(
			&obj, 1, 1, 1, 0, 1, 0);

        Transform t = ent->t;
        RendObj_InterpolatePosition(
                &t.pos,
                &ent->previousPos,
                &ent->t.pos,
                interpolateTime);
        
        // Add to command list
        cmd = &g_renderCommands[nextCommand++];
        cmd->type = REND_CMD_TYPE_DRAW;
        cmd->drawItem.obj = obj;
        cmd->drawItem.transform = t;
    }

    *numCommands = nextCommand;
    #endif
}

