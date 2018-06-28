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

	FileSegment staticCommands;
	FileSegment dynamicCommands;
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

struct CommandDescription
{
	i32 type;
	i32 size;
	char label[32];
};
#if 0
CommandDescription g_descriptions[] = {
	{ 0, { 'f', 'o', '/0' } },
	{ 1, { 'b', 'a', '/0' } }
};
#endif
#if 1
CommandDescription g_descriptions[32];
i32 g_numDescriptions = 0;
#endif

void Test_InitCmdDescription(CommandDescription* cmd, i32 type, i32 size, char* label)
{
	cmd->type = type;
	cmd->size = size;
	COM_CopyStringLimited(label, cmd->label, 32);
}

void Test_InitCmdDescriptions()
{
	Test_InitCmdDescription(&g_descriptions[0], 100, 48, "Spawn Static");
	Test_InitCmdDescription(&g_descriptions[1], 101, 8, "Server Impulse");
	// 16 bytes for input + 12 for ids etc
	Test_InitCmdDescription(&g_descriptions[2], 102, 28, "Player Input");
	Test_InitCmdDescription(&g_descriptions[3], 103, 28, "Client Update");

	Test_InitCmdDescription(&g_descriptions[4], 104, 5, "Text");
	Test_InitCmdDescription(&g_descriptions[5], 105, 84, "Dynamic Ent State");
	Test_InitCmdDescription(&g_descriptions[6], 106, 4, "Remove Ent");
	//Test_InitCmdDescription(&g_descriptions[7], 107, 4, "Ent Delta");
	Test_InitCmdDescription(&g_descriptions[7], 108, 12, "Player State");
	g_numDescriptions = 8;
}

