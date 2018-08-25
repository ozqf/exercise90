#pragma once

#include "../game.h"

// EntityState g_templates[64];
// i32 g_numTemplates = 0;
/*
Spawn sequence with templates:
> Create a state struct.
> Set it to 'default' values for the specified entity template via 'SetTemplate'
> Patch values via 'options' data
> write spawn command to stream
*/

void Ent_ApplySpawnOptions(EntityState* state, EntitySpawnOptions* options)
{
    // TODO: More tidy way to handle scale issues...?
    if (options->scale.x <= 0) { options->scale.x = 1; /*printf("  SpawnOptions had scaleX <= 0, forced to 1\n");*/ }
    if (options->scale.y <= 0) { options->scale.y = 1; /*printf("  SpawnOptions had scaleY <= 0, forced to 1\n");*/ }
    if (options->scale.z <= 0) { options->scale.z = 1; /*printf("  SpawnOptions had scaleZ <= 0, forced to 1\n");*/ }

    state->entMetaData.source = options->source;
    // if (state->source.value != 0)
    // {
    //     printf("  Ent source: %d/%d\n", state->source.iteration, state->source.index);
    // }

    if (state->componentBits & EC_FLAG_TRANSFORM)
    {
        //printf("  EC Apply transform options\n");
        Transform_SetByPosAndDegrees(&state->transform, &options->pos, &options->rot);
        Transform_SetScale(&state->transform, options->scale.x, options->scale.y, options->scale.z);
    }
    if (state->componentBits & EC_FLAG_COLLIDER)
    {
        state->colliderState.def.pos[0] = options->pos.x;
        state->colliderState.def.pos[1] = options->pos.y;
        state->colliderState.def.pos[2] = options->pos.z;
    }
}

void Ent_SetTemplate_WorldCube(EntityState* state, EntitySpawnOptions* options, i32 factoryType)
{
    printf("GAME Spawn world cube template\n");
    // apply defaults
    state->componentBits |= EC_FLAG_ENTITY;
    state->entMetaData.factoryType = factoryType;

    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    if (factoryType == ENTITY_TYPE_WORLD_CUBE)
    {
        state->componentBits |= EC_FLAG_RENDERER;
        COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
        COM_CopyStringLimited("textures\\COMP03_1.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
        state->renderState.colourRGB[0] = 1;
        state->renderState.colourRGB[1] = 1;
        state->renderState.colourRGB[2] = 1;
    }
    
    state->componentBits |= EC_FLAG_COLLIDER;
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        0.5f, 0.5f, 0.5f, 
        ZCOLLIDER_FLAG_STATIC,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEFAULT
    );

    Ent_ApplySpawnOptions(state, options);
}

void Ent_SetTemplate_RigidbodyCube(EntityState* state, EntitySpawnOptions* options)
{
    state->componentBits |= EC_FLAG_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE_RIGIDBODY_CUBE;
    
    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited("textures\\white_bordered.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
    state->renderState.colourRGB[0] = 0;
    state->renderState.colourRGB[1] = 1;
    state->renderState.colourRGB[2] = 1;


    state->componentBits |= EC_FLAG_COLLIDER;
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        0.5f, 0.5f, 0.5f, 
        0,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEBRIS
    );

    state->componentBits |= EC_FLAG_HEALTH;
    state->healthState.hp = 100;

    Ent_ApplySpawnOptions(state, options);
}

//////////////////////////////////////////////////////////////////////////
// ENEMY
//////////////////////////////////////////////////////////////////////////
#if 0
void Ent_SetTemplate_Enemy(EntityState* state, EntitySpawnOptions* options, i32 templateId)
{
    state->componentBits |= EC_FLAG_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE_ENEMY;
    printf("  TEMPLATES spawn enemy %d/%d\n", state->entId.iteration, state->entId.index);

    // apply defaults
    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_FLAG_AICONTROLLER;
    state->aiState.ticker.tickMax = 0.1f;
    
    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited("textures\\white_bordered.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
    state->renderState.colourRGB[0] = 1;
    state->renderState.colourRGB[1] = 0;
    state->renderState.colourRGB[2] = 0;

    state->componentBits |= EC_FLAG_COLLIDER;
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        0.5f, 0.5f, 0.5f, 
        ZCOLLIDER_FLAG_GROUNDCHECK,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEBRIS
    );

    state->componentBits  |= EC_FLAG_ACTORMOTOR;
    state->actorState.runSpeed = 10;
    state->actorState.runAcceleration = 75;


    state->componentBits |= EC_FLAG_HEALTH;
    state->healthState.hp = 100;

    Ent_ApplySpawnOptions(state, options);
}
#endif
void Ent_SetTemplate_Actor(EntityState* state, EntitySpawnOptions* options)
{
    state->componentBits |= EC_FLAG_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE_ACTOR_GROUND;
    
    // apply defaults
    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);


    state->componentBits |= EC_FLAG_COLLIDER;
    f32 playerHeight = 1.85f; // average male height in metres
	f32 playerWidth = 0.46f; // reasonable shoulder width?
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        playerWidth / 2, playerHeight / 2, playerWidth / 2, 
        ZCOLLIDER_FLAG_NO_ROTATION,
        COLLISION_LAYER_WORLD,
        COL_MASK_ACTOR
    );

    state->componentBits  |= EC_FLAG_ACTORMOTOR;
    state->actorState.runSpeed = 12;
    state->actorState.runAcceleration = 100;
    state->actorState.attackType = 1;
    state->actorState.ticker.tickMax = 0.05f;

    state->componentBits |= EC_FLAG_HEALTH;
    state->healthState.hp = 100;
    

    Ent_ApplySpawnOptions(state, options);
}

