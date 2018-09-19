#pragma once

#include "../game.h"

/*
Spawn sequence with templates:
> Create a state struct.
> Set it to 'default' values for the specified entity template via 'SetTemplate'
> Patch values via 'options' data
> write spawn command to stream
*/ 

void Ent_SetTemplate_WorldCube(EntityState* state, EntitySpawnOptions* options, i32 factoryType)
{
    printf("GAME Spawn world cube template\n");
    // apply defaults
    state->componentBits |= EC_BIT0_ENTITY;
    state->entMetaData.factoryType = factoryType;

    state->componentBits |= EC_BIT1_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    if (factoryType == ENTITY_TYPE1_WORLD_CUBE)
    {
        state->componentBits |= EC_BIT2_RENDERER;
        COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
        COM_CopyStringLimited("textures\\COMP03_1.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
        state->renderState.colourRGB[0] = 1;
        state->renderState.colourRGB[1] = 1;
        state->renderState.colourRGB[2] = 1;
    }
    
    state->componentBits |= EC_BIT3_COLLIDER;
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        0.5f, 0.5f, 0.5f, 
        ZCOLLIDER_FLAG_STATIC,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEFAULT,
        0
    );

    Ent_ApplySpawnOptions(state, options);
}

void Ent_SetTemplate_RigidbodyCube(EntityState* state, EntitySpawnOptions* options)
{
    state->componentBits |= EC_BIT0_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE2_RIGIDBODY_CUBE;
    
    state->componentBits |= EC_BIT1_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_BIT2_RENDERER;
    COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited("textures\\white_bordered.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
    state->renderState.colourRGB[0] = 0;
    state->renderState.colourRGB[1] = 1;
    state->renderState.colourRGB[2] = 1;


    state->componentBits |= EC_BIT3_COLLIDER;
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        0.5f, 0.5f, 0.5f, 
        0,
        COLLISION_LAYER_DEBRIS,
        COL_MASK_DEBRIS,
        0
    );

    state->componentBits |= EC_BIT8_HEALTH;
    state->healthState.hp = 100;

    Ent_ApplySpawnOptions(state, options);
}

//////////////////////////////////////////////////////////////////////////
// ENEMY
//////////////////////////////////////////////////////////////////////////
void Ent_SetTemplate_Actor(EntityState* state, EntitySpawnOptions* options)
{
    state->componentBits |= EC_BIT0_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE3_ACTOR_GROUND;
    state->entMetaData.team = TEAM_PLAYERS;
    
    // apply defaults
    state->componentBits |= EC_BIT1_TRANSFORM;
    Transform_SetToIdentity(&state->transform);


    state->componentBits |= EC_BIT3_COLLIDER;
    f32 playerHeight = 1.85f; // average male height in metres
	f32 playerWidth = 0.8f; //0.46f; // reasonable shoulder width?
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        playerWidth / 2, playerHeight / 2, playerWidth / 2, 
        ZCOLLIDER_FLAG_NO_ROTATION | ZCOLLIDER_FLAG_INTERESTING,
        COLLISION_LAYER_ACTOR,
        COL_MASK_ACTOR,
        0
    );

    #if 1
    state->componentBits |= EC_BIT10_MULTI_RENDOBJ;
    EC_SetRendObjStateDefault(&state->multiRendState.objStates[EC_MULTI_RENDOBJ_BASE], 0, 0, 1);
    state->multiRendState.GetBaseRendObj()->heightOffset = -0.5f;
    EC_SetRendObjStateDefault(&state->multiRendState.objStates[EC_MULTI_RENDOBJ_HEAD], 0, 0, 1);
    state->multiRendState.GetHeadRendObj()->heightOffset = 0.5f;
    //EC_SetRendObjStateDefault(&state->multiRendState.objStates[2]);
    #endif

    state->componentBits  |= EC_BIT5_ACTORMOTOR;
    state->actorState.runSpeed = 16;
    state->actorState.runAcceleration = 150;

    // Machine gun
    state->actorState.attack1Type = ATTACK_INDEX_FAST_PROJECTILE;
    state->actorState.attack1Reload = 0.05f;

    // shotgun
    state->actorState.attack2Type = ATTACK_INDEX_PROJECTILE_SHOTGUN;
	state->actorState.attack2Reload = 0.75f;
    state->actorState.attack2AnimStyle = 1;

    state->actorState.ticker.tickMax = 1;
    state->actorState.ticker.tick = 0;
    state->actorState.animStyle = 0;

    state->componentBits |= EC_BIT8_HEALTH;
    state->healthState.SetHealthAndMax(100);
    state->healthState.flags |= EC_STATE_FLAG_IS_PLAYER;
    
    state->componentBits |= EC_BIT12_SENSOR;

    Ent_ApplySpawnOptions(state, options);
}

