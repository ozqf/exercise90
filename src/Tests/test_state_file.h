#pragma once

#include "../common/com_module.h"

struct FileSegment
{
	u32 offset = 0;
	u32 count = 0;
	u32 size = 0;
};

struct ReplayFrameHeader
{
    u32 frameNumber;
    u32 size;
	char label[16];
};

struct StateSaveHeader
{
	char magic[4] = { 'S', 'A', 'V', 'E' };
	char baseFile[32];

	FileSegment staticEntities;
	FileSegment dynamicEntities;
	FileSegment frames;
};

struct CmdHeader
{
	u32 type;
	u32 size;
};

#define CMD_TYPE_SPAWN 100
//#define CMD_TYPE_SPAWN_WORLD_CUBE 101

#define ENTITY_TYPE_WORLD_NULL 0
#define ENTITY_TYPE_WORLD_CUBE 1
#define ENTITY_TYPE_RIGIDBODY_CUBE 2

union EntId
{
    struct
    {
        u16 iteration;
        u16 index;
    };
    u16 arr[2];
};

// 100
struct Cmd_Spawn
{
    i32 factoryType;
	EntId entityId;
    Vec3 pos;
    Vec3 rot;
    Vec3 size;
    u32 flags;
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

inline void DebugStateHeader(StateSaveHeader* h)
{
	FileSeg_PrintDebug("Static Entities", &h->staticEntities);
	FileSeg_PrintDebug("Dynamic Entities", &h->dynamicEntities);
	FileSeg_PrintDebug("Frames", &h->frames);
}

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

	u32 cmdSpawnSize = sizeof(CmdHeader) + sizeof(Cmd_Spawn);

	u16 id = 0;

