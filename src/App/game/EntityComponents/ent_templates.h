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
    if (options->scale.x <= 0) { options->scale.x = 1; printf("  SpawnOptions had scaleX <= 0, forced to 1\n"); }
    if (options->scale.y <= 0) { options->scale.y = 1; printf("  SpawnOptions had scaleY <= 0, forced to 1\n"); }
    if (options->scale.z <= 0) { options->scale.z = 1; printf("  SpawnOptions had scaleZ <= 0, forced to 1\n"); }

    if (state->componentBits & EC_FLAG_TRANSFORM)
    {
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

// TODO: Setting EntId - done by the caller?

void Ent_SetTemplate_WorldCube(EntityState* state, EntitySpawnOptions* options)
{
    state->factoryType = ENTITY_TYPE_WORLD_CUBE;
    
    // apply defaults
    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited("textures\\COMP03_1.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);

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
    state->factoryType = ENTITY_TYPE_RIGIDBODY_CUBE;
    
    // apply defaults
    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited("textures\\W33_5.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);

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

void Ent_SetTemplate_Actor(EntityState* state, EntitySpawnOptions* options)
{
    ILLEGAL_CODE_PATH

    state->factoryType = ENTITY_TYPE_ACTOR_GROUND;


}

void Ent_SetTemplate_Projectile(EntityState* state, EntitySpawnOptions* options)
{
    state->factoryType = ENTITY_TYPE_PROJECTILE;

    state->componentBits |= EC_FLAG_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    
    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited("textures\\W33_5.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);

    state->componentBits |= EC_FLAG_PROJECTILE;
    state->projectileState = {};
    state->projectileState.damage.damage = 10;
    state->projectileState.speed = 100;

    // apply options
    //state->projectileState.ticker.tock = cmd->ticker.tock;
    //state->projectileState.ticker.tockMax = cmd->ticker.tockMax;
    //state->projectileState.ticker.tick = cmd->ticker.tick;
    //state->projectileState.ticker.tickMax = cmd->ticker.tickMax;
    state->projectileState.move.x = options->vel.x;
    state->projectileState.move.y = options->vel.y;
    state->projectileState.move.z = options->vel.z;
}

void Ent_SetTemplate_Thinker(EntityState* state, EntitySpawnOptions* options)
{
    ILLEGAL_CODE_PATH
    
    state->factoryType = ENTITY_TYPE_THINKER;
}

#define ENT_SET_TEMPLATE(caseValue, func, state, options) case caseValue##: \
{ \
    func##(##state##, options##); \
} break;

u8 Game_WriteSpawnTemplate(i32 factoryType, EntityState* state, EntitySpawnOptions* options)
{
     switch (factoryType)
     {
        ENT_SET_TEMPLATE(ENTITY_TYPE_WORLD_CUBE, Ent_SetTemplate_WorldCube, state, options);
        ENT_SET_TEMPLATE(ENTITY_TYPE_RIGIDBODY_CUBE, Ent_SetTemplate_RigidbodyCube, state, options);
        ENT_SET_TEMPLATE(ENTITY_TYPE_ACTOR_GROUND, Ent_SetTemplate_Actor, state, options);
        ENT_SET_TEMPLATE(ENTITY_TYPE_PROJECTILE, Ent_SetTemplate_Projectile, state, options);
        ENT_SET_TEMPLATE(ENTITY_TYPE_THINKER, Ent_SetTemplate_Thinker, state, options);
        default: { printf("GAME Unknown ent factory type %d\n", factoryType); return 0; }
     }
     return 1;
}
