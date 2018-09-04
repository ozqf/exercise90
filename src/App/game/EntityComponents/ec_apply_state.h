#pragma once

#include "game.h"

///////////////////////////////////////////////////////////////////////////////////
// APPLY COMPONENT STATE
///////////////////////////////////////////////////////////////////////////////////
internal void EC_TransformApplyState(GameState* gs, Ent* ent, Transform* transform)
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

internal void EC_RestoreRendObj(RendObj* rendObj, EC_RendObjState* state)
{
    RendObj_SetAsMesh(
        rendObj,
        Assets_GetMeshDataByName(state->meshName),// g_meshCube,
        state->colourRGB[0], state->colourRGB[1], state->colourRGB[2],
        AppGetTextureIndexByName(state->textureName)
    );
}

internal void EC_SingleRendObjApplyState(GameState* gs, Ent* ent, EC_RendObjState* state)
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

internal void EC_MultiRendObjApplyState(GameState* gs, Ent* ent, EC_MultiRendObjState* state)
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

internal void EC_ColliderApplyState(GameState* gs, Ent* ent, EC_ColliderState* state)
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
        col->shapeId = PhysCmd_CreateShape(
            &col->state.def,
            ent->entId.value
        );
    }
    else
    {
        // Restore state...
		// So who is setting the position here?
    }
}

internal void EC_ThinkerApplyState(GameState* gs, Ent* ent, EC_ThinkerState* state)
{
    EC_Thinker* thinker = EC_FindThinker(gs, ent);
    if (thinker == NULL)
    {
        thinker = EC_AddThinker(gs, ent);
    }
    thinker->state = *state;
}

internal void EC_ActorMotorApplyState(GameState* gs, Ent* ent, EC_ActorMotorState* state)
{
    EC_ActorMotor* motor = EC_FindActorMotor(gs, ent);
    if (motor == NULL)
    {
        motor = EC_AddActorMotor(gs, ent);
    }
    motor->state = *state;
}

internal void EC_AIControllerApplyState(GameState* gs, Ent* ent, EC_AIState* state)
{
    EC_AIController* ai = EC_FindAIController(gs, ent);
    if (ai == NULL)
    {
        ai = EC_AddAIController(gs, ent);
        EC_Transform* trans = EC_FindTransform(gs, ent);
        if (trans)
        {
            Transform* t = &trans->t;
            Game_SpawnLocalEntity(t->pos.x, t->pos.y, t->pos.z, NULL, 0, 2);
        }
    }
    ai->state = *state;
}

internal void EC_HealthApplyState(GameState* gs, Ent* ent, EC_HealthState* state)
{
    EC_Health* hp = EC_FindHealth(gs, ent);
    if (hp == NULL)
    {
        hp = EC_AddHealth(gs, ent);
        hp->state = *state;
    }
}

internal void EC_ProjectileApplyState(GameState* gs, Ent* ent, EC_ProjectileState* state)
{
    EC_Projectile* prj = EC_FindProjectile(gs, ent);
    if (prj == NULL)
    {
        prj = EC_AddProjectile(gs, ent);
    }
    prj->state = *state;
}

internal void EC_LabelApplyState(GameState* gs, Ent* ent, EC_LabelState* state)
{
    EC_Label* label = EC_FindLabel(gs, ent);
    if (label == NULL)
    {
        label = EC_AddLabel(gs, ent);
    }
    COM_CopyStringLimited(state->label, label->state.label, EC_LABEL_LENGTH);
}

internal void EC_ApplyEntityMetaData(GameState* gs, Ent* ent, Ent* entState)
{
    // TODO: Move these data items into an 'EC_MetaData' component
    // In theory safe to copy everything but being careful
    // In use and Id should be handled locally
    //ent->inUse = entState->inUse;

    ent->tag = entState->tag;
    ent->source = entState->source;
    ent->factoryType = entState->factoryType;
    ent->componentBits = entState->componentBits;
}
