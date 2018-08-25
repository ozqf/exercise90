#pragma once

#include "game.h"

void EC_SetRendObjStateDefault(EC_RendObjState* rendObj, f32 red, f32 green, f32 blue)
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

void Ent_SetTemplate_GenericEnemy(EntityState* state, EntitySpawnOptions* options)
{
    state->componentBits |= EC_FLAG_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE_ENEMY;
    printf("  TEMPLATES spawn enemy %d/%d\n", state->entId.iteration, state->entId.index);

    // apply defaults
    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_FLAG_AICONTROLLER;
    state->aiState.ticker.tickMax = 0.1f;
    state->aiState.minApproachDistance = 5.0f;
    
    //state->componentBits |= EC_FLAG_RENDERER;
    //COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    //COM_CopyStringLimited("textures\\white_bordered.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
    //state->renderState.colourRGB[0] = 1;
    //state->renderState.colourRGB[1] = 0;
    //state->renderState.colourRGB[2] = 0;

    #if 1
    state->componentBits |= EC_FLAG_MULTI_RENDOBJ;
    EC_SetRendObjStateDefault(&state->multiRendState.objStates[0], 1, 0, 0);
    EC_SetRendObjStateDefault(&state->multiRendState.objStates[1], 1, 1, 1);
    //EC_SetRendObjStateDefault(&state->multiRendState.objStates[2]);
    #endif

    state->componentBits |= EC_FLAG_COLLIDER;
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        0.5f, 1.0f, 0.5f, 
        ZCOLLIDER_FLAG_GROUNDCHECK | ZCOLLIDER_FLAG_NO_ROTATION,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEBRIS
    );

    state->componentBits  |= EC_FLAG_ACTORMOTOR;
    state->actorState.runSpeed = 10;
    state->actorState.runAcceleration = 75;


    state->componentBits |= EC_FLAG_HEALTH;
    state->healthState.hp = 100;

    //Ent_ApplySpawnOptions(state, options);
}

void Ent_SetTemplate_Grunt(EntityState* state, EntitySpawnOptions* options)
{ 
    Ent_SetTemplate_GenericEnemy(state, options);
    Ent_ApplySpawnOptions(state, options);
}

void Ent_SetTemplate_Brute(EntityState* state, EntitySpawnOptions* options)
{
    Ent_SetTemplate_GenericEnemy(state, options);
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        1, 2, 1, 
        ZCOLLIDER_FLAG_GROUNDCHECK  | ZCOLLIDER_FLAG_NO_ROTATION,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEBRIS
    );
    state->transform.scale = { 2, 2, 2 };
    state->renderState.colourRGB[0] = 1;
    state->renderState.colourRGB[1] = 0;
    state->renderState.colourRGB[2] = 1;
    state->healthState.hp = 500;
    options->scale = { 2, 2, 2 };
    Ent_ApplySpawnOptions(state, options);
}

u8 Ent_SetTemplate_Enemy(EntityState* state, EntitySpawnOptions* options, i32 templateId)
{
    switch(templateId)
    {
        case ENTITY_TYPE_ENEMY: { Ent_SetTemplate_Grunt(state, options); } return 1;
        case ENTITY_TYPE_ENEMY_BRUTE: { Ent_SetTemplate_Brute(state, options); } return 1;
        default: return 0;
    }
}
