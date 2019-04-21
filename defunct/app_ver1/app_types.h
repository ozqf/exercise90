#pragma once

#include "../common/common.h"

////////////////////////////////////////////////////////////////////
// ASSETS
////////////////////////////////////////////////////////////////////
#define MAX_TEXTURES 128
#define MAX_MESHES 128

struct TextureHandles
{
    Texture2DHeader textureHeaders[MAX_TEXTURES];
    BlockRef blockRefs[MAX_TEXTURES]; // IF allocated on the heap, find the reference here
    u32 numTextures = 0;
    u32 maxTextures = MAX_TEXTURES;
};

struct MeshHandle
{
    i32 id;
    char name[50];
    i32 loaded;
    MeshData data;
};

struct MeshHandles
{
    MeshHandle meshes[MAX_MESHES];
    u32 nextMesh = 0;
    u32 maxMeshes = MAX_MESHES;
};

enum ReplayMode
{
	NoReplayMode,
	RecordingReplay,
	PlayingReplay
};
