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

///////////////////////////////////////////////////////////////////////////////////
// TRANSFORM
///////////////////////////////////////////////////////////////////////////////////
void EC_Transform_ApplyState(GameState* gs, Ent* ent, Transform* transform)
{
    ent->transform = *  transform;
}

u32 EC_Transform_ReadState(GameState* gs, Ent* ent, u8* stream)
{
    u8* origin = stream;
    Transform t = {};
    stream += COM_COPY_STRUCT(stream, &t, Transform);
    printf("  Read transform pos %.2f, %.2f, %.2f\n",
        t.pos.x,
        t.pos.y,
        t.pos.z
    );
	EC_Transform_ApplyState(gs, ent, &t);
    return (stream - origin);
}

///////////////////////////////////////////////////////////////////////////////////
// RENDERER
///////////////////////////////////////////////////////////////////////////////////
void EC_Renderer_ApplyState(GameState* gs, Ent* ent, EC_RendererState* state)
{
    EC_Renderer* r = EC_FindRenderer(gs, ent);
    if (r == NULL)
    {
        r = EC_AddRenderer(gs, ent);
    }
    RendObj_SetAsMesh(
        &r->rendObj,
        Assets_GetMeshDataByName(state->meshName),// g_meshCube,
        1, 1, 1,
        AppGetTextureIndexByName(state->textureName)
    );
}

u32 EC_Renderer_ReadState(GameState* gs, Ent* ent, u8* stream)
{
    u8* origin = stream;
    EC_RendererState state = {};
    stream += COM_COPY_STRUCT(stream, &state, EC_RendererState);
    printf("Read Renderer State\n  Mesh: %s\n  Texture: %s\n",
        state.meshName,
        state.textureName
    );
    EC_Renderer_ApplyState(gs, ent, &state);
    return (stream - origin);
}

///////////////////////////////////////////////////////////////////////////////////
// COLLIDER
///////////////////////////////////////////////////////////////////////////////////

void EC_ColliderApplyState(GameState* gs, Ent* ent, EC_ColliderState* state)
{
    EC_Collider* col = EC_FindCollider(gs, ent);
    if (col == NULL)
    {
        printf("  GAME Create collider for %d/%d\n",
			ent->entId.iteration,
			ent->entId.index
		);
        col = EC_AddCollider(gs, ent);
        col->state = *state;
        col->shapeId = Phys_CreateShape(&col->state.def, ent->entId.index, ent->entId.iteration);
        //col->state.shapeId.SetAsBox(0, 0, 0, state->def);
    }
    else
    {
        // Restore state...
		// So who is setting the position here?
    }
}

u32 EC_Collider_ReadState(GameState* gs, Ent* ent, u8* stream)
{
    u8* origin = stream;
    EC_ColliderState state = {};
    stream += COM_COPY_STRUCT(stream, &state, EC_ColliderState);
    EC_ColliderApplyState(gs, ent, &state);
    return (stream - origin);
}

///////////////////////////////////////////////////////////////////////////////////
// HEALTH
///////////////////////////////////////////////////////////////////////////////////
void EC_HealthApplyState(GameState* gs, Ent* ent, EC_HealthState* state)
{
    EC_Health* hp = EC_FindHealth(gs, ent);
    if (hp == NULL)
    {
        hp = EC_AddHealth(gs, ent);
        hp->state.hp = state->hp;
    }
}

u32 EC_Health_ReadState(GameState* gs, Ent* ent, u8* stream)
{
    u8* origin = stream;
    EC_HealthState state = {};
    stream += COM_COPY_STRUCT(stream, &state, EC_HealthState);
    return (stream - origin);
}

