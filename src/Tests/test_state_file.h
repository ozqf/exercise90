#pragma once

#include "../common/com_module.h"

struct FileSegment
{
	u32 offset = 0;
	u32 count = 0;
	u32 size = 0;
};

struct CmdHeader
{
	u32 type;
	u32 size;
};

inline void FileSeg_Add(FileSegment* f, u32 fileSize)
{
	f->count++;
	f->size += fileSize;
};

inline void WriteCommandHeader(FILE* f, u32 type, u32 size)
{
	CmdHeader h;
	h.type = type;
	h.size = size;
	fwrite(&h, sizeof(CmdHeader), 1, f);
}

inline void FileSeg_PrintDebug(char* label, FileSegment* f)
{
	printf("%s - Count: %d, Offset: %d, Size: %d\n",
		label,
		f->count,
		f->offset,
		f->size
	);
}

struct StateSaveHeader
{
    char magic[4] = { 'S', 'A', 'V', 'E' };
    char baseFile[32];
	
    FileSegment staticEntities;
	FileSegment dynamicEntities;
	FileSegment frames;
};

inline void DebugStateHeader(StateSaveHeader* h)
{
	FileSeg_PrintDebug("Static Entities", &h->staticEntities);
	FileSeg_PrintDebug("Dynamic Entities", &h->dynamicEntities);
	FileSeg_PrintDebug("Frames", &h->frames);
}

#define CMD_TYPE_SPAWN 100
#define CMD_TYPE_SPAWN_WORLD_CUBE 101

#define ENTITY_TYPE_WORLD_NULL 0
#define ENTITY_TYPE_WORLD_CUBE 1
#define ENTITY_TYPE_RIGIDBODY_CUBE 2

struct GCmd_SpawnWorldCube
{
    Vec3 pos;
    Vec3 rot;
    Vec3 size;
    u32 flags;
};

struct CmdSpawn
{
    i32 entityType;
    f32 pos[3];
	f32 rot[3];
    u32 flags;
};

void Test_WriteStateFile(char* fileName, char* baseFileName)
{
    printf("Write savestate\n");
	printf("Size of State header: %d\n", sizeof(StateSaveHeader));
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
    
	/////////////////////////////////////////////////////////////
	// Write static entities
	/////////////////////////////////////////////////////////////
	header.staticEntities.offset = ftell(f);
	
	u32 cmdSpawnSize = sizeof(CmdHeader) + sizeof(GCmd_SpawnWorldCube);
	
    GCmd_SpawnWorldCube spawn;
    spawn = {};
    //spawn.entityType = 1;
    spawn.pos.e[0] = 1;
    spawn.pos.e[1] = 2;
    spawn.pos.e[2] = 3;
    spawn.size = { 48, 1, 48 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN_WORLD_CUBE, sizeof(GCmd_SpawnWorldCube));
    fwrite(&spawn, sizeof(GCmd_SpawnWorldCube), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

    spawn = {};
    //spawn.entityType = 1;
    spawn.pos.e[0] = 4;
    spawn.pos.e[1] = 5;
    spawn.pos.e[2] = 6;
    spawn.size = { 1, 16, 48 };
    WriteCommandHeader(f, CMD_TYPE_SPAWN_WORLD_CUBE, sizeof(GCmd_SpawnWorldCube));
    fwrite(&spawn, sizeof(GCmd_SpawnWorldCube), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

    spawn = {};
    //spawn.entityType = 1;
    spawn.pos.e[0] = 7;
    spawn.pos.e[1] = 8;
    spawn.pos.e[2] = 9;
	spawn.rot.e[1] = 2.34654546f;
    spawn.size = { 1, 16, 48 };
    WriteCommandHeader(f, CMD_TYPE_SPAWN_WORLD_CUBE, sizeof(GCmd_SpawnWorldCube));
    fwrite(&spawn, sizeof(GCmd_SpawnWorldCube), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

	/////////////////////////////////////////////////////////////
	// Write dynamic entities
	/////////////////////////////////////////////////////////////
	header.dynamicEntities.offset = ftell(f);
	header.dynamicEntities.count = 0;
	header.dynamicEntities.size = 0;
	
	/////////////////////////////////////////////////////////////
	// Frames
	/////////////////////////////////////////////////////////////
	header.frames.offset = ftell(f);
	header.frames.count = 0;
	header.frames.size = 0;
	
	/////////////////////////////////////////////////////////////
	// Finish
	/////////////////////////////////////////////////////////////
	
    // step back, write header
    fseek(f, 0, SEEK_SET);
    
    fwrite(&header, sizeof(StateSaveHeader), 1, f);

    fseek(f, 0, SEEK_END);
    u32 end = ftell(f);
    printf("Wrote %d bytes to \"%s\"\n\n", end, fileName);
	DebugStateHeader(&header);
	
    fclose(f);
}
#if 1
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
        printf("Read base file, continuing...\n\n");
    }
    else
    {
        printf("No base file\n");
    }
	
	DebugStateHeader(&h);
	
	fseek(f, h.staticEntities.offset, SEEK_SET);
	u32 sectionEnd = h.staticEntities.offset + h.staticEntities.size;
    
	while (ftell(f) < (i32)sectionEnd)
    {
		CmdHeader cheader = {};
		fread(&cheader, sizeof(CmdHeader), 1, f);
		switch (cheader.type)
		{
			case CMD_TYPE_SPAWN:
			{
				CmdSpawn s = {};
				fread(&s, sizeof(CmdSpawn), 1, f);
				printf("Spawn %d at %.2f, %.2f, %.2f\nRot: %.2f, %.2f, %.2f\n",
					s.entityType,
					s.pos[0], s.pos[1], s.pos[2],
					s.rot[0], s.rot[1], s.rot[2]
					);
			} break;

            case CMD_TYPE_SPAWN_WORLD_CUBE:
            {
                GCmd_SpawnWorldCube s = {};
                fread(&s, sizeof(GCmd_SpawnWorldCube), 1, f);
                printf("World Cube at %.2f, %.2f, %.2f\nSize: %.2f, %.2f, %.2f\n",
					s.pos.x, s.pos.y, s.pos.z,
					s.size.x, s.size.y, s.size.z
					);
            } break;
			
			default:
			{
				printf("Unknown cmd type %d. Skipping %d bytes\n", cheader.type, cheader.size);
                Assert(cheader.size > 0);
				fseek(f, ftell(f) + cheader.size, SEEK_SET);
			}
		}
    }

    fclose(f);
    return 0;
}
#endif
void Test_StateSaving()
{
    Test_WriteStateFile("map1.lvl", NULL);
    //Test_WriteStateFile("map2.lvl", "map1.lvl");
	printf("---------------------------------------\nWrite completed. Reading...\n\n");
    Test_ReadState("map1.lvl", 0);
    //Test_ReadState("map2.lvl", 0);
}