void Ent_SetTemplate_Projectile(EntityState* state, EntitySpawnOptions* options)
{
    state->componentBits |= EC_BIT0_ENTITY;
    state->entMetaData.factoryType = ENTITY_TYPE4_PROJECTILE;

    state->componentBits |= EC_BIT1_TRANSFORM;
    Transform_SetToIdentity(&state->transform);
    Transform_SetScale(&state->transform, 0.45f, 0.45f, 0.45f);

    state->componentBits |= EC_BIT2_RENDERER;
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

    state->componentBits |= EC_BIT6_PROJECTILE;
    state->projectileState = {};
    state->projectileState.damage.damage = 10;
    state->projectileState.speed = 150;
    f32 timeToLive = 0.5f;
    i32 tocks = 1;
    state->projectileState.ticker = { timeToLive, timeToLive };
	if (options)
	{
		state->projectileState.move.x = options->vel.x;
		state->projectileState.move.y = options->vel.y;
		state->projectileState.move.z = options->vel.z;
	}

    Ent_ApplySpawnOptions(state, options);
}

void Ent_SetTemplate_Spawner(EntityState* state, EntitySpawnOptions* options)
{
    state->entMetaData.factoryType = ENTITY_TYPE5_SPAWNER;
    state->componentBits |= EC_BIT1_TRANSFORM;
    Transform_SetToIdentity(&state->transform);

    state->componentBits |= EC_BIT9_THINKER;
    EC_ThinkerState* s = &state->thinkerState;
    s->max = 4;
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

u8 Game_WriteSpawnTemplate(i32 factoryType, EntityState* state, EntitySpawnOptions* options);

internal EntityState g_entTemplates[32];


internal void Game_CreateEntityTemplates()
{
    EntityState* arr = g_entTemplates;
	Ent_SetTemplate_WorldCube(&arr[ENTITY_TYPE1_WORLD_CUBE], NULL, ENTITY_TYPE1_WORLD_CUBE);
    Ent_SetTemplate_RigidbodyCube(&arr[ENTITY_TYPE2_RIGIDBODY_CUBE], NULL);
    Ent_SetTemplate_Actor(&arr[ENTITY_TYPE3_ACTOR_GROUND], NULL);
    Ent_SetTemplate_Projectile(&arr[ENTITY_TYPE4_PROJECTILE], NULL);
    Ent_SetTemplate_Spawner(&arr[ENTITY_TYPE5_SPAWNER], NULL);
    Ent_SetTemplate_Grunt(&arr[ENTITY_TYPE6_ENEMY], NULL);
    Ent_SetTemplate_WorldCube(&arr[ENTITY_TYPE7_BLOCKING_VOLUME], NULL, ENTITY_TYPE7_BLOCKING_VOLUME);
    Ent_SetTemplate_Brute(&arr[ENTITY_TYPE8_ENEMY_BRUTE], NULL);
    Ent_SetTemplate_Charger(&arr[ENTITY_TYPE9_ENEMY_CHARGER], NULL);
    Ent_SetTemplate_Fodder(&arr[ENTITY_TYPE10_ENEMY_FODDER], NULL);
    Ent_SetTemplate_Swarm(&arr[ENTITY_TYPE11_ENEMY_SWARM], NULL);
    Ent_SetTemplate_Spinner(&arr[ENTITY_TYPE12_ENEMY_SPINNER], NULL);
    Ent_SetTemplate_Volume(&arr[ENTITY_TYPE13_VOLUME], NULL);
}

// Returns 0 if template Id isn't valid
internal i32 Ent_CopyTemplate(i32 templateId, EntityState* target, EntitySpawnOptions* options)
{
	if (templateId < 0 || templateId >= 32)
	{
		printf("GAME TemplateId %d is out of bounds\n", templateId);
		return 0;
	}
	*target = g_entTemplates[templateId];
    Ent_ApplySpawnOptions(target, options);
	return 1;
}

//typedef void (*Fn_EntFromTemplate)(EntityState* state, EntitySpawnOptions* options, i32 factoryType);
//internal Fn_EntFromTemplate g_templates[32];

// EntityState g_templates[64];
// i32 g_numTemplates = 0;
