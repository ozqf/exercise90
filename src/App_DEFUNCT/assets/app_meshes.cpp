#pragma once

#include "../app_module.cpp"

i32 Assets_RegisterMesh(char* name, MeshData* mesh)
{
    Assert(g_meshHandles.nextMesh < g_meshHandles.maxMeshes)
    MeshHandle* handle = &g_meshHandles.meshes[g_meshHandles.nextMesh];
    
    COM_CopyStringLimited(name, handle->name, 50);
    handle->data = *mesh;
    handle->loaded = 1;
    handle->id = g_meshHandles.nextMesh;
    g_meshHandles.nextMesh++;
    return handle->id;
}

MeshData Assets_GetMeshDataByName(char* name)
{
    for (u32 i = 0; i < g_meshHandles.nextMesh; ++i)
    {
        if (!COM_CompareStrings(name, g_meshHandles.meshes[i].name))
        {
            return g_meshHandles.meshes[i].data;
        }
    }
    printf("APP Found no mesh %s\n", name);
    return g_meshCube;
}

void Assets_InitMeshDB()
{
    // Add common statically stored meshes
    Assets_RegisterMesh("Cube", &g_meshCube);
    Assets_RegisterMesh("Inverse_Cube", &g_meshInverseCube);
    Assets_RegisterMesh("Octahedron", &g_meshOctahedron);
    Assets_RegisterMesh("Quad", &g_meshPrimitive_quad);
}

#if 0
BlockRef Assets_LoadMeshOntoHeap(Heap* heap, Mesh* m)
{
    u32 totalSize = sizeof(Mesh) + COM_CalcSizeOfMesh(m);

    BlockRef r = {};
    Heap_Allocate(heap, &r, totalSize, )
}
#endif