#define EC_UPDATE_CALL(gameState, flag, func) \
{ \
    if (componentBits & flag##) \
    { \
        stream += func##(##gameState##, ent, stream); \
    } \
}

#define EC_APPLY_COMPONENT_STATE(gameState, flag, func) \
{ \
    if (componentBits & flag##) \
    { \
        stream += func##(##gameState##, ent, stream); \
    } \
}

void Ent_ApplyStateData(GameState* gs, Entity_FullState* state)
{
    Ent* ent = Ent_GetEntityById(&gs->entList, &state->entId);
    if (ent == NULL)
    {
        // create!
        ent = Ent_GetAndAssign(&gs->entList, &state->entId);
        ent->factoryType = ENTITY_TYPE_RIGIDBODY_CUBE;
    }
    if (state->componentBits & EC_FLAG_TRANSFORM) { EC_Transform_ApplyState(gs, ent, &state->transform); }
    if (state->componentBits & EC_FLAG_RENDERER) { EC_Renderer_ApplyState(gs, ent, &state->renderState); }
    if (state->componentBits & EC_FLAG_COLLIDER) { EC_ColliderApplyState(gs, ent, &state->colliderState); }
    if (state->componentBits & EC_FLAG_HEALTH) { EC_HealthApplyState(gs, ent, &state->healthState); }
}

u32 Ent_ReadStateData(GameState* gs, u8* stream, u32 numBytes)
{
    u8* origin = stream;
    Cmd_EntityStateHeader h = {};
    stream += COM_COPY_STRUCT(stream, &h, Cmd_EntityStateHeader);
    #if 1
    printf("Reading %d bytes of state for ent %d/%d, comp bits %d:\n  ",
        numBytes,
        h.entId.iteration,
        h.entId.index,
        h.componentBits
    );
    COM_PrintBits(h.componentBits, 1);
    #endif

    Entity_FullState state = {};
    state.entId = h.entId;
    state.componentBits = h.componentBits;

    if (h.componentBits & EC_FLAG_TRANSFORM) { stream += COM_COPY_STRUCT(stream, &state.transform, Transform); }
    if (h.componentBits & EC_FLAG_RENDERER) { stream += COM_COPY_STRUCT(stream, &state.renderState, EC_RendererState); }
    if (h.componentBits & EC_FLAG_COLLIDER) { stream += COM_COPY_STRUCT(stream, &state.colliderState, EC_ColliderState); }
    if (h.componentBits & EC_FLAG_ACTORMOTOR) { stream += COM_COPY_STRUCT(stream, &state.actorState, EC_ActorMotorState); }
    if (h.componentBits & EC_FLAG_HEALTH) { stream += COM_COPY_STRUCT(stream, &state.healthState, EC_HealthState); }
    if (h.componentBits & EC_FLAG_PROJECTILE) { stream += COM_COPY_STRUCT(stream, &state.projectileState, EC_ProjectileState); }
    if (h.componentBits & EC_FLAG_LABEL) { stream += COM_COPY_STRUCT(stream, &state.labelState, EC_LabelState); }

    u32 read = (stream - origin);
    if (read != numBytes)
    {
        printf("!GAME Read state data expected %d bytes but read %d!\n", numBytes, read);
        ILLEGAL_CODE_PATH
    }
    
    Ent_ApplyStateData(gs, &state);

    return read;
}

void Test_ReadEntityState(Entity_FullState* target, u8* stream, u32 numBytes)
{
    
}

void Test_WriteEntityState(Entity_FullState* target, u8* stream, u32 numBytes)
{
    
}

void Test_Template(GameState* gs)
{
    // Spawn an entity of a type but with specific state overridden...
    /*
    > Original
        \/
    patch fields
        \/
    > New version
    */
}

void Test_WriteTestEntityBuffer(GameState* gs, EntitySpawnOptions* options)
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
    h.componentBits |= EC_FLAG_HEALTH;

    size += App_WriteCommandBytes((u8*)&h, sizeof(Cmd_EntityStateHeader));

    // create transform state
    Transform t = {};
    Transform_SetToIdentity(&t);
    Transform_SetByPosAndDegrees(&t, &options->pos, &options->rot);
    Transform_SetScale(&t, options->scale.x, options->scale.y, options->scale.z);

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

    EC_HealthState state = {};
    state.hp = 100;
    size += App_WriteCommandBytes((u8*)&state, sizeof(EC_HealthState));

    // Close command
    App_FinishCommandStream(headerPos, CMD_TYPE_ENTITY_STATE_2, 0, (u16)size);
    printf("ECS test Wrote cmd stream type %d, size %d bytes\n", CMD_TYPE_ENTITY_STATE_2, size);
    #endif
}

void Game_WriteSpawnCmd(GameState* gs, i32 factoryType, EntitySpawnOptions* options)
{
    switch (factoryType)
    {
        case ENTITY_TYPE_RIGIDBODY_CUBE:
        {
            Test_WriteTestEntityBuffer(gs, options);
        } break;

        default:
        {
            printf("GAME Unknown factory type %d\n", factoryType);
        } break;
    }
}
