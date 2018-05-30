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

#define STATE_CMD_NONE 0
#define STATE_CMD_SPAWN 1
#define STATE_CMD_SPAWN_STATIC 2

struct CmdSpawn
{
    i32 entityType;
    f32 pos[3];
	f32 rot[3];
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
	
	u32 cmdSpawnSize = sizeof(CmdHeader) + sizeof(CmdSpawn);
	
    CmdSpawn spawn;
    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 1;
    spawn.pos[1] = 2;
    spawn.pos[2] = 3;
	WriteCommandHeader(f, 1, sizeof(CmdSpawn));
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 4;
    spawn.pos[1] = 5;
    spawn.pos[2] = 6;
    WriteCommandHeader(f, 1, sizeof(CmdSpawn));
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

    spawn = {};
    spawn.entityType = 1;
    spawn.pos[0] = 7;
    spawn.pos[1] = 8;
    spawn.pos[2] = 9;
	spawn.rot[1] = 2.34654546f;
    WriteCommandHeader(f, 1, sizeof(CmdSpawn));
    fwrite(&spawn, sizeof(CmdSpawn), 1, f);
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
			case 1:
			{
				CmdSpawn s = {};
				fread(&s, sizeof(CmdSpawn), 1, f);
				printf("Spawn %d at %.2f, %.2f, %.2f\nRot: %.2f, %.2f, %.2f\n",
					s.entityType,
					s.pos[0], s.pos[1], s.pos[2],
					s.rot[0], s.rot[1], s.rot[2]
					);
			} break;
			
			default:
			{
				printf("Unknown cmd type %d. Skipping %d bytes\n", cheader.type, cheader.size);
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
    Test_WriteStateFile("map2.lvl", "map1.lvl");
	printf("---------------------------------------\nWrite completed. Reading...\n\n");
    Test_ReadState("map2.lvl", 0);
}