	Cmd_Spawn spawn;
	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 0;
	spawn.pos.y = -8;
	spawn.pos.z = 0;
	spawn.size = { 48, 1, 48 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 0;
	spawn.pos.y = 8;
	spawn.pos.z = 0;
	spawn.size = { 48, 1, 48 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 0;
	spawn.pos.y = 0;
	spawn.pos.z = -24;
	spawn.size = { 48, 16, 1 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 0;
	spawn.pos.y = 0;
	spawn.pos.z = 24;
	spawn.size = { 48, 16, 1 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = -24;
	spawn.pos.y = 0;
	spawn.pos.z = 0;
	spawn.size = { 1, 16, 48 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticEntities, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 24;
	spawn.pos.y = 0;
	spawn.pos.z = 0;
	spawn.size = { 1, 16, 48 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
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
			Cmd_Spawn s = {};
			fread(&s, sizeof(Cmd_Spawn), 1, f);
			printf("Spawn %d at %.2f, %.2f, %.2f\nRot: %.2f, %.2f, %.2f\n",
				s.factoryType,
				s.pos.x, s.pos.y, s.pos.z,
				s.rot.e[0], s.rot.e[1], s.rot.e[2]
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

void Test_ReadCommandBuffer(ByteBuffer* bytes)
{
	u8* read = bytes->ptrStart;
	while (read < bytes->ptrEnd)
	{
		CmdHeader cheader = {};
		//fread(&cheader, sizeof(CmdHeader), 1, f);
		read += COM_COPY_STRUCT(read, &cheader, CmdHeader);
		switch (cheader.type)
		{
			case CMD_TYPE_SPAWN:
			{
				Cmd_Spawn s = {};
				//fread(&s, sizeof(Cmd_Spawn), 1, f);rt
				read += COM_COPY_STRUCT(read, &s, Cmd_Spawn);
				printf("Spawn %d at %.2f, %.2f, %.2f\nRot: %.2f, %.2f, %.2f\n",
					s.factoryType,
					s.pos.e[0], s.pos.e[1], s.pos.e[2],
					s.rot.e[0], s.rot.e[1], s.rot.e[2]
				);
			} break;

			default:
			{
				printf("Unknown cmd type %d. Skipping %d bytes\n", cheader.type, cheader.size);
				Assert(cheader.size > 0);
				read += cheader.size;
			}
		}
	}
}

u32 GetFileSizeAndSeekToStart(FILE* f)
{
	fseek(f, 0, SEEK_END);
	u32 result = ftell(f);
	fseek(f, 0, SEEK_SET);
	return result;
}

u8 IsHeaderValid(StateSaveHeader* h)
{
	
	if (
		h->magic[0] != 'S'
		|| h->magic[1] != 'A'
		|| h->magic[2] != 'V'
		|| h->magic[3] != 'E'
		)
	{
		return 0;
	}
	return 1;
}

void Test_PrintStateFileScan(char* filePath)
{
	printf("*** State Save File %s ***\n", filePath);
	FILE* f;
	fopen_s(&f, filePath, "rb");
	if (f == NULL)
	{
		printf("  File not found\n");
		return;
	}
	StateSaveHeader h = {};
	u32 fileSize = GetFileSizeAndSeekToStart(f);
	fread(&h, sizeof(StateSaveHeader), 1, f);
	if (!IsHeaderValid(&h))
	{
		printf("  %s is not a State save file\n", filePath);
		return;
	}

	printf("File is %d bytes\n", fileSize);

	// Static Commands
	if (h.staticEntities.size > 0)
	{
		printf("* Scanning static commands\n");
		u32 origin = h.staticEntities.offset;
		fseek(f, origin, SEEK_SET);
		u32 read = 0;
		i32 cmdCount = 0;
		while (read < h.staticEntities.size)
		{
			CmdHeader cmdH = {};
			fread(&cmdH, sizeof(CmdHeader), 1, f);
			read += sizeof(CmdHeader);
			printf("  CMD %d: type %d size %d\n", cmdCount, cmdH.type, cmdH.size);
			read += cmdH.size;
			cmdCount++;
			fseek(f, origin + read, SEEK_SET);
		}
	}
	else
	{
		printf("* No static commands found\n");
	}
	
	// Dynamic Commands
	if (h.dynamicEntities.size > 0)
	{
		printf("* Scanning dynamic commands\n");
		u32 origin = h.dynamicEntities.offset;
		fseek(f, origin, SEEK_SET);
		u32 read = 0;
		u32 cmdCount = 0;
		while (read < h.dynamicEntities.size)
		{
			CmdHeader cmdH = {};
			fread(&cmdH, sizeof(CmdHeader), 1, f);
			read += sizeof(CmdHeader);
			printf("  CMD %d: type %d size %d\n", cmdCount, cmdH.type, cmdH.size);
			read += cmdH.size;
			cmdCount++;
			fseek(f, origin + read, SEEK_SET);
		}
		if (!(cmdCount == h.dynamicEntities.count))
		{
			printf("*** CMD Count mismatch! Claimed: %d Actual: %d", h.dynamicEntities.count, cmdCount);
		}
	}
	else
	{
		printf("* No dynamic commands found\n");
	}
	
	i32 sizeOfReplayHeader = sizeof(ReplayFrameHeader);
	i32 framesOffset = h.dynamicEntities.offset + h.dynamicEntities.size;
	i32 totalFramesData = fileSize - framesOffset;
	u32 position = framesOffset;
	printf("%d bytes of frame data\n", totalFramesData);
	fseek(f, framesOffset, SEEK_SET);


	while (position < fileSize)
	{
		ReplayFrameHeader replay = {};
		fread(&replay, sizeOfReplayHeader, 1, f);
		position += sizeOfReplayHeader;
		position += replay.size;
		fseek(f, position, SEEK_SET);
		if (replay.size > 0)
		{
			printf("Demo header %s: frameNumber %d size %d\n", replay.label, replay.frameNumber, replay.size);
		}
	}
	

	DebugStateHeader(&h);
	fclose(f);
}

ByteBuffer Test_LoadEntireFile(char* filePath)
{
	ByteBuffer bytes = {};
	FILE* f;
	fopen_s(&f, filePath, "rb");
	if (f == NULL)
	{
		return bytes;
	}

	bytes.capacity = GetFileSizeAndSeekToStart(f);
	fseek(f, 0, SEEK_SET);
	void* ptr = malloc(bytes.capacity);
	bytes.ptrStart = (u8*)ptr;
	Assert(bytes.ptrStart != NULL);
	bytes.ptrEnd = bytes.ptrStart + bytes.capacity;

	fread(bytes.ptrStart, bytes.capacity, 1, f);

	fclose(f);

	return bytes;
}

u8 Test_LoadAndRun(char* filePath)
{
	ByteBuffer bytes = Test_LoadEntireFile(filePath);
	if (bytes.ptrStart == NULL)
	{
		return 1;
	}

	u8* ptr = bytes.ptrStart;
	StateSaveHeader h = {};
	ptr += COM_COPY_STRUCT(ptr, &h, StateSaveHeader);

	if (
		h.magic[0] != 'S'
		|| h.magic[1] != 'A'
		|| h.magic[2] != 'V'
		|| h.magic[3] != 'E'
		)
	{
		printf("File \"%s\" is not a SAVE file\n", filePath);

		// Cleanup
		free(bytes.ptrStart);
		return 1;
	}

	if (h.baseFile[0] != NULL)
	{
		printf("Reading base file \"%s\"\n", h.baseFile);
		if (Test_LoadAndRun(h.baseFile))
		{
			printf("Error reading base file. aborting\n");
			free(bytes.ptrStart);
			return 1;
		}
		printf("Read base file, continuing...\n\n");
	}
	else
	{
		printf("No base file\n");
	}

	printf("*** CONTENTS OF \"%s\"\n", filePath);
	DebugStateHeader(&h);

	// Read static entities file section
	ByteBuffer sub = {};
	sub.ptrStart = bytes.ptrStart + h.staticEntities.offset;
	sub.capacity = h.staticEntities.size;
	sub.count = h.staticEntities.count;
	sub.ptrEnd = bytes.ptrStart + bytes.capacity;

	Test_ReadCommandBuffer(&sub);

	// Cleanup
	free(bytes.ptrStart);
	return 0;
}

void Test_StateSaving()
{
	//Test_WriteStateFile("base\\testbox.lvl", NULL);
	//Test_WriteStateFile("map2.lvl", "map1.lvl");

	Test_PrintStateFileScan("base\\demo.dem");

	//Test_PrintStateFileScan("base\\foo3");
	//Test_PrintStateFileScan("foo2");

	//printf("\nLOAD %s\n\n", "map2.lvl");
	//Test_LoadAndRun("map2.lvl");

	//printf("---------------------------------------\nWrite completed. Reading...\n\n");
	//Test_ReadState("map1.lvl", 0);
	//Test_ReadState("map2.lvl", 0);
}