void Ent_SetTemplate_Projectile(EntityState* state, EntitySpawnOptions* options)
{
    state->componentBits |= EC_FLAG_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE_PROJECTILE;

    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);
    Transform_SetScale(&state->transform, 0.35f, 0.35f, 0.35f);

    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited(
        "Cube",
        state->renderState.meshName,
        EC_RENDERER_STRING_LENGTH
    );
    COM_CopyStringLimited(
        "textures\\white_bordered.bmp",
        state->renderState.textureName,
        EC_RENDERER_STRING_LENGTH
    );
    state->renderState.colourRGB[0] = 1;
    state->renderState.colourRGB[1] = 1;
    state->renderState.colourRGB[2] = 0;

    state->componentBits |= EC_FLAG_PROJECTILE;
    state->projectileState = {};
    state->projectileState.damage.damage = 10;
    state->projectileState.speed = 150;
    f32 timeToLive = 0.5f;
    i32 tocks = 1;
    state->projectileState.ticker = { timeToLive, timeToLive, tocks, tocks };

    state->projectileState.move.x = options->vel.x;
    state->projectileState.move.y = options->vel.y;
    state->projectileState.move.z = options->vel.z;

    Ent_ApplySpawnOptions(state, options);
}

void Ent_SetTemplate_Spawner(EntityState* state, EntitySpawnOptions* options)
{
    state->entMetaData.factoryType = ENTITY_TYPE_SPAWNER;
    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_FLAG_THINKER;
    EC_ThinkerState* s = &state->thinkerState;
    s->type = EC_THINKER_SPAWNER;
    s->ticker.tickMax = 1.5f;
    s->ticker.tick = 0.5f;//s->ticker.tickMax;

    Ent_ApplySpawnOptions(state, options);
    
    printf("GAME Create Spawner at %.2f, %.2f, %.2f\n",
        state->transform.pos.x,
        state->transform.pos.y,
        state->transform.pos.z
    );

}

#define ENT_SET_TEMPLATE(caseValue, func, state, options) case caseValue##: \
{ \
    func##(##state##, options##); \
} break;

#define ENT_SET_TEMPLATE_WITH_SUB_TYPES(caseValue, func, state, options, factoryType) \
case caseValue##: \
{ \
    func##(##state##, options##, factoryType##); \
} break;

u8 Game_WriteSpawnTemplate(i32 factoryType, EntityState* state, EntitySpawnOptions* options)
{
    if (Ent_SetTemplate_Enemy(state, options, factoryType))
    {
        return 1;
    }
    switch (factoryType)
    {
       ENT_SET_TEMPLATE_WITH_SUB_TYPES(ENTITY_TYPE_WORLD_CUBE, Ent_SetTemplate_WorldCube, state, options, factoryType);
       ENT_SET_TEMPLATE_WITH_SUB_TYPES(ENTITY_TYPE_BLOCKING_VOLUME, Ent_SetTemplate_WorldCube, state, options, factoryType);
       ENT_SET_TEMPLATE(ENTITY_TYPE_RIGIDBODY_CUBE, Ent_SetTemplate_RigidbodyCube, state, options);
       ENT_SET_TEMPLATE(ENTITY_TYPE_ACTOR_GROUND, Ent_SetTemplate_Actor, state, options);
       ENT_SET_TEMPLATE(ENTITY_TYPE_PROJECTILE, Ent_SetTemplate_Projectile, state, options);
       ENT_SET_TEMPLATE(ENTITY_TYPE_SPAWNER, Ent_SetTemplate_Spawner, state, options);
       //ENT_SET_TEMPLATE_WITH_SUB_TYPES(ENTITY_TYPE_ENEMY, Ent_SetTemplate_Enemy, state, options, factoryType);
       default: { printf("GAME Unknown ent factory type %d\n", factoryType); return 0; }
    }
    return 1;
}
