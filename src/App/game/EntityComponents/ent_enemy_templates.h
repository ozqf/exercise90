#pragma once

#include "game.h"

internal void EC_SetRendObjStateDefault(EC_RendObjState* rendObj, f32 red, f32 green, f32 blue)
{
    COM_CopyStringLimited(
        "Cube",
        rendObj->meshName, EC_RENDERER_STRING_LENGTH
    );
    COM_CopyStringLimited(
        "textures\\white_bordered.bmp",
        rendObj->textureName, EC_RENDERER_STRING_LENGTH
    );
    rendObj->colourRGB[0] = red;
    rendObj->colourRGB[1] = green;
    rendObj->colourRGB[2] = blue;
}

internal void Ent_SetTemplate_GenericEnemy(EntityState* state, EntitySpawnOptions* options)
{
    state->componentBits |= EC_FLAG_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE_ENEMY;
    state->entMetaData.team = TEAM_ENEMIES;
    printf("  TEMPLATES spawn enemy %d/%d\n", state->entId.iteration, state->entId.index);

    // apply defaults
    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_FLAG_AICONTROLLER;
    //state->aiState.ticker.tickMax = 0.033f;
    state->aiState.minApproachDistance = 25.0f;
    state->aiState.atkSettings.Set(2, 0.3f, 0.0f, 1, 1.0f, ATTACK_FLAG_NO_TARGET_TRACK);
    
    //state->componentBits |= EC_FLAG_RENDERER;
    //COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    //COM_CopyStringLimited("textures\\white_bordered.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
    //state->renderState.colourRGB[0] = 1;
    //state->renderState.colourRGB[1] = 0;
    //state->renderState.colourRGB[2] = 0;

    #if 1
    state->componentBits |= EC_FLAG_MULTI_RENDOBJ;
    EC_SetRendObjStateDefault(&state->multiRendState.objStates[EC_MULTI_RENDOBJ_BASE], 1, 0, 0);
    state->multiRendState.GetBaseRendObj()->heightOffset = -0.5f;
    EC_SetRendObjStateDefault(&state->multiRendState.objStates[EC_MULTI_RENDOBJ_HEAD], 1, 0, 0);
    state->multiRendState.GetHeadRendObj()->heightOffset = 0.5f;
    //EC_SetRendObjStateDefault(&state->multiRendState.objStates[2]);
    #endif

    state->componentBits |= EC_FLAG_COLLIDER;
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        0.5f, 1.0f, 0.5f, 
        ZCOLLIDER_FLAG_GROUNDCHECK | ZCOLLIDER_FLAG_NO_ROTATION,
        COLLISION_LAYER_ACTOR,
        COL_MASK_ACTOR,
        0
    );

    state->componentBits  |= EC_FLAG_ACTORMOTOR;
    state->actorState.runSpeed = 6;
    state->actorState.runAcceleration = 75;
    state->actorState.ticker.tickMax = 1.0f;
    state->actorState.attackType = ATTACK_INDEX_SLOW_PROJECTILE;


    state->componentBits |= EC_FLAG_HEALTH;
    state->healthState.SetHealthAndMax(100);

    //Ent_ApplySpawnOptions(state, options);
}

internal void Ent_SetTemplate_Grunt(EntityState* state, EntitySpawnOptions* options)
{ 
    Ent_SetTemplate_GenericEnemy(state, options);
    Ent_ApplySpawnOptions(state, options);
}

internal void Ent_SetTemplate_Brute(EntityState* state, EntitySpawnOptions* options)
{
    Ent_SetTemplate_GenericEnemy(state, options);
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        1, 2, 1, 
        ZCOLLIDER_FLAG_GROUNDCHECK | ZCOLLIDER_FLAG_NO_ROTATION,
        COLLISION_LAYER_ACTOR,
        COL_MASK_ACTOR,
        0
    );
    state->transform.scale = { 2, 2, 2 };
    state->renderState.colourRGB[0] = 1;
    state->renderState.colourRGB[1] = 0;
    state->renderState.colourRGB[2] = 1;
    state->healthState.SetHealthAndMax(500);
    options->scale = { 2, 2, 2 };

    state->aiState.atkSettings.Set(2, 1.0f, 0.1f, 10, 1.0f, 0);
    
    state->actorState.runSpeed = 3;

    state->multiRendState.GetBaseRendObj()->heightOffset = -1.0f;
    state->multiRendState.GetHeadRendObj()->heightOffset = 1.0f;

    Ent_ApplySpawnOptions(state, options);
}

internal void Ent_SetTemplate_Charger(EntityState* state, EntitySpawnOptions* options)
{
    Ent_SetTemplate_GenericEnemy(state, options);
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        1, 1, 1, 
        ZCOLLIDER_FLAG_GROUNDCHECK | ZCOLLIDER_FLAG_NO_ROTATION,
        COLLISION_LAYER_ACTOR,
        COL_MASK_ACTOR,
        0
    );
    state->transform.scale = { 2, 2, 2 };
    state->renderState.colourRGB[0] = 1;
    state->renderState.colourRGB[1] = 0;
    state->renderState.colourRGB[2] = 1;
    options->scale = { 2, 2, 2 };

    state->healthState.SetHealthAndMax(250);
    state->healthState.stunThreshold = 1000;

    state->actorState.runSpeed = 7;

    state->aiState.minApproachDistance = 3.0f;
    state->aiState.type = 1;
    state->aiState.atkSettings.Set(-1, 0.5f, 0.0f, 1, 1.0f, 0);

    // Disable multi-render component
    state->componentBits &= ~EC_FLAG_MULTI_RENDOBJ;

    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited("textures\\white_bordered.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
    state->renderState.colourRGB[0] = 0.2f;
    state->renderState.colourRGB[1] = 0.2f;
    state->renderState.colourRGB[2] = 1;

    Ent_ApplySpawnOptions(state, options);
}

internal void Ent_SetTemplate_Swarm(EntityState* state, EntitySpawnOptions* options)
{
	Ent_SetTemplate_GenericEnemy(state, options);
	Ent_ApplySpawnOptions(state, options);
	
}

internal void Ent_SetTemplate_Fodder(EntityState* state, EntitySpawnOptions* options)
{
	Ent_SetTemplate_GenericEnemy(state, options);
	Ent_ApplySpawnOptions(state, options);
}

internal void Ent_SetTemplate_Spinner(EntityState* state, EntitySpawnOptions* options)
{
	Ent_SetTemplate_GenericEnemy(state, options);
	Ent_ApplySpawnOptions(state, options);
}

internal u8 Ent_SetTemplate_Enemy(EntityState* state, EntitySpawnOptions* options, i32 templateId)
{
    switch(templateId)
    {
        case ENTITY_TYPE_ENEMY: { Ent_SetTemplate_Grunt(state, options); } return 1;
        case ENTITY_TYPE_ENEMY_BRUTE: { Ent_SetTemplate_Brute(state, options); } return 1;
        case ENTITY_TYPE_ENEMY_CHARGER: { Ent_SetTemplate_Charger(state, options); } return 1;
		case ENTITY_TYPE_ENEMY_SWARM: { Ent_SetTemplate_Swarm(state, options); } return 1;
		case ENTITY_TYPE_ENEMY_FODDER: { Ent_SetTemplate_Fodder(state, options); } return 1;
		case ENTITY_TYPE_ENEMY_SPINNER: { Ent_SetTemplate_Spinner(state, options); } return 1;
        default: return 0;
    }
}
