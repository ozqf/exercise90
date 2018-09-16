#pragma once

#include "../game.h"


void Ent_SetTemplate_Volume(EntityState* state, EntitySpawnOptions* options)
{
    printf("SET TEMPLATE Volume\n");
    state->entMetaData.factoryType = ENTITY_TYPE_VOLUME;

    state->componentBits |= EC_FLAG_TRANSFORM;

    state->componentBits |= EC_FLAG_RENDERER;
    COM_CopyStringLimited("Cube", state->renderState.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited("textures\\COMP03_1.bmp", state->renderState.textureName, EC_RENDERER_STRING_LENGTH);
    state->renderState.colourRGB[0] = 1;
    state->renderState.colourRGB[1] = 1;
    state->renderState.colourRGB[2] = 1;

    state->componentBits |= EC_FLAG_VOLUME;
    EC_VolumeState* s = &state->volumeState;
    
    Ent_ApplySpawnOptions(state, options);
}


internal void Game_UpdateVolumes(GameState* gs, GameTime* time)
{
    for (i32 i = 0; i < GAME_MAX_ENTITIES; ++i)
    {
        EC_Volume* vol = &gs->volumeList.items[i];
        if (!vol->header.inUse) { continue; }


    }
}