CommandDescription* Test_GetCmdDescription(i32 type)
{
	for (i32 i = 0; i < g_numDescriptions; ++i)
	{
		if (g_descriptions[i].type == type)
		{
			return &g_descriptions[i];
		}
	}
	return NULL;
}

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
	FileSeg_PrintDebug("Static Entities", &h->staticCommands);
	FileSeg_PrintDebug("Dynamic Entities", &h->dynamicCommands);
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
	header.staticCommands.offset = ftell(f);

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
	FileSeg_Add(&header.staticCommands, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 0;
	spawn.pos.y = 8;
	spawn.pos.z = 0;
	spawn.size = { 48, 1, 48 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticCommands, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 0;
	spawn.pos.y = 0;
	spawn.pos.z = -24;
	spawn.size = { 48, 16, 1 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticCommands, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 0;
	spawn.pos.y = 0;
	spawn.pos.z = 24;
	spawn.size = { 48, 16, 1 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticCommands, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = -24;
	spawn.pos.y = 0;
	spawn.pos.z = 0;
	spawn.size = { 1, 16, 48 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticCommands, cmdSpawnSize);

	spawn = {};
	spawn.factoryType = 1;
	spawn.entityId = { 0, id++ };
	spawn.pos.x = 24;
	spawn.pos.y = 0;
	spawn.pos.z = 0;
	spawn.size = { 1, 16, 48 };
	WriteCommandHeader(f, CMD_TYPE_SPAWN, sizeof(Cmd_Spawn));
	fwrite(&spawn, sizeof(Cmd_Spawn), 1, f);
	FileSeg_Add(&header.staticCommands, cmdSpawnSize);

	/////////////////////////////////////////////////////////////
	// Write dynamic entities
	/////////////////////////////////////////////////////////////
	header.dynamicCommands.offset = ftell(f);
	header.dynamicCommands.count = 0;
	header.dynamicCommands.size = 0;

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
u8 Test_ReadState(char* fileName, u8 staticCommandsOnly)
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
	printf("Reading %d bytes from \"%s\". Static only: %d\n", end, fileName, staticCommandsOnly);

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

	fseek(f, h.staticCommands.offset, SEEK_SET);
	u32 sectionEnd = h.staticCommands.offset + h.staticCommands.size;

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
	if (h.staticCommands.size > 0)
	{
		printf("* Scanning static commands\n");
		u32 origin = h.staticCommands.offset;
		fseek(f, origin, SEEK_SET);
		u32 read = 0;
		i32 cmdCount = 0;
		while (read < h.staticCommands.size)
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
	if (h.dynamicCommands.size > 0)
	{
		printf("* Scanning dynamic commands\n");
		u32 origin = h.dynamicCommands.offset;
		fseek(f, origin, SEEK_SET);
		u32 read = 0;
		u32 cmdCount = 0;
		while (read < h.dynamicCommands.size)
		{
			CmdHeader cmdH = {};
			fread(&cmdH, sizeof(CmdHeader), 1, f);
			read += sizeof(CmdHeader);
			CommandDescription* def = Test_GetCmdDescription(cmdH.type);
			if (def != NULL)
			{
				printf("  %s (%d)\n", def->label, def->size);
			}
			else
			{
				printf("  CMD %d: type %d (UNKNOWN!) size %d\n", cmdCount, cmdH.type, cmdH.size);
			}
			
			read += cmdH.size;
			cmdCount++;
			fseek(f, origin + read, SEEK_SET);
		}
		if (!(cmdCount == h.dynamicCommands.count))
		{
			printf("*** CMD Count mismatch! Claimed: %d Actual: %d", h.dynamicCommands.count, cmdCount);
		}
	}
	else
	{
		printf("* No dynamic commands found\n");
	}
	
	i32 sizeOfReplayHeader = sizeof(ReplayFrameHeader);
	i32 framesOffset = h.dynamicCommands.offset + h.dynamicCommands.size;
	i32 totalFramesData = fileSize - framesOffset;
	u32 filePosition = framesOffset;
	u32 numFrames = 0;
	u32 numInterestingFrames = 0;

	printf("\n%d bytes of frame data:\n", totalFramesData);
	fseek(f, framesOffset, SEEK_SET);
	while (filePosition < fileSize)
	{
		ReplayFrameHeader replay = {};
		fread(&replay, sizeOfReplayHeader, 1, f);

		// Check file position isn't mangled
		Assert(COM_CompareStrings(replay.label, "FRAME") == 0);

		filePosition += sizeOfReplayHeader;
		numFrames++;
		if (replay.size > 0)
		{
			// cmd headers are 8 bytes
			// ---- 128 bytes can breakdown to ---
			// 8 byte header
			// 84 byte entity state
			// 8 byte header
			// 28 byte player input
			// ---- 312 bytes breakdown ---
			// 8 byte header
			// 84 byte entity state
			// 8 byte header
			// 84 byte entity state
			// 8 byte header
			// 84 byte entity state
			// 				- 276 so far
			// 8 byte header
			// 28 byte player input
			// therefore, stepping process is broken here!

			numInterestingFrames++;
			printf("%s %d (%d bytes)\n", replay.label, replay.frameNumber, replay.size);
			CmdHeader cmd = {};
			// Define start and end of frame data.
			// Step file position forward
			u32 frameStart = filePosition;
			u32 framePosition = frameStart;
			u32 endPosition = filePosition + replay.size;
			filePosition = endPosition;

			i32 overFlow = 0;
			i32 numCommands = 0;
			while (framePosition < endPosition)
			{
				fread(&cmd, sizeof(CmdHeader), 1, f);
				framePosition += sizeof(CmdHeader);
				//printf(" Positions: %d (%d - %d)\n", (framePosition - frameStart), framePosition, frameStart);
				//printf(" framePosition += %d\n", sizeof(CmdHeader));

				CommandDescription* def = Test_GetCmdDescription(cmd.type);
				if (def == NULL)
				{
					printf("UNKNOWN CMD %d size %d, ", cmd.type, cmd.size);
				}
				else
				{
					if ((u32) def->size != cmd.size)
					{
						printf("  %s. *** SIZE MISMATCH! *** target: %d actual %d\n", def->label, def->size, cmd.size);
					}
					else
					{
						printf("  %s (%d)\n", def->label, cmd.size);
					}
					
				}
				numCommands++;
				//printf("CMD type %d size %d, ", cmd.type, cmd.size);
				if (cmd.size > 0){
					//position += cmd.size;
					framePosition += cmd.size;
					fseek(f, framePosition, SEEK_SET);
					//printf(" framePosition += %d\n", cmd.size);
				}
				overFlow++;
				if (overFlow > 50)
				{
					printf("Overflowed!\n");
					break;
				}
				
			}
			// snapping to end works, stepping does not...?
			printf("    (%d cmds)\n", numCommands);
			fseek(f, endPosition, SEEK_SET);
			filePosition = endPosition;
		}
		else
		{
			printf("%s %d (Empty)\n", replay.label, replay.frameNumber);
			filePosition += replay.size;
			fseek(f, filePosition, SEEK_SET);
		}
	}
	printf("  %d frames in demo, of which %d are interesting\n\n", numFrames, numInterestingFrames);

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
	sub.ptrStart = bytes.ptrStart + h.staticCommands.offset;
	sub.capacity = h.staticCommands.size;
	sub.count = h.staticCommands.count;
	sub.ptrEnd = bytes.ptrStart + bytes.capacity;

	Test_ReadCommandBuffer(&sub);

	// Cleanup
	free(bytes.ptrStart);
	return 0;
}

void Test_StateSaving()
{
	Test_InitCmdDescriptions();
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
