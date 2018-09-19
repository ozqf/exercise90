#pragma once

#include "game.h"

///////////////////////////////////////////////////////////////////////////////////
// APPLY ENTITY STATE
///////////////////////////////////////////////////////////////////////////////////

// Central point of entity state creation/update
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
    if (state->componentBits & EC_BIT0_ENTITY) 			{ EC_ApplyEntityMetaData(gs, ent, &state->entMetaData); }
    if (state->componentBits & EC_BIT1_TRANSFORM) 		{ EC_TransformApplyState(gs, ent, &state->transform); }
    if (state->componentBits & EC_BIT2_RENDERER) 		{ EC_SingleRendObjApplyState(gs, ent, &state->renderState); }
    if (state->componentBits & EC_BIT3_COLLIDER) 		{ EC_ColliderApplyState(gs, ent, &state->colliderState); }
    if (state->componentBits & EC_BIT4_AICONTROLLER) 	{ EC_AIControllerApplyState(gs, ent, &state->aiState); }
    if (state->componentBits & EC_BIT5_ACTORMOTOR) 		{ EC_ActorMotorApplyState(gs, ent, &state->actorState); }
    if (state->componentBits & EC_BIT6_PROJECTILE) 		{ EC_ProjectileApplyState(gs, ent, &state->projectileState); }
    if (state->componentBits & EC_BIT7_LABEL) 			{ EC_LabelApplyState(gs, ent, &state->labelState); }
    if (state->componentBits & EC_BIT8_HEALTH) 			{ EC_HealthApplyState(gs, ent, &state->healthState); }
    if (state->componentBits & EC_BIT9_THINKER) 		{ EC_ThinkerApplyState(gs, ent, &state->thinkerState); }
    if (state->componentBits & EC_BIT10_MULTI_RENDOBJ) 	{ EC_MultiRendObjApplyState(gs, ent, &state->multiRendState); }
    if (state->componentBits & EC_BIT11_VOLUME) 			{ EC_VolumeApplyState(gs, ent, &state->volumeState); }
    if (state->componentBits & EC_BIT12_SENSOR) 			{ EC_SensorApplyState(gs, ent, &state->sensorState); }

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
    if (h.componentBits & EC_BIT0_ENTITY) 			{ stream += COM_COPY_STRUCT(stream, &state.entMetaData, Ent); }
    if (h.componentBits & EC_BIT1_TRANSFORM) 		{ stream += COM_COPY_STRUCT(stream, &state.transform, Transform); }
    if (h.componentBits & EC_BIT2_RENDERER) 		{ stream += COM_COPY_STRUCT(stream, &state.renderState, EC_RendObjState); }
    if (h.componentBits & EC_BIT3_COLLIDER) 		{ stream += COM_COPY_STRUCT(stream, &state.colliderState, EC_ColliderState); }
    if (h.componentBits & EC_BIT4_AICONTROLLER) 	{ stream += COM_COPY_STRUCT(stream, &state.aiState, EC_AIState); }
    if (h.componentBits & EC_BIT5_ACTORMOTOR) 		{ stream += COM_COPY_STRUCT(stream, &state.actorState, EC_ActorMotorState); }
    if (h.componentBits & EC_BIT6_PROJECTILE) 		{ stream += COM_COPY_STRUCT(stream, &state.projectileState, EC_ProjectileState); }
    if (h.componentBits & EC_BIT7_LABEL) 			{ stream += COM_COPY_STRUCT(stream, &state.labelState, EC_LabelState); }
    if (h.componentBits & EC_BIT8_HEALTH) 			{ stream += COM_COPY_STRUCT(stream, &state.healthState, EC_HealthState); }
    if (h.componentBits & EC_BIT9_THINKER) 			{ stream += COM_COPY_STRUCT(stream, &state.thinkerState, EC_ThinkerState); }
    if (h.componentBits & EC_BIT10_MULTI_RENDOBJ)	{ stream += COM_COPY_STRUCT(stream, &state.multiRendState, EC_MultiRendObjState); }
    if (h.componentBits & EC_BIT11_VOLUME)	        { stream += COM_COPY_STRUCT(stream, &state.volumeState, EC_VolumeState); }
    if (h.componentBits & EC_BIT12_SENSOR)	        { stream += COM_COPY_STRUCT(stream, &state.sensorState, EC_SensorState); }


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
	state->componentBits |= EC_BIT0_ENTITY;

    // TODO: Replace raw struct copy with proper encoding functions!
    // WARNING: THIS IS ORDER DEPENDENT!    
    Cmd_EntityStateHeader h = {};
    h.entId = state->entId;
    h.componentBits = state->componentBits;
    stream += COM_COPY_STRUCT(&h, stream, Cmd_EntityStateHeader);

    if (h.componentBits & EC_BIT0_ENTITY)
    { stream += COM_COPY_STRUCT(&state->entMetaData, stream, Ent); }
    if (h.componentBits & EC_BIT1_TRANSFORM)
    { stream += COM_COPY_STRUCT(&state->transform, stream, Transform); }
    if (h.componentBits & EC_BIT2_RENDERER)
    { stream += COM_COPY_STRUCT(&state->renderState, stream, EC_RendObjState); }
    if (h.componentBits & EC_BIT3_COLLIDER)
    { stream += COM_COPY_STRUCT(&state->colliderState, stream, EC_ColliderState); }
    if (h.componentBits & EC_BIT4_AICONTROLLER)
    { stream += COM_COPY_STRUCT(&state->aiState, stream, EC_AIState); }
    if (h.componentBits & EC_BIT5_ACTORMOTOR)
    { stream += COM_COPY_STRUCT(&state->actorState, stream, EC_ActorMotorState); }
    if (h.componentBits & EC_BIT6_PROJECTILE)
    { stream += COM_COPY_STRUCT(&state->projectileState, stream, EC_ProjectileState); }
    if (h.componentBits & EC_BIT7_LABEL)
    { stream += COM_COPY_STRUCT(&state->labelState, stream, EC_LabelState); }
    if (h.componentBits & EC_BIT8_HEALTH)
    { stream += COM_COPY_STRUCT(&state->healthState, stream, EC_HealthState); }
    if (h.componentBits & EC_BIT9_THINKER)
    { stream += COM_COPY_STRUCT(&state->thinkerState, stream, EC_ThinkerState); }
    if (h.componentBits & EC_BIT10_MULTI_RENDOBJ)
    { stream += COM_COPY_STRUCT(&state->multiRendState, stream, EC_MultiRendObjState); }
    if (h.componentBits & EC_BIT11_VOLUME)
    { stream += COM_COPY_STRUCT(&state->volumeState, stream, EC_VolumeState); }
    if (h.componentBits & EC_BIT12_SENSOR)
    { stream += COM_COPY_STRUCT(&state->sensorState, stream, EC_SensorState); }

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
    if (Ent_HasVolume(ent))
	{
		printf("  Has Volume\n");
	}
    if (Ent_HasSensor(ent))
	{
		printf("  Has Sensor\n");
	}
}

