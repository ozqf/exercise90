#pragma once

#include "game.h"

///////////////////////////////////////////////////////////////////////////////////
// APPLY ENTITY STATE
///////////////////////////////////////////////////////////////////////////////////
internal void Ent_ApplyStateData(GameState* gs, EntityState* state)
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
    if (state->componentBits & EC_FLAG_VOLUME) { EC_VolumeApplyState(gs, ent, &state->volumeState); }

}

internal u32 Ent_ReadStateData(GameState* gs, u8* stream, u32 numBytes)
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
    if (h.componentBits & EC_FLAG_ENTITY) 			{ stream += COM_COPY_STRUCT(stream, &state.entMetaData, Ent); }
    if (h.componentBits & EC_FLAG_TRANSFORM) 		{ stream += COM_COPY_STRUCT(stream, &state.transform, Transform); }
    if (h.componentBits & EC_FLAG_RENDERER) 		{ stream += COM_COPY_STRUCT(stream, &state.renderState, EC_RendObjState); }
    if (h.componentBits & EC_FLAG_COLLIDER) 		{ stream += COM_COPY_STRUCT(stream, &state.colliderState, EC_ColliderState); }
    if (h.componentBits & EC_FLAG_AICONTROLLER) 	{ stream += COM_COPY_STRUCT(stream, &state.aiState, EC_AIState); }
    if (h.componentBits & EC_FLAG_ACTORMOTOR) 		{ stream += COM_COPY_STRUCT(stream, &state.actorState, EC_ActorMotorState); }
    if (h.componentBits & EC_FLAG_HEALTH) 			{ stream += COM_COPY_STRUCT(stream, &state.healthState, EC_HealthState); }
    if (h.componentBits & EC_FLAG_PROJECTILE) 		{ stream += COM_COPY_STRUCT(stream, &state.projectileState, EC_ProjectileState); }
    if (h.componentBits & EC_FLAG_LABEL) 			{ stream += COM_COPY_STRUCT(stream, &state.labelState, EC_LabelState); }
    if (h.componentBits & EC_FLAG_THINKER) 			{ stream += COM_COPY_STRUCT(stream, &state.thinkerState, EC_ThinkerState); }
    if (h.componentBits & EC_FLAG_MULTI_RENDOBJ)	{ stream += COM_COPY_STRUCT(stream, &state.multiRendState, EC_MultiRendObjState); }
    if (h.componentBits & EC_FLAG_VOLUME)	        { stream += COM_COPY_STRUCT(stream, &state.volumeState, EC_VolumeState); }

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
internal u16 Ent_WriteEntityStateCmd(u8* optionalOutputStream, EntityState* state)
{
	// bumping this up 
    const u32 bufferSize = (sizeof(EntityState) * 2);
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
    if (h.componentBits & EC_FLAG_VOLUME)
    { stream += COM_COPY_STRUCT(&state->volumeState, stream, EC_VolumeState); }

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

internal void Ent_PrintComponents(Ent* ent)
{
	printf("ENT %d/%d components\n", ent->entId.iteration, ent->entId.index);
	COM_PrintBits(ent->componentBits, 1);
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
	if (Ent_HasMultiRendObj(ent))
	{
		printf("  Has Multi-Renderer\n");
	}
}

/**
 * Fill out a state struct for the given entity
 */
internal void Ent_CopyFullEntityState(GameState* gs, Ent* ent, EntityState* state)
{
	if (gs->verbose)
	{
		Ent_PrintComponents(ent);
	}
    *state = {};
    state->entId = ent->entId;
    state->componentBits = ent->componentBits;
    EC_ApplyEntityMetaData(gs, &state->entMetaData, ent);
    //state->entMetaData = *ent;

    EC_Transform* ecT = EC_FindTransform(gs, ent);

	if (ecT) { state->transform = ecT->t; }
    if (ent->componentBits & EC_FLAG_RENDERER)
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
    if (ent->componentBits & EC_FLAG_ACTORMOTOR) { state->actorState = (EC_FindActorMotor(gs, ent))->state; }
	if (ent->componentBits & EC_FLAG_AICONTROLLER) { state->aiState = (EC_FindAIController(gs, ent))->state; }
	if (ent->componentBits & EC_FLAG_HEALTH) { state->healthState = (EC_FindHealth(gs, ent))->state; }
    if (ent->componentBits & EC_FLAG_PROJECTILE) { state->projectileState = (EC_FindProjectile(gs, ent))->state; }
    if (ent->componentBits & EC_FLAG_LABEL) { state->labelState = (EC_FindLabel(gs, ent))->state; }
    if (ent->componentBits & EC_FLAG_MULTI_RENDOBJ)
    { state->multiRendState = (EC_FindMultiRendObj(gs, ent))->state; }
    if (ent->componentBits & EC_FLAG_VOLUME)
    { state->volumeState = (EC_FindVolume(gs, ent))->state; }
}

internal u8 Game_PrepareSpawnCmd(GameState* gs, i32 factoryType, EntityState* target, EntitySpawnOptions* options)
{
    *target = {};
    EntId entId = Ent_ReserveFreeEntity(&gs->entList);
    
    u8 result = Game_WriteSpawnTemplate(factoryType, target, options);
    target->entId = entId;
    return result;
}

internal EntId Game_WriteSpawnCmd(GameState* gs, i32 factoryType, EntitySpawnOptions* options)
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
