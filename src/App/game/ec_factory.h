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
        if (gs->verbose)
        {
            printf("EC Creating EC_Transform for %d/%d\n", ent->entId.iteration, ent->entId.index);
        }
	}
    ecT->t = *transform;
}

void EC_RestoreRendObj(RendObj* rendObj, EC_RendObjState* state)
{
    RendObj_SetAsMesh(
        rendObj,
        Assets_GetMeshDataByName(state->meshName),// g_meshCube,
        state->colourRGB[0], state->colourRGB[1], state->colourRGB[2],
        AppGetTextureIndexByName(state->textureName)
    );
}

void EC_SingleRendObjApplyState(GameState* gs, Ent* ent, EC_RendObjState* state)
{
    EC_SingleRendObj* r = EC_FindSingleRendObj(gs, ent);
    if (r == NULL)
    {
        r = EC_AddSingleRendObj(gs, ent);
    }
    r->state = *state;
	COM_CopyStringLimited(state->meshName, r->state.meshName, EC_RENDERER_STRING_LENGTH);
	COM_CopyStringLimited(state->textureName, r->state.textureName, EC_RENDERER_STRING_LENGTH);
    EC_RestoreRendObj(&r->rendObj, &r->state);
    //RendObj_SetAsMesh(
    //    &r->rendObj,
    //    Assets_GetMeshDataByName(state->meshName),// g_meshCube,
    //    state->colourRGB[0], state->colourRGB[1], state->colourRGB[2],
    //    AppGetTextureIndexByName(state->textureName)
    //);
}

void EC_MultiRendObjApplyState(GameState* gs, Ent* ent, EC_MultiRendObjState* state)
{
    EC_MultiRendObj* r = EC_FindMultiRendObj(gs, ent);
    if (r == NULL)
    {
        r = EC_AddMultiRendObj(gs, ent);
    }
    r->state = *state;
    COM_CopyStringLimited(state->objStates[0].meshName, state->objStates[0].meshName, EC_RENDERER_STRING_LENGTH);
    EC_RestoreRendObj(&r->rendObjs[EC_MULTI_RENDOBJ_BASE], &state->objStates[EC_MULTI_RENDOBJ_BASE]);
    EC_RestoreRendObj(&r->rendObjs[EC_MULTI_RENDOBJ_HEAD], &state->objStates[EC_MULTI_RENDOBJ_HEAD]);
}

