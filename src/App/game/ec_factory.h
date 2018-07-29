#pragma once

#include "game.h"

///////////////////////////////////////////////////////////////////////////////////
// APPLY COMPONENT STATE
///////////////////////////////////////////////////////////////////////////////////
void EC_TransformApplyState(GameState* gs, Ent* ent, Transform* transform)
{
    EC_Transform* ecT = EC_FindTransform(gs, &ent->entId);
	if (ecT == NULL)
	{
		ecT = EC_AddTransform(gs, ent);
	}
    ecT->t = *transform;
}

void EC_RendererApplyState(GameState* gs, Ent* ent, EC_RendererState* state)
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
        col->shapeId = Phys_CreateShape(
            &col->state.def,
            ent->entId.index,
            ent->entId.iteration
        );
    }
    else
    {
        // Restore state...
		// So who is setting the position here?
    }
}

void EC_ActorMotorApplyState(GameState* gs, Ent* ent, EC_ActorMotorState* state)
{
    EC_ActorMotor* motor = EC_FindActorMotor(gs, ent);
    if (motor == NULL)
    {
        motor = EC_AddActorMotor(gs, ent);
    }
    motor->state = *state;
}

void EC_HealthApplyState(GameState* gs, Ent* ent, EC_HealthState* state)
{
    EC_Health* hp = EC_FindHealth(gs, ent);
    if (hp == NULL)
    {
        hp = EC_AddHealth(gs, ent);
        hp->state.hp = state->hp;
    }
}

void EC_ProjectileApplyState(GameState* gs, Ent* ent, EC_ProjectileState* state)
{
    EC_Projectile* prj = EC_FindProjectile(gs, ent);
    if (prj == NULL)
    {
        prj = EC_AddProjectile(gs, ent);
    }
    prj->state = *state;
}

void EC_LabelApplyState(GameState* gs, Ent* ent, EC_LabelState* state)
{
    EC_Label* label = EC_FindLabel(gs, ent);
    if (label == NULL)
    {
        label = EC_AddLabel(gs, ent);
    }
    COM_CopyStringLimited(state->label, label->state.label, EC_LABEL_LENGTH);
}

///////////////////////////////////////////////////////////////////////////////////
// APPLY ENTITY STATE
///////////////////////////////////////////////////////////////////////////////////
void Ent_ApplyStateData(GameState* gs, EntityState* state)
{
    Ent* ent = Ent_GetEntityById(&gs->entList, &state->entId);
    if (ent == NULL)
    {
        // create for now. TODO: Split into create/update functions
        ent = Ent_GetAndAssign(&gs->entList, &state->entId);
        ent->factoryType = state->factoryType;
    }
    ent->source = state->source;
    if (state->componentBits & EC_FLAG_TRANSFORM) { EC_TransformApplyState(gs, ent, &state->transform); }
    if (state->componentBits & EC_FLAG_RENDERER) { EC_RendererApplyState(gs, ent, &state->renderState); }
    if (state->componentBits & EC_FLAG_COLLIDER) { EC_ColliderApplyState(gs, ent, &state->colliderState); }
    if (state->componentBits & EC_FLAG_ACTORMOTOR) { EC_ActorMotorApplyState(gs, ent, &state->actorState); }
    if (state->componentBits & EC_FLAG_HEALTH) { EC_HealthApplyState(gs, ent, &state->healthState); }
    if (state->componentBits & EC_FLAG_PROJECTILE) { EC_ProjectileApplyState(gs, ent, &state->projectileState); }
    if (state->componentBits & EC_FLAG_LABEL) { EC_LabelApplyState(gs, ent, &state->labelState); }
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

    EntityState state = {};
    state.entId = h.entId;
    state.componentBits = h.componentBits;
	
	// TODO: Replace raw struct copy with proper encoding functions!
    // WARNING: THIS IS ORDER DEPENDENT!
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

/**
 * Fill out a state struct for the given entity
 */
void Ent_CopyFullEntityState(EntityState* state, Ent* ent)
{
    
}

/**
 * Write a state command to output.
 */
u16 Ent_WriteEntityStateCmd(EntityState* state)
{
    const u32 bufferSize = 1024;
    u8 buffer[bufferSize];
    u8* origin = buffer;
    u8* stream = origin;
    
    
    Cmd_EntityStateHeader h = {};
    h.entId = state->entId;
    h.componentBits = state->componentBits;
    stream += COM_COPY_STRUCT(&h, stream, Cmd_EntityStateHeader);
    
    if (h.componentBits & EC_FLAG_TRANSFORM)
    { stream += COM_COPY_STRUCT(&state->transform, stream, Transform); }
    if (h.componentBits & EC_FLAG_RENDERER)
    { stream += COM_COPY_STRUCT(&state->renderState, stream, EC_RendererState); }
    if (h.componentBits & EC_FLAG_COLLIDER)
    { stream += COM_COPY_STRUCT(&state->colliderState, stream, EC_RendererState); }
    if (h.componentBits & EC_FLAG_ACTORMOTOR)
    { stream += COM_COPY_STRUCT(&state->actorState, stream, EC_ActorMotorState); }
    if (h.componentBits & EC_FLAG_HEALTH)
    { stream += COM_COPY_STRUCT(&state->healthState, stream, EC_HealthState); }
    if (h.componentBits & EC_FLAG_PROJECTILE)
    { stream += COM_COPY_STRUCT(&state->projectileState, stream, EC_ProjectileState); }
    if (h.componentBits & EC_FLAG_LABEL)
    { stream += COM_COPY_STRUCT(&state->labelState, stream, EC_LabelState); }

    u16 bytesWritten = (u16)(stream - origin);

    u8* cmdOrigin = App_StartCommandStream();
    App_WriteCommandBytes(origin, bytesWritten);
    App_FinishCommandStream(cmdOrigin, CMD_TYPE_ENTITY_STATE_2, 0, bytesWritten);

    return bytesWritten;
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

EntId Game_WriteSpawnCmd(GameState* gs, i32 factoryType, EntitySpawnOptions* options)
{
    EntityState s = {};
    EntId entId = Ent_ReserveFreeEntity(&gs->entList);
    s.entId = entId;
    if (Game_WriteSpawnTemplate(factoryType, &s, options))
    {
        Ent_WriteEntityStateCmd(&s);
    }
    return entId;
}
