#pragma once

#include "../common/com_module.h"
/*
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
*/
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

// 105
struct Cmd_EntityState
{
    // Identification
    i32 factoryType;
    EntId entityId;
    i32 tag;
    // total 8b
    
    // Linking
    EntId target;            // 4b
    EntId source;            // 4b
    // total 16b
    
    // Physical state
    Vec3 pos;                // 12b
    Vec3 rot;                // 12b
    Vec3 vel;                // 12b
    Vec3 size;               // 12b
    // total 48b
    
    // Settings
    u32 flags;
    f32 moveSpeed;
    f32 tick;                
    f32 tock;
    // total 12b
    // -- 84b so far --
    // packet of 1000b = 11 updates per packet
};

// 106
struct Cmd_RemoveEntity
{
    EntId entId;
};

struct CommandDescription
{
	u8 type;
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

void Test_InitCmdDescription(CommandDescription* cmd, u8 type, i32 size, char* label)
{
	cmd->type = type;
	cmd->size = size;
	COM_CopyStringLimited(label, cmd->label, 32);
}

void Test_InitCmdDescriptions()
{
	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 100, 48, "Spawn Static");
	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 101, 8, "Server Impulse");
	// 16 bytes for input + 12 for ids etc
	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 102, 28, "Player Input");
	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 103, 28, "Client Update");

	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 104, 5, "Text");
	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 105, 84, "Dynamic Ent State");
	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 106, 4, "Remove Ent");
	//Test_InitCmdDescription(&g_descriptions[7], 107, 4, "Ent Delta");
	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 108, 12, "Player State");
	Test_InitCmdDescription(&g_descriptions[g_numDescriptions++], 109, -1, "Entity state");
	//g_numDescriptions = 9;
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

