#pragma once

#include "../common/com_module.h"

struct StateSaveHeader
{
    char magic[4] = { 'S', 'A', 'V', 'E' };
    char baseFile[32];
    u32 numStaticEntities = 0;
    u32 numDynamicEntities = 0;
    u32 numFrames = 0;
};

struct CmdSpawn
{
    i32 entityType;
    f32 pos[3];
};

void Test_WriteStateFile(char* fileName, char* baseFileName)
{
    printf("Write savestate\n");
    FILE* f;
    fopen_s(&f, fileName, "wb");
    if (f == NULL)
    {
        printf("Failed to open file %s for writing, aborted.\n", fileName);
        return;
    }
    // Step forward, write save info
    fseek(f, sizeof(StateSaveHeader), SEEK_SET);

    StateSaveHeader header = {};
    if (baseFileName != NULL)
    {
        COM_CopyStringLimited(baseFileName, header.baseFile, 16);
    }
    else
    {
        COM_ZeroMemory((u8*)header.baseFile, 32);
    }
    

    CmdSpawn spawn;
    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 1;
    spawn.pos[1] = 2;
    spawn.pos[2] = 3;
    header.numDynamicEntities++;
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);

    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 4;
    spawn.pos[1] = 5;
    spawn.pos[2] = 6;
    header.numDynamicEntities++;
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);

    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 7;
    spawn.pos[1] = 8;
    spawn.pos[2] = 9;
    header.numDynamicEntities++;
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);

    // step back, write header
    fseek(f, 0, SEEK_SET);
    
    fwrite(&header, sizeof(StateSaveHeader), 1, f);

    fseek(f, 0, SEEK_END);
    u32 end = ftell(f);
    printf("Wrote %d bytes (%d entities) to \"%s\"\n\n", end, header.numDynamicEntities, fileName);
    fclose(f);
}

u8 Test_ReadState(char* fileName, u8 staticEntitiesOnly)
{
    FILE* f;
    fopen_s(&f, fileName, "rb");
    if (f == NULL)
    {
        printf("Failed to open file %s for reading, aborted.\n", fileName);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    u32 end = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("Reading %d bytes from \"%s\". Static only: %d\n", end, fileName, staticEntitiesOnly);

    StateSaveHeader h = {};
    fread(&h, sizeof(StateSaveHeader), 1, f);
    if (
        h.magic[0] != 'S'
        || h.magic[1] != 'A'
        || h.magic[2] != 'V'
        || h.magic[3] != 'E'
    )
    {
        printf("File \"%s\" is not a SAVE file\n", fileName);
        fclose(f);
        return 2;
    }
    printf("Has %d entities\n", h.numDynamicEntities);
    if (h.baseFile[0] != NULL)
    {
        printf("Reading base file \"%s\"\n", h.baseFile);
        // TODO: Nothing to prevent circular references here
        if (Test_ReadState(h.baseFile, 1))
        {
            printf("Error reading base file. aborting\n");
            fclose(f);
            return 3;
        }
        printf("Read base file\n");
    }
    else
    {
        printf("No base file\n");
    }
    for(u32 i = 0; i < h.numDynamicEntities; ++i)
    {
        CmdSpawn s = {};
        fread(&s, sizeof(CmdSpawn), 1, f);
        printf("Spawn %d at %.2f, %.2f, %.2f\n", s.entityType, s.pos[0], s.pos[1], s.pos[2]);
    }

    fclose(f);
    return 0;
}

void Test_StateSaving()
{
    Test_WriteStateFile("map1.lvl", NULL);
    Test_WriteStateFile("map2.lvl", "map1.lvl");
    Test_ReadState("map2.lvl", 0);
}
