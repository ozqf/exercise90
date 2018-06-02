#pragma once

#include "../../common/com_module.h"

#if 0
void Spawn_RigidbodyCube(GameState* gs, GCmd_Spawn* cmd)
{
    Ent* ent = Ent_GetFreeEntity(&gs->entList);

    Transform_SetPosition(&ent->transform, cmd->pos.x, cmd->pos.y, cmd->pos.z);
    
    Transform_SetScale(&ent->transform, 1, 1, 1);
    Transform_SetRotationDegrees(&ent->transform, 0, 0, 0);
    
    //controller = EC_AddAIController(ent, gs);
    //Ent_InitAIController(controller, 1, 0, 0, 5);

    EC_Renderer* renderer = EC_AddRenderer(ent, gs);
	RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, 5);

    f32 size = 1.0f;
    EC_Collider* collider = EC_AddCollider(ent, gs);
    collider->size = { size, size, size };
    collider->shapeId = Phys_CreateBox(
        ent->transform.pos.x,
        ent->transform.pos.y,
        ent->transform.pos.z,
        size / 2, size / 2, size / 2,
        0,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEBRIS,
        ent->entId.index,
        ent->entId.iteration);
}
#endif

// void Cmd_Spawn(GameState* gs, GCmd_Spawn* cmd)
// {
//     switch (cmd->entityType)
//     {
//         case ENTITY_TYPE_WORLD_CUBE:
//         {

//         } break;
    
//         case ENTITY_TYPE_RIGIDBODY_CUBE:
//         {
//             Spawn_RigidbodyCube(gs, cmd);
//         } break;

//         default:
//         {
//             ILLEGAL_CODE_PATH
//         } break;
//     }
// }
