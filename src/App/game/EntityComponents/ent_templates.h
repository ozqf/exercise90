#pragma once

#include "../game.h"

EntityState g_templates[64];
i32 g_numTemplates = 0;

void Ent_SetTemplate_WorldCube(EntityState* state, EntitySpawnOptions* options)
{
    state->factoryType = ENTITY_TYPE_WORLD_CUBE;

}

void Ent_SetTemplate_RigidbodyCube(EntityState* state, EntitySpawnOptions* options)
{
    state->factoryType = ENTITY_TYPE_RIGIDBODY_CUBE;
}

void Ent_SetTemplate_Actor(EntityState* state, EntitySpawnOptions* options)
{
    state->factoryType = ENTITY_TYPE_ACTOR_GROUND;
}

void Ent_SetTemplate_Projectile(EntityState* state, EntitySpawnOptions* options)
{
    state->factoryType = ENTITY_TYPE_PROJECTILE;
}

void Ent_SetTemplate_Thinker(EntityState* state, EntitySpawnOptions* options)
{
    state->factoryType = ENTITY_TYPE_THINKER;
}

void Ent_ApplySpawnOptions(EntityState* state, EntitySpawnOptions* options)
{

}

#define ENT_SET_TEMPLATE(caseValue, func, state, options) case caseValue##: \
{ \
    func##(##state##, options##); \
} break;

void Game_WriteSpawnTemplate(i32 factoryType, EntityState* state, EntitySpawnOptions* options)
{
     switch (factoryType)
     {
        ENT_SET_TEMPLATE(ENTITY_TYPE_WORLD_CUBE, Ent_SetTemplate_WorldCube, state, options);
        ENT_SET_TEMPLATE(ENTITY_TYPE_RIGIDBODY_CUBE, Ent_SetTemplate_RigidbodyCube, state, options);
        ENT_SET_TEMPLATE(ENTITY_TYPE_ACTOR_GROUND, Ent_SetTemplate_Actor, state, options);
        ENT_SET_TEMPLATE(ENTITY_TYPE_PROJECTILE, Ent_SetTemplate_Projectile, state, options);
        ENT_SET_TEMPLATE(ENTITY_TYPE_THINKER, Ent_SetTemplate_Thinker, state, options);
        default: { printf("GAME Unknown ent factory type %d\n", factoryType); return; }
     }
}
