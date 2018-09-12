#pragma once

#include "game.h"

#define WALL_TEXTURE "textures\\brbrick2.bmp"
#define SCIFI_TEXTURE "textures\\COMP03_1.bmp"

internal void Game_AddTestSolid(GameState* gs,
	f32 x, f32 y, f32 z,
	f32 halfSizeX, f32 halfSizeY, f32 halfSizeZ, u8 isVisible)
{
	u8* headerPos = App_StartCommandStream();
	u32 size = 0;
	Cmd_EntityStateHeader h = {};
	
	h.entId = Ent_ReserveFreeEntity(&gs->entList);
    h.componentBits |= EC_FLAG_ENTITY;
	h.componentBits |= EC_FLAG_TRANSFORM;
    if (isVisible) { h.componentBits |= EC_FLAG_RENDERER; }
    h.componentBits |= EC_FLAG_COLLIDER;

	size += App_WriteCommandBytesToFrameOutput((u8*)&h, sizeof(Cmd_EntityStateHeader));

    Ent ent = {};
    ent.entId = h.entId;
    ent.inUse = ENTITY_STATUS_IN_USE;
    if (isVisible)
    {
        ent.factoryType = ENTITY_TYPE_WORLD_CUBE;
    }
    else
    {
        ent.factoryType = ENTITY_TYPE_BLOCKING_VOLUME;
    }
    
    size += App_WriteCommandBytesToFrameOutput((u8*)&ent, sizeof(Ent));

    // create transform state
    Transform t = {};
    Transform_SetToIdentity(&t);
	t.pos.x = x;
    t.pos.y = y;
	t.pos.z = z;
	t.scale.x = halfSizeX * 2;
	t.scale.y = halfSizeY * 2;
	t.scale.z = halfSizeZ * 2;

    size += App_WriteCommandBytesToFrameOutput((u8*)&t, sizeof(Transform));
    
    // Wall texture "textures\\COMP03_1.bmp"
    // Metal texture "textures\\W33_5.bmp"

    u16 group;
    u16 mask;

    if (isVisible)
    {
        EC_RendObjState r = {};
        COM_CopyStringLimited("Cube", r.meshName, EC_RENDERER_STRING_LENGTH);
        COM_CopyStringLimited((WALL_TEXTURE), r.textureName, EC_RENDERER_STRING_LENGTH);
        r.colourRGB[0] = 1;
        r.colourRGB[1] = 1;
        r.colourRGB[2] = 1;
        size += App_WriteCommandBytesToFrameOutput((u8*)&r, sizeof(EC_RendObjState));
        group = COLLISION_LAYER_WORLD;
        mask = COL_MASK_DEFAULT;
    }
    else
    {
        group = COLLISION_LAYER_FENCE;
        mask = COL_MASK_DEFAULT;
    }

    // Create Collider
    EC_ColliderState col = {};
    col.def.SetAsBox(
        t.pos.x, t.pos.y, t.pos.z,
        halfSizeX, halfSizeY, halfSizeZ,
        ZCOLLIDER_FLAG_STATIC,
        group,
        mask,
        0
    );
    col.def.restitution = 1.0f;

    size += App_WriteCommandBytesToFrameOutput((u8*)&col, sizeof(EC_ColliderState));

    // Close command
    App_FinishCommandStream(headerPos, CMD_TYPE_ENTITY_STATE_2, 0, (u16)size);
    printf("  Add test solid: cmd stream type %d, size %d bytes\n", CMD_TYPE_ENTITY_STATE_2, size);
}

internal void Game_BuildTestScene_0(GameState* gs)
{
	Game_AddTestSolid(gs, 0, -6, 0, 24, 1, 24, 1);
	Game_AddTestSolid(gs, 0, 6, 0, 24, 1, 24, 0);

    Game_AddTestSolid(gs, 24, 0, 0, 1, 12, 24, 0);
    Game_AddTestSolid(gs, -24, 0, 0, 1, 12, 24, 0);

    Game_AddTestSolid(gs, 0, 0, 24, 24, 12, 1, 0);
    Game_AddTestSolid(gs, 0, 0, -24, 24, 12, 1, 0);
}

internal void Game_BuildTestScene_1(GameState* gs)
{
	Game_AddTestSolid(gs, 0, -6, 0, 24, 1, 24, 1);
	Game_AddTestSolid(gs, 0, 6, 0, 24, 1, 24, 1);

    Game_AddTestSolid(gs, 24, 0, 0, 1, 12, 24, 1);
    Game_AddTestSolid(gs, -24, 0, 0, 1, 12, 24, 1);

    Game_AddTestSolid(gs, 0, 0, 24, 24, 12, 1, 1);
    Game_AddTestSolid(gs, 0, 0, -24, 24, 12, 1, 1);
}

internal void Game_BuildTestScene(GameState* gs, i32 sceneIndex)
{
	switch (sceneIndex)
	{
		case 1:
		{
			Game_BuildTestScene_1(gs);
		} break;
		default:
		{
			Game_BuildTestScene_0(gs);
		} break;
	}

    //EntitySpawnOptions options;
    #if 0
    
    options = {};
    options.pos = { 20, 0, -20 };
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_SPAWNER, &options);
    options = {};
    options.pos = { 10, 0, -20 };
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_SPAWNER, &options);
    options = {};
    options.pos = { -10, 0, -20 };
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_SPAWNER, &options);
    options = {};
    options.pos = { -20, 0, -20 };
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_SPAWNER, &options);
    #endif

    #if 0
    EntitySpawnOptions options;
    options = {};
    options.pos = { 20, 0, 20 };
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_SPAWNER, &options);

    options = {};
    options.pos = { -20, 0, -20 };
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_SPAWNER, &options);

    options = {};
    options.pos = { -20, 0, 20 };
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_SPAWNER, &options);

    options = {};
    options.pos = { 20, 0, -20 };
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_SPAWNER, &options);
    #endif

    //EntitySpawnOptions options = {};
    //options = {};
    //Game_WriteSpawnCmd(gs, ENTITY_TYPE_ENEMY_CHARGER, &options);
    
}