void EC_ColliderApplyState(GameState* gs, Ent* ent, EC_ColliderState* state)
{
    EC_Collider* col = EC_FindCollider(gs, ent);
    if (col == NULL)
    {
        if (gs->verbose)
        {
            printf("  GAME Create collider for %d/%d\n",
			    ent->entId.iteration,
			    ent->entId.index
		    );
        }
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

void EC_ThinkerApplyState(GameState* gs, Ent* ent, EC_ThinkerState* state)
{
    EC_Thinker* thinker = EC_FindThinker(gs, ent);
    if (thinker == NULL)
    {
        thinker = EC_AddThinker(gs, ent);
    }
    thinker->state = *state;
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

void EC_AIControllerApplyState(GameState* gs, Ent* ent, EC_AIState* state)
{
    EC_AIController* ai = EC_FindAIController(gs, ent);
    if (ai == NULL)
    {
        ai = EC_AddAIController(gs, ent);
    }
    ai->state = *state;
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

void EC_ApplyEntityMetaData(GameState* gs, Ent* ent, Ent* entState)
{
    // TODO: Move these data items into an 'EC_MetaData' component
    // In theory safe to copy everything but being careful
    // In use and Id should be handled locally
    //ent->inUse = entState->inUse;

    ent->tag = entState->tag;
    ent->source = entState->source;
    ent->factoryType = entState->factoryType;
    ent->componentFlags = entState->componentFlags;
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
        if (gs->verbose)
        {
            printf("  Failed to find ent %d/%d, assigning\n", state->entId.iteration, state->entId.index);
        }
        ent = Ent_GetAndAssign(&gs->entList, &state->entId);
    }
    if (state->componentBits & EC_FLAG_ENTITY) { EC_ApplyEntityMetaData(gs, ent, &state->entMetaData); }
    if (state->componentBits & EC_FLAG_TRANSFORM) { EC_TransformApplyState(gs, ent, &state->transform); }
    if (state->componentBits & EC_FLAG_RENDERER) { EC_SingleRendObjApplyState(gs, ent, &state->renderState); }
    if (state->componentBits & EC_FLAG_COLLIDER) { EC_ColliderApplyState(gs, ent, &state->colliderState); }
    if (state->componentBits & EC_FLAG_AICONTROLLER) { EC_AIControllerApplyState(gs, ent, &state->aiState); }
    if (state->componentBits & EC_FLAG_ACTORMOTOR) { EC_ActorMotorApplyState(gs, ent, &state->actorState); }
    if (state->componentBits & EC_FLAG_HEALTH) { EC_HealthApplyState(gs, ent, &state->healthState); }
    if (state->componentBits & EC_FLAG_PROJECTILE) { EC_ProjectileApplyState(gs, ent, &state->projectileState); }
    if (state->componentBits & EC_FLAG_LABEL) { EC_LabelApplyState(gs, ent, &state->labelState); }
    if (state->componentBits & EC_FLAG_THINKER) { EC_ThinkerApplyState(gs, ent, &state->thinkerState); }
    if (state->componentBits & EC_FLAG_MULTI_RENDOBJ) { EC_MultiRendObjApplyState(gs, ent, &state->multiRendState); }
}

u32 Ent_ReadStateData(GameState* gs, u8* stream, u32 numBytes)
{
    u8* origin = stream;
    Cmd_EntityStateHeader h = {};
    stream += COM_COPY_STRUCT(stream, &h, Cmd_EntityStateHeader);
    #if 1
    if (gs->verbose)
    {
        printf("Reading %d bytes of state for ent %d/%d, comp bits %d:\n  ",
            numBytes,
            h.entId.iteration,
            h.entId.index,
            h.componentBits
        );
        COM_PrintBits(h.componentBits, 1);
    }
    
    #endif

    EntityState state = {};
    state.entId = h.entId;
    state.componentBits = h.componentBits;
	
	// TODO: Replace raw struct copy with proper encoding functions!
    // WARNING: THIS IS ORDER DEPENDENT!
    if (h.componentBits & EC_FLAG_ENTITY) { stream += COM_COPY_STRUCT(stream, &state.entMetaData, Ent); }
    if (h.componentBits & EC_FLAG_TRANSFORM) { stream += COM_COPY_STRUCT(stream, &state.transform, Transform); }
    if (h.componentBits & EC_FLAG_RENDERER) { stream += COM_COPY_STRUCT(stream, &state.renderState, EC_RendObjState); }
    if (h.componentBits & EC_FLAG_COLLIDER) { stream += COM_COPY_STRUCT(stream, &state.colliderState, EC_ColliderState); }
    if (h.componentBits & EC_FLAG_AICONTROLLER) { stream += COM_COPY_STRUCT(stream, &state.aiState, EC_AIState); }
    if (h.componentBits & EC_FLAG_ACTORMOTOR) { stream += COM_COPY_STRUCT(stream, &state.actorState, EC_ActorMotorState); }
    if (h.componentBits & EC_FLAG_HEALTH) { stream += COM_COPY_STRUCT(stream, &state.healthState, EC_HealthState); }
    if (h.componentBits & EC_FLAG_PROJECTILE) { stream += COM_COPY_STRUCT(stream, &state.projectileState, EC_ProjectileState); }
    if (h.componentBits & EC_FLAG_LABEL) { stream += COM_COPY_STRUCT(stream, &state.labelState, EC_LabelState); }
    if (h.componentBits & EC_FLAG_THINKER) { stream += COM_COPY_STRUCT(stream, &state.thinkerState, EC_ThinkerState); }
    if (h.componentBits & EC_FLAG_MULTI_RENDOBJ) { stream += COM_COPY_STRUCT(stream, &state.multiRendState, EC_MultiRendObjState); }

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
 * Write a state command to output.
 */
u16 Ent_WriteEntityStateCmd(u8* optionalOutputStream, EntityState* state)
{
    const u32 bufferSize = 1024;
    u8 buffer[bufferSize];
    u8* origin = buffer;
    u8* stream = origin;
    
	// TODO: Fix me! Force entity meta data flag. Loading will always read it
	state->componentBits |= EC_FLAG_ENTITY;

    // TODO: Replace raw struct copy with proper encoding functions!
    // WARNING: THIS IS ORDER DEPENDENT!    
    Cmd_EntityStateHeader h = {};
    h.entId = state->entId;
    h.componentBits = state->componentBits;
    stream += COM_COPY_STRUCT(&h, stream, Cmd_EntityStateHeader);

    if (h.componentBits & EC_FLAG_ENTITY)
    { stream += COM_COPY_STRUCT(&state->entMetaData, stream, Ent); }
    if (h.componentBits & EC_FLAG_TRANSFORM)
    { stream += COM_COPY_STRUCT(&state->transform, stream, Transform); }
    if (h.componentBits & EC_FLAG_RENDERER)
    { stream += COM_COPY_STRUCT(&state->renderState, stream, EC_RendObjState); }
    if (h.componentBits & EC_FLAG_COLLIDER)
    { stream += COM_COPY_STRUCT(&state->colliderState, stream, EC_ColliderState); }
    if (h.componentBits & EC_FLAG_AICONTROLLER)
    { stream += COM_COPY_STRUCT(&state->aiState, stream, EC_AIState); }
    if (h.componentBits & EC_FLAG_ACTORMOTOR)
    { stream += COM_COPY_STRUCT(&state->actorState, stream, EC_ActorMotorState); }
    if (h.componentBits & EC_FLAG_HEALTH)
    { stream += COM_COPY_STRUCT(&state->healthState, stream, EC_HealthState); }
    if (h.componentBits & EC_FLAG_PROJECTILE)
    { stream += COM_COPY_STRUCT(&state->projectileState, stream, EC_ProjectileState); }
    if (h.componentBits & EC_FLAG_LABEL)
    { stream += COM_COPY_STRUCT(&state->labelState, stream, EC_LabelState); }
    if (h.componentBits & EC_FLAG_THINKER)
    { stream += COM_COPY_STRUCT(&state->thinkerState, stream, EC_ThinkerState); }
    if (h.componentBits & EC_FLAG_MULTI_RENDOBJ)
    { stream += COM_COPY_STRUCT(&state->multiRendState, stream, EC_MultiRendObjState); }

    u16 bytesWritten = (u16)(stream - origin);

    if (optionalOutputStream == NULL)
    {
        // chuck into main app output
        u8* cmdOrigin = App_StartCommandStream();
        App_WriteCommandBytesToFrameOutput(origin, bytesWritten);
        App_FinishCommandStream(cmdOrigin, CMD_TYPE_ENTITY_STATE_2, 0, bytesWritten);
        return bytesWritten;
    }
    else
    {
        CmdHeader cmdHeader = {};
        cmdHeader.SetType(CMD_TYPE_ENTITY_STATE_2);
        cmdHeader.SetSize(bytesWritten);
        optionalOutputStream += cmdHeader.Write(optionalOutputStream);
        optionalOutputStream += COM_COPY(origin, optionalOutputStream, bytesWritten);
        return bytesWritten + sizeof(CmdHeader);
    }
}

void Ent_PrintComponents(Ent* ent)
{
	printf("ENT %d/%d components\n", ent->entId.iteration, ent->entId.index);
	COM_PrintBits(ent->componentFlags, 1);
	if (Ent_HasTransform(ent))
	{
		printf("  Has Transform\n");
	}
	if (Ent_HasSingleRendObj(ent))
	{
		printf("  Has Single Renderer\n");
	}
	if (Ent_HasCollider(ent))
	{
		printf("  Has Collider\n");
	}
	if (Ent_HasActorMotor(ent))
	{
		printf("  Has Actor\n");
	}
	if (Ent_HasHealth(ent))
	{
		printf("  Has Health\n");
	}
	if (Ent_HasProjectile(ent))
	{
		printf("  Has Projectile\n");
	}
}

/**
 * Fill out a state struct for the given entity
 */
void Ent_CopyFullEntityState(GameState* gs, Ent* ent, EntityState* state)
{
	if (gs->verbose)
	{
		Ent_PrintComponents(ent);
	}
    *state = {};
    state->entId = ent->entId;
    state->componentBits = ent->componentFlags;
    EC_ApplyEntityMetaData(gs, &state->entMetaData, ent);
    //state->entMetaData = *ent;

    EC_Transform* ecT = EC_FindTransform(gs, ent);

	if (ecT) { state->transform = ecT->t; }
    if (ent->componentFlags & EC_FLAG_RENDERER)
    {
        EC_SingleRendObj* r = EC_FindSingleRendObj(gs, ent);
        state->renderState = r->state;
        COM_CopyStringLimited(r->state.meshName, state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
        COM_CopyStringLimited(r->state.textureName, state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
    }
    EC_Collider* collider = EC_FindCollider(gs, ent);
    if (collider)
    {
        if (ecT)
        {
            collider->state.def.pos[0] = ecT->t.pos.x;
            collider->state.def.pos[1] = ecT->t.pos.y;
            collider->state.def.pos[2] = ecT->t.pos.z;
        }
        state->colliderState = (EC_FindCollider(gs, ent))->state;
    }
    if (ent->componentFlags & EC_FLAG_ACTORMOTOR) { state->actorState = (EC_FindActorMotor(gs, ent))->state; }
	if (ent->componentFlags & EC_FLAG_HEALTH) { state->healthState = (EC_FindHealth(gs, ent))->state; }
    if (ent->componentFlags & EC_FLAG_PROJECTILE) { state->projectileState = (EC_FindProjectile(gs, ent))->state; }
    if (ent->componentFlags & EC_FLAG_LABEL) { state->labelState = (EC_FindLabel(gs, ent))->state; }
    if (ent->componentFlags & EC_FLAG_MULTI_RENDOBJ) { state->multiRendState = (EC_FindMultiRendObj(gs, ent))->state; }
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

    size += App_WriteCommandBytesToFrameOutput((u8*)&h, sizeof(Cmd_EntityStateHeader));

    // create transform state
    Transform t = {};
    Transform_SetToIdentity(&t);
    Transform_SetByPosAndDegrees(&t, &options->pos, &options->rot);
    Transform_SetScale(&t, options->scale.x, options->scale.y, options->scale.z);

    size += App_WriteCommandBytesToFrameOutput((u8*)&t, sizeof(Transform));
    
    // Wall texture "textures\\COMP03_1.bmp"
    // Metal texture "textures\\W33_5.bmp"

    // create renderer state
    EC_RendObjState r = {};
    COM_CopyStringLimited("Cube", r.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited(("textures\\W33_5.bmp"), r.textureName, EC_RENDERER_STRING_LENGTH);

    size += App_WriteCommandBytesToFrameOutput((u8*)&r, sizeof(EC_RendObjState));

    // Create Collider
    EC_ColliderState col = {};
    col.def.SetAsBox(
        t.pos.x, t.pos.y, t.pos.z,
        0.5f, 0.5f, 0.5f, 
        0,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEBRIS
    );

    size += App_WriteCommandBytesToFrameOutput((u8*)&col, sizeof(EC_ColliderState));

    EC_HealthState state = {};
    state.hp = 100;
    size += App_WriteCommandBytesToFrameOutput((u8*)&state, sizeof(EC_HealthState));

    // Close command
    App_FinishCommandStream(headerPos, CMD_TYPE_ENTITY_STATE_2, 0, (u16)size);
    printf("ECS test Wrote cmd stream type %d, size %d bytes\n", CMD_TYPE_ENTITY_STATE_2, size);
    #endif
}

void Game_PrepareSpawnCmd(GameState* gs, i32 factoryType, EntitySpawnOptions* options)
{
    
}

EntId Game_WriteSpawnCmd(GameState* gs, i32 factoryType, EntitySpawnOptions* options)
{
    EntityState s = {};
    EntId entId = Ent_ReserveFreeEntity(&gs->entList);
    s.entId = entId;
    if (Game_WriteSpawnTemplate(factoryType, &s, options))
    {
        Ent_WriteEntityStateCmd(NULL, &s);
    }
    return entId;
}
