#pragma once

#include "../game.h"


void Ent_SetTemplate_Volume(EntityState* state, EntitySpawnOptions* options)
{
    printf("SET TEMPLATE Volume\n");
    state->entMetaData.factoryType = ENTITY_TYPE_VOLUME;

    state->componentBits |= EC_FLAG_TRANSFORM;

    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited(
        "Cube",
        state->renderState.meshName,
        EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited(
        "textures\\white.bmp",
        state->renderState.textureName,
        EC_RENDERER_STRING_LENGTH);
    state->renderState.colourRGB[0] = 1;
    state->renderState.colourRGB[1] = 1;
    state->renderState.colourRGB[2] = 0;

    state->componentBits |= EC_FLAG_VOLUME;
    EC_VolumeState* s = &state->volumeState;

    u32 flags = ZCOLLIDER_FLAG_STATIC |
        ZCOLLIDER_FLAG_NO_COLLISION_RESPONSE |
        ZCOLLIDER_FLAG_VERY_INTERESTING;
        
    state->componentBits |= EC_FLAG_COLLIDER;
    state->colliderState.def.SetAsBox(
        state->transform.pos.x, state->transform.pos.y, state->transform.pos.z,
        0.5f, 0.5f, 0.5f, 
        flags,
        COLLISION_LAYER_VOLUME,
        COL_MASK_VOLUME,
        0
    );
	
	state->componentBits |= EC_FLAG_SENSOR;
	state->sensorState.listenerComponents |= EC_FLAG_VOLUME;
    
    Ent_ApplySpawnOptions(state, options);
}

internal void EC_VolumeHandleOverlap(
	GameState* gs,
	GameTime* time,
	EC_Volume* vol,
	Ent* self,
	Ent* target)
{
    if (!Game_AttackIsValid(self->team, target->team)) { return; }
    
	EC_Health* hp = EC_FindHealth(gs, target);
	if (hp)
	{
		hp->state.hp -= 1;
        hp->state.damageThisFrame += 1;
        hp->state.lastHitFrame = time->gameFrameNumber + 1;
        hp->state.lastHitSource = self->entId;
	}
}


internal void Game_UpdateVolumes(GameState* gs, GameTime* time)
{
    for (i32 i = 0; i < GAME_MAX_ENTITIES; ++i)
    {
        EC_Volume* vol = &gs->volumeList.items[i];
        if (!vol->header.inUse) { continue; }
		
		if (vol->state.moveType == 1)
		{
			i32 shapeId = EC_FindCollider(gs, &vol->header.entId)->shapeId;
			PhysCmd_TeleportShape(shapeId, 0, 0, 0);
		}
    }
}
