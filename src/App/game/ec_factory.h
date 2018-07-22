#pragma once

#include "game.h"

// struct Ent_UpdateHeader
// {
//     EntId entId;
//     u32 componentBits;
// };

#if 0
u32 EC_AIController_ApplyState(GameState* gs, Ent* ent, u8* ptr)
{
    u8* origin = ptr;
    EC_AIController update;
    COM_COPY_STRUCT(ptr, &update, EC_AIController);

    EC_AIController ai = EC_FindAIController(gs, entId);
    
    return 0;
}
#endif

u32 EC_Transform_ApplyState(GameState* gs, Ent* ent, u8* stream)
{
    u8* origin = stream;
    Transform t = {};
    stream += COM_COPY_STRUCT(stream, &t, Transform);
    printf("  Read transform pos %.2f, %.2f, %.2f\n",
        t.pos.x,
        t.pos.y,
        t.pos.z
    );
    return (stream - origin);
}

u32 EC_Renderer_ApplyState(GameState* gs, Ent* ent, u8* stream)
{
    u8* origin = stream;
    EC_RendererState state = {};
    stream += COM_COPY_STRUCT(stream, &state, EC_RendererState);
    printf("Read Renderer State\n  Mesh: %s\n  Texture: %s\n",
        state.meshName,
        state.textureName
    );

    EC_Renderer* r = EC_FindRenderer(gs, ent);
    if (r == NULL)
    {
        r = EC_AddRenderer(gs, ent);
    }
    RendObj_SetAsMesh(
        &r->rendObj,
        Assets_GetMeshDataByName(state.meshName),// g_meshCube,
        1, 1, 1,
        AppGetTextureIndexByName(state.textureName)
    );

    return (stream - origin);
}

u32 EC_Collider_ReadState(GameState* gs, Ent* ent, u8* stream)
{
    u8* origin = stream;
    EC_ColliderState state = {};
    stream += COM_COPY_STRUCT(stream, &state, EC_ColliderState);
    EC_ColliderApplyState(gs, ent, &state);
    return (stream - origin);
}

#define EC_UPDATE_CALL(gameState, flag, func) \
{ \
    if (componentBits & flag##) \
    { \
        stream += func##(##gameState##, ent, stream); \
    } \
} 

u32 Ent_ApplyState(GameState* gs, u8* stream, u32 numBytes)
{
    u8* origin = stream;
    Cmd_EntityStateHeader h = {};
    stream += COM_COPY_STRUCT(stream, &h, Cmd_EntityStateHeader);
    printf("Ent state for ent %d/%d, comp bits %d\n",
        h.entId.iteration,
        h.entId.index,
        h.componentBits
    );
    Ent* ent = Ent_GetEntityById(&gs->entList, &h.entId);
    if (ent == NULL)
    {
        // create!
        ent = Ent_GetAndAssign(&gs->entList, &h.entId);
    }
    u32 componentBits = h.componentBits;
    EC_UPDATE_CALL(gs, EC_FLAG_TRANSFORM, EC_Transform_ApplyState)
    EC_UPDATE_CALL(gs, EC_FLAG_RENDERER, EC_Renderer_ApplyState)
    EC_UPDATE_CALL(gs, EC_FLAG_COLLIDER, EC_Collider_ReadState);
    return (stream - origin);
}

void Test_WriteTestEntityBuffer(GameState* gs)
{
    printf("--- ECS Test ---\n");
    #if 1
    // start write sequence
    u8* headerPos = App_StartCommandStream();
    u32 size = 0;
    
    // write state header
    Cmd_EntityStateHeader h = {};
    h.entId = Ent_ReserveFreeEntity(&gs->entList);
    h.componentBits |= EC_FLAG_TRANSFORM;
    h.componentBits |= EC_FLAG_RENDERER;
    h.componentBits |= EC_FLAG_COLLIDER;

    size += App_WriteCommandBytes((u8*)&h, sizeof(Cmd_EntityStateHeader));

    // create transform state
    Transform t = {};
    Transform_SetToIdentity(&t);
    t.pos.y += 0.5f;

    size += App_WriteCommandBytes((u8*)&t, sizeof(Transform));
    
    // Wall texture "textures\\COMP03_1.bmp"
    // Metal texture "textures\\W33_5.bmp"

    // create renderer state
    EC_RendererState r = {};
    COM_CopyStringLimited("Cube", r.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited(("textures\\W33_5.bmp"), r.textureName, EC_RENDERER_STRING_LENGTH);

    size += App_WriteCommandBytes((u8*)&r, sizeof(EC_RendererState));

    // Create Collider
    EC_ColliderState col = {};
    col.def.SetAsBox(
        t.pos.x, t.pos.y, t.pos.z,
        0.5f, 0.5f, 0.5f, 
        0,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEBRIS
    );

    size += App_WriteCommandBytes((u8*)&col, sizeof(EC_ColliderState));

    // Close command
    App_FinishCommandStream(headerPos, CMD_TYPE_ENTITY_STATE_2, 0, (u16)size);
    printf("Wrote cmd stream type %d, size %d bytes\n", CMD_TYPE_ENTITY_STATE_2, size);
    #endif
}
