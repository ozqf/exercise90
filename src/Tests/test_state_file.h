#pragma once

#include "../common/com_module.h"

struct StateSaveHeader
{
    char magic[4] = { 'S', 'A', 'V', 'E' };
    u32 numEntities = 0;
};

struct CmdSpawn
{
    i32 entityType;
    f32 pos[3];
};

void Test_WriteStateFile(char* fileName)
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

    CmdSpawn spawn;
    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 1;
    spawn.pos[1] = 2;
    spawn.pos[2] = 3;
    header.numEntities++;
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);

    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 4;
    spawn.pos[1] = 5;
    spawn.pos[2] = 6;
    header.numEntities++;
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);

    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 7;
    spawn.pos[1] = 8;
    spawn.pos[2] = 9;
    header.numEntities++;
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);

    // step back, write header
    fseek(f, 0, SEEK_SET);
    
    fwrite(&header, sizeof(StateSaveHeader), 1, f);

    fseek(f, 0, SEEK_END);
    u32 end = ftell(f);
    printf("Wrote %d bytes (%d entities) to \"%s\"\n\n", end, header.numEntities, fileName);
    fclose(f);
}

void Test_ReadState(char* fileName)
{
    FILE* f;
    fopen_s(&f, fileName, "rb");
    if (f == NULL)
    {
        printf("Failed to open file %s for reading, aborted.\n", fileName);
        return;
    }

    fseek(f, 0, SEEK_END);
    u32 end = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("Reading %d bytes from \"%s\"\n", end, fileName);

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
        return;
    }
    printf("Loading %d entities\n", h.numEntities);
    for(u32 i = 0; i < h.numEntities; ++i)
    {
        CmdSpawn s = {};
        fread(&s, sizeof(CmdSpawn), 1, f);
        printf("Spawn %d at %.2f, %.2f, %.2f\n", s.entityType, s.pos[0], s.pos[1], s.pos[2]);
    }

    fclose(f);
}

void Test_StateSaving()
{
    Test_WriteStateFile("map.lvl");
    Test_ReadState("map.lvl");
}
