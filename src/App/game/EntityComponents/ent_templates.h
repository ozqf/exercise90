#pragma once

#include "../game.h"

EntityState g_templates[64];
i32 g_numTemplates = 0;

void Ent_SetTemplate_WorldCube(EntityState* state)
{
    
}

void Ent_SetTemplate_RigidbodyCube(EntityState* state)
{
    
}

void Ent_SetTemplate_ActorMotor(EntityState* state)
{
    
}

void Ent_SetTemplate_Projectile(EntityState* state)
{
    
}

void Ent_SetTemplate_Thinker(EntityState* state)
{
    
}

void Game_InitEntityTemplates()
{
     //Ent_SetTemplate_RigidbodyCube(&g_templates[ENTITY_TYPE_NULL]);
     g_templates[ENTITY_TYPE_NULL] = {};
     Ent_SetTemplate_WorldCube(&g_templates[ENTITY_TYPE_WORLD_CUBE]);
     Ent_SetTemplate_RigidbodyCube(&g_templates[ENTITY_TYPE_RIGIDBODY_CUBE]);
     Ent_SetTemplate_ActorMotor(&g_templates[ENTITY_TYPE_ACTOR_GROUND]);
     Ent_SetTemplate_RigidbodyCube(&g_templates[ENTITY_TYPE_PROJECTILE]);
     Ent_SetTemplate_RigidbodyCube(&g_templates[ENTITY_TYPE_THINKER]);
     Ent_SetTemplate_RigidbodyCube(&g_templates[ENTITY_TYPES_COUNT]);
}
