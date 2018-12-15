#pragma once

#include "game.h"


void Ent_ApplySpawnOptions(EntityState* state, EntitySpawnOptions* options)
{
	if (options == NULL) { return; }
    // TODO: More tidy way to handle scale issues...?
    //if (options->scale.x <= 0) { options->scale.x = 1; /*printf("  SpawnOptions had scaleX <= 0, forced to 1\n");*/ }
    //if (options->scale.y <= 0) { options->scale.y = 1; /*printf("  SpawnOptions had scaleY <= 0, forced to 1\n");*/ }
    //if (options->scale.z <= 0) { options->scale.z = 1; /*printf("  SpawnOptions had scaleZ <= 0, forced to 1\n");*/ }

    state->entMetaData.source = options->source;
    // if (state->source.value != 0)
    // {
    //     printf("  Ent source: %d/%d\n", state->source.iteration, state->source.index);
    // }
    if (options->flags & ENT_OPTION_FLAG_TEAM)
    {
        state->entMetaData.team = options->team;
    }

    if (state->componentBits & EC_BIT1_TRANSFORM)
    {
        //printf("  EC Apply transform options\n");
        Transform_SetByPosAndDegrees(&state->transform, &options->pos, &options->rot);
        //if (options->scale.x != 0 && options->scale.y != 0 && options->scale.z != 0)
        if (options->flags & ENT_OPTION_FLAG_SCALE)
        {
            Transform_SetScale(&state->transform, options->scale.x, options->scale.y, options->scale.z);
        }
        
    }
    if (state->componentBits & EC_BIT3_COLLIDER)
    {
        state->colliderState.def.pos[0] = options->pos.x;
        state->colliderState.def.pos[1] = options->pos.y;
        state->colliderState.def.pos[2] = options->pos.z;

        if (state->componentBits & ENT_OPTION_FLAG_SCALE)
        {
            if (options->scale.x == 0 || options->scale.y == 0 || options->scale.z == 0)
            {
                printf("CANNOT APPLY SCALE TO %d/%d An axis is zero!\n",
                    state->entId.iteration, state->entId.index
                );
            }
            else
            {
                printf("APPLY SCALE %.2f/%.2f/%.2f to ent %d/%d\n",
                    options->scale.x, options->scale.y, options->scale.z,
                    state->entId.iteration, state->entId.index
                );
                state->colliderState.def.SetScale(
                    options->scale.x * 0.5f,
                    options->scale.y * 0.5f,
                    options->scale.z * 0.5f
                );
            }
        }
    }
    if (state->componentBits & EC_BIT6_PROJECTILE &&
        options->flags & ENT_OPTION_FLAG_VELOCITY)
    {
        state->projectileState.move = options->vel;
        // printf("PRJ Options Vel: %.2f, %.2f, %.2f\n",
        //     state->projectileState.move.x, state->projectileState.move.y, state->projectileState.move.z
        // );
    }
}

///////////////////////////////////////////////////////////////////////////////////
// APPLY SPECIFIC COMPONENT STATE
///////////////////////////////////////////////////////////////////////////////////
internal void EC_TransformApplyState(GameScene* gs, Ent* ent, Transform* transform)
{
    EC_Transform* ecT = EC_FindTransform(gs, &ent->entId);
	if (ecT == NULL)
	{
		ecT = EC_AddTransform(gs, ent);
        if (gs->Verbose())
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

internal void EC_SingleRendObjApplyState(GameScene* gs, Ent* ent, EC_RendObjState* state)
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

internal void EC_MultiRendObjApplyState(GameScene* gs, Ent* ent, EC_MultiRendObjState* state)
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

internal void EC_ColliderApplyState(GameScene* gs, Ent* ent, EC_ColliderState* state)
{
    EC_Collider* col = EC_FindCollider(gs, ent);
    if (col == NULL)
    {
        if (gs->Verbose())
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

internal void EC_ThinkerApplyState(GameScene* gs, Ent* ent, EC_ThinkerState* state)
{
    EC_Thinker* thinker = EC_FindThinker(gs, ent);
    if (thinker == NULL)
    {
        thinker = EC_AddThinker(gs, ent);
    }
    thinker->state = *state;
}

internal void EC_ActorMotorApplyState(GameScene* gs, Ent* ent, EC_ActorMotorState* state)
{
    EC_ActorMotor* motor = EC_FindActorMotor(gs, ent);
    if (motor == NULL)
    {
        motor = EC_AddActorMotor(gs, ent);
    }
    motor->state = *state;
}

internal void EC_AIControllerApplyState(GameScene* gs, Ent* ent, EC_AIState* state)
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

internal void EC_HealthApplyState(GameScene* gs, Ent* ent, EC_HealthState* state)
{
    EC_Health* hp = EC_FindHealth(gs, ent);
    if (hp == NULL)
    {
        hp = EC_AddHealth(gs, ent);
        hp->state = *state;
    }
}

internal void EC_ProjectileApplyState(GameScene* gs, Ent* ent, EC_ProjectileState* state)
{
    EC_Projectile* prj = EC_FindProjectile(gs, ent);
    if (prj == NULL)
    {
        prj = EC_AddProjectile(gs, ent);
    }
    prj->state = *state;
}

internal void EC_LabelApplyState(GameScene* gs, Ent* ent, EC_LabelState* state)
{
    EC_Label* label = EC_FindLabel(gs, ent);
    if (label == NULL)
    {
        label = EC_AddLabel(gs, ent);
    }
    COM_CopyStringLimited(state->label, label->state.label, EC_LABEL_LENGTH);
}

internal void EC_ApplyEntityMetaData(GameScene* gs, Ent* ent, Ent* entState)
{
    // TODO: Move these data items into an 'EC_MetaData' component
    // In theory safe to copy everything but being careful
    // In use and Id should be handled locally
    //ent->inUse = entState->inUse;

    ent->tag = entState->tag;
    ent->team = entState->team;
    ent->source = entState->source;
    ent->factoryType = entState->factoryType;
    ent->componentBits = entState->componentBits;
}

internal void EC_VolumeApplyState(GameScene* gs, Ent* ent, EC_VolumeState* state)
{
    EC_Volume* vol = EC_FindVolume(gs, ent);
    if (vol == NULL)
    {
        vol = EC_AddVolume(gs, ent);
    }
    vol->state = *state;
}

internal void EC_SensorApplyState(GameScene* gs, Ent* ent, EC_SensorState* state)
{
    EC_Sensor* sensor = EC_FindSensor(gs, ent);
    if (sensor == NULL)
    {
        sensor = EC_AddSensor(gs, ent);
    }
    sensor->state = *state;
}