inline void WriteCommandHeader(FILE* f, u8 type, u16 size)
{
	CmdHeader h;
	h.SetType(type);
	h.SetSize(size);
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
		switch (cheader.GetType())
		{
		case CMD_TYPE_SPAWN:
		{
			Cmd_Spawn s = {};
			fread(&s, sizeof(Cmd_Spawn), 1, f);
			printf("Spawn %d at %.2f, %.2f, %.2f\nRot: %.2f, %.2f, %.2f\n",
				s.factoryType,
				s.pos.x, s.pos.y, s.pos.z,
				s.rot.parts[0], s.rot.parts[1], s.rot.parts[2]
			);
		} break;

		default:
		{
			printf("Unknown cmd type %d. Skipping %d bytes\n", cheader.GetType(), cheader.GetSize());
			Assert(cheader.GetSize() > 0);
			fseek(f, ftell(f) + cheader.GetSize(), SEEK_SET);
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
		switch (cheader.GetType())
		{
			case CMD_TYPE_SPAWN:
			{
				Cmd_Spawn s = {};
				//fread(&s, sizeof(Cmd_Spawn), 1, f);rt
				read += COM_COPY_STRUCT(read, &s, Cmd_Spawn);
				printf("Spawn %d at %.2f, %.2f, %.2f\nRot: %.2f, %.2f, %.2f\n",
					s.factoryType,
					s.pos.parts[0], s.pos.parts[1], s.pos.parts[2],
					s.rot.parts[0], s.rot.parts[1], s.rot.parts[2]
				);
			} break;

			default:
			{
				printf("Unknown cmd type %d. Skipping %d bytes\n", cheader.GetType(), cheader.GetSize());
				Assert(cheader.GetSize() > 0);
				read += cheader.GetSize();
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

/////////////////////////////////////////////////////////////////////////////
// Analyse demo file
/////////////////////////////////////////////////////////////////////////////
void Test_PrintCommandDetails(i32 type, i32 size, u8* ptr)
{
	CommandDescription* def = Test_GetCmdDescription(type);
	if (def == NULL)
	{
		printf("UNKNOWN CMD %d size %d, ", type, size);
	}
	else
	{
		if (def->size != size)
		{
			printf("  %s. *** SIZE MISMATCH! *** target: %d actual %d ", def->label, def->size, size);
		}
		else
		{
			//Test_PrintCommandDetails
			switch (type)
			{
				// 105 dynamic state, 106 remove ent.
				case 105:
				{
					Cmd_EntityState cmd = {};
					COM_COPY_STRUCT(ptr, &cmd, Cmd_EntityState);
					printf("  %s ent %d/%d type %d pos: %.2f, %.2f, %.2f.",
						def->label, cmd.entityId.iteration, cmd.entityId.index,
						cmd.factoryType, cmd.pos.x, cmd.pos.y, cmd.pos.z
					);
				} break;

				case 106:
				{
					Cmd_RemoveEntity cmd = {};
					COM_COPY_STRUCT(ptr, &cmd, Cmd_RemoveEntity);
					printf("  %s ent %d/%d.", def->label, cmd.entId.iteration, cmd.entId.index);
				} break;

				default :
				{
					printf("  %s (%d).", def->label, size);
				} break;
			}
		}
		
	}

}

void Test_PrintReplayFramesFromFile(FILE* f, u32 fileSize, StateSaveHeader* h)
{
	if (h->frames.size == 0)
	{
		printf("* No replay data found\n");
		return;
	}
	i32 batchCount = 0;

	i32 sizeOfReplayHeader = sizeof(ReplayFrameHeader);
	i32 framesOffset = h->dynamicCommands.offset + h->dynamicCommands.size;
	i32 totalFramesData = fileSize - framesOffset;
	u32 filePosition = framesOffset;
	u32 numFrames = 0;
	u32 numInterestingFrames = 0;

	#define CMD_READ_BUFFER_SIZE 512

	MemoryBlock cmdMem = { NULL, CMD_READ_BUFFER_SIZE };
	cmdMem.ptrMemory = malloc( cmdMem.size );

	printf("\n%d bytes of frame data:\n", totalFramesData);
	fseek(f, framesOffset, SEEK_SET);
	while (filePosition < fileSize)
	{
		ReplayFrameHeader replay = {};
		fread(&replay, sizeOfReplayHeader, 1, f);

		// Check file position isn't mangled
		if (COM_CompareStrings(replay.label, "FRAME") != 0)
		{
			printf("Failed to read valid frame header at %d\n", (ftell(f) - sizeOfReplayHeader));
			free(cmdMem.ptrMemory);
			fclose(f);
			return;
		}

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
			printf("%s %d (%d bytes) ", replay.label, replay.frameNumber, replay.size);
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

				numCommands++;
				//printf("CMD type %d size %d, ", cmd.GetType(), cmd.size);
				if (cmd.GetSize() > 0){
					fread(cmdMem.ptrMemory, cmd.GetSize(), 1, f);
					Test_PrintCommandDetails(cmd.GetType(), cmd.GetSize(), (u8*)cmdMem.ptrMemory);
					//position += cmd.GetSize();
					framePosition += cmd.GetSize();
					//fseek(f, framePosition, SEEK_SET);
					//printf(" framePosition += %d\n", cmd.GetSize());
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

		batchCount++;
		if (batchCount == 60)
		{
			printf("   END OF SECOND - enter q to stop, anything else to continue...\n");
			batchCount = 0;
			char c = (char)getchar();
			if (c == 'q')
			{
				printf("  Ending read\n");
				break;
			}
		}
	}

	
	free(cmdMem.ptrMemory);
	printf("  %d frames read, of which %d were interesting\n\n", numFrames, numInterestingFrames);

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
		i32 read = 0;
		i32 cmdCount = 0;
		while (read < h.staticCommands.size)
		{
			CmdHeader cmdH = {};
			fread(&cmdH, sizeof(CmdHeader), 1, f);
			read += sizeof(CmdHeader);
			printf("  CMD %d: type %d size %d\n", cmdCount, cmdH.GetType(), cmdH.GetSize());
			read += cmdH.GetSize();
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
		i32 read = 0;
		u32 cmdCount = 0;
		while (read < h.dynamicCommands.size)
		{
			CmdHeader cmdH = {};
			fread(&cmdH, sizeof(CmdHeader), 1, f);
			read += sizeof(CmdHeader);
			CommandDescription* def = Test_GetCmdDescription(cmdH.GetType());
			if (def != NULL)
			{
				printf("  %s (%d, actual %d)\n", def->label, def->size, cmdH.GetSize());
			}
			else
			{
				printf("  CMD %d: type %d (UNKNOWN!) size %d\n", cmdCount, cmdH.GetType(), cmdH.GetSize());
			}
			
			read += cmdH.GetSize();
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
	

	//////////////////////////////////////////////////////////
	// Read Replay data
	//////////////////////////////////////////////////////////
	Test_PrintReplayFramesFromFile(f, fileSize, &h);

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

	//Test_PrintStateFileScan("base\\testbox_new.lvl");
	Test_PrintStateFileScan("base\\foo2");
	//Test_PrintStateFileScan("base\\DEMO_2018-7-1_0-44-48.DEM");
	//Test_PrintStateFileScan("base\\demo.dem");

	//Test_PrintStateFileScan("base\\foo3");
	//Test_PrintStateFileScan("foo2");

	//printf("\nLOAD %s\n\n", "map2.lvl");
	//Test_LoadAndRun("map2.lvl");

	//printf("---------------------------------------\nWrite completed. Reading...\n\n");
	//Test_ReadState("map1.lvl", 0);
	//Test_ReadState("map2.lvl", 0);
}