u32 Ent_CalcDiffBits(EntityState* a, EntityState* b)
{
	u32 diffBits = 0;
	if ((a->componentBits & EC_BIT0_ENTITY)
		&& !COM_CompareMemory((u8*)&a->entMetaData, (u8*)&b->entMetaData, sizeof(Ent)))
	{ diffBits |= EC_BIT0_ENTITY; }
	
	if ((a->componentBits & EC_BIT1_TRANSFORM)
		&& !COM_CompareMemory((u8*)&a->transform, (u8*)&b->transform, sizeof(EC_Transform)))
	{ diffBits |= EC_BIT1_TRANSFORM; }
	
	if ((a->componentBits & EC_BIT2_RENDERER)
		&& !COM_CompareMemory((u8*)&a->renderState, (u8*)&b->renderState, sizeof(EC_SingleRendObj)))
	{ diffBits |= EC_BIT2_RENDERER; }
	
	return diffBits;
}

/*internal*/ u32 Ent_CreateTemplateDiffBits(i32 templateId, EntityState* copy)
{
	u32 diffBits = 0;
	EntityState temp;
	Ent_CopyTemplate(templateId, &temp, NULL);
	return Ent_CalcDiffBits(&temp, copy);
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
    if (ent->componentBits & EC_BIT2_RENDERER)
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
    if (ent->componentBits & EC_BIT4_AICONTROLLER) { state->aiState = (EC_FindAIController(gs, ent))->state; }
    if (ent->componentBits & EC_BIT5_ACTORMOTOR) { state->actorState = (EC_FindActorMotor(gs, ent))->state; }
    if (ent->componentBits & EC_BIT6_PROJECTILE) { state->projectileState = (EC_FindProjectile(gs, ent))->state; }
    if (ent->componentBits & EC_BIT7_LABEL) { state->labelState = (EC_FindLabel(gs, ent))->state; }
    if (ent->componentBits & EC_BIT8_HEALTH) { state->healthState = (EC_FindHealth(gs, ent))->state; }
    if (ent->componentBits & EC_BIT9_THINKER) { state->thinkerState = (EC_FindThinker(gs, ent))->state; }
    if (ent->componentBits & EC_BIT10_MULTI_RENDOBJ) { state->multiRendState = (EC_FindMultiRendObj(gs, ent))->state; }
    if (ent->componentBits & EC_BIT11_VOLUME) { state->volumeState = (EC_FindVolume(gs, ent))->state; }
    if (ent->componentBits & EC_BIT12_SENSOR) { state->sensorState = (EC_FindSensor(gs, ent))->state; }
}

internal u8 Ent_PrepareSpawnCmd(GameState* gs, i32 factoryType, EntityState* target, EntitySpawnOptions* options)
{
    *target = {};
    EntId entId = Ent_ReserveFreeEntity(&gs->entList);
    
    i32 result = Ent_CopyTemplate(factoryType, target, options);
    target->entId = entId;
    Ent_ApplySpawnOptions(target, options);
    return (u8)result;
}

internal EntId Ent_QuickSpawnCmd(GameState* gs, i32 factoryType, EntitySpawnOptions* options)
{
    EntityState s = {};
    EntId entId = Ent_ReserveFreeEntity(&gs->entList);
    if (Ent_CopyTemplate(factoryType, &s, options))
    {
        s.entId = entId;
        Ent_ApplySpawnOptions(&s, options);
        Ent_WriteEntityStateCmd(NULL, &s);
    }
    return entId;
}
