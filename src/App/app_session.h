#pragma once

#include "app_module.cpp"

////////////////////////////////////////////////////////////
// Save GameState
////////////////////////////////////////////////////////////

// Returns bytes written
u32 App_WriteSaveState(GameState* gs, ByteBuffer* buf, StateSaveHeader* header)
{   
    StateSaveHeader h = {};
	h.magic[0] = 'S';
	h.magic[1] = 'A';
	h.magic[2] = 'V';
	h.magic[3] = 'E';
	COM_CopyStringLimited(g_currentSceneName, h.baseFile, 32);
    printf("Header base file: %s\n", h.baseFile);

    // step forward, write header later
    buf->ptrWrite += sizeof(StateSaveHeader);
	//platform.Platform_WriteToFile(g_replayFileId, (u8*)&h, sizeof(StateSaveHeader));

    printf("APP sizeof(StateSaveHeader): %d\n", sizeof(StateSaveHeader));
    printf("APP sizeof(EntityState): %d\n", sizeof(Cmd_EntityState));

    ///////////////////////////////
    // Write commands

    BufferItemHeader cmdHeader = {};
    
    // Record current position to calculate bytes written after loop
    u8* startOfDynamicCmds = buf->ptrWrite;
    h.dynamicCommands.offset = (u32)(buf->ptrWrite - buf->ptrStart);

    /////////////////////////////////////////////////////////
    // Write ents
    /////////////////////////////////////////////////////////
    i32 numEntities = gs->entList.count;
    for (i32 i = 0; i < numEntities; ++i)
    {
        Ent* e = &gs->entList.items[i];
        if (e->inUse != ENTITY_STATUS_IN_USE) { continue; }

        if (e->factoryType == ENTITY_TYPE_WORLD_CUBE)
        {
            Cmd_Spawn s = {};
            Game_WriteStaticState(gs, e, &s);
            cmdHeader.type = CMD_TYPE_STATIC_STATE;
            cmdHeader.size = sizeof(Cmd_Spawn);
            buf->ptrWrite += COM_COPY_STRUCT(&cmdHeader, buf->ptrWrite, BufferItemHeader);
            buf->ptrWrite += COM_COPY_STRUCT(&s, buf->ptrWrite, Cmd_Spawn);
            printf("Writing static Ent %d/%d ent type %d\n",
                s.entityId.iteration,
                s.entityId.index,
                s.factoryType
            );
        }
        else
        {
            Cmd_EntityState s = {};
            Game_WriteEntityState(gs, e, &s);
            cmdHeader.type = CMD_TYPE_ENTITY_STATE;
            cmdHeader.size = sizeof(Cmd_EntityState);
            buf->ptrWrite += COM_COPY_STRUCT(&cmdHeader, buf->ptrWrite, BufferItemHeader);
            buf->ptrWrite += COM_COPY_STRUCT(&s, buf->ptrWrite, Cmd_EntityState);
            printf("Writing dynamic %d/%d ent type %d\n",
                s.entityId.iteration,
                s.entityId.index,
                s.factoryType
            );
        }

        
        h.dynamicCommands.count++;
    }

    /////////////////////////////////////////////////////////
    // Write Clients
    // (after entities so they are spawned first)
    /////////////////////////////////////////////////////////
    ClientList* cls = &gs->clientList;
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client* cl = &cls->items[i];
        Cmd_ClientUpdate cmd = {};
        cmd.clientId = cl->clientId;
        cmd.state = cl->state;
        cmd.entId = cl->entId;
        // ? cmd.isLocal = cl->isLocal;
        cmd.input = cl->input;

        BufferItemHeader cmdH = {};
        cmdH.type = CMD_TYPE_CLIENT_UPDATE;
        cmdH.size = sizeof(Cmd_ClientUpdate);
        buf->ptrWrite += COM_COPY_STRUCT(&cmdH, buf->ptrWrite, BufferItemHeader);
        buf->ptrWrite += COM_COPY_STRUCT(&cmd, buf->ptrWrite, Cmd_ClientUpdate);

        h.dynamicCommands.count++;
    }


    u32 written = (u32)(buf->ptrWrite - buf->ptrStart);
    
    // write header
    h.dynamicCommands.size = (u32)(buf->ptrWrite - startOfDynamicCmds);
    
    // Demo frames marked as negative means demo frames should be expected
    // but the count is unknown. If the count is not set it could mean there
    // was a crash and the demo was never completed

    COM_COPY_STRUCT(&h, buf->ptrStart, StateSaveHeader);
	
	// Copy header details if necessary
	if (header != NULL)
	{
		*header = h;
	}
	
    return written;
}

////////////////////////////////////////////////////////////

// Returns id of the file opened to
i32 App_WriteStateToFile(char* fileName, u8 closeFileAfterWrite, StateSaveHeader* header)
{
    printf("APP Writing state to %s\n", fileName);
    // Allocate a temporary chunk to write to, then dump it all out into a file
    i32 capacity = MegaBytes(10);
    
    MemoryBlock mem = {};
    platform.Platform_Malloc(&mem, capacity);
    ByteBuffer buf = Buf_FromMemoryBlock(mem);

    u32 written = App_WriteSaveState(&g_gameState, &buf, header);
    
    // Write state to buffer
    
    char basePath[64];
    platform.Platform_GetBaseDirectoryName(basePath, 64);
    char path[128];
    sprintf_s(path, 128, "%s\\%s", basePath, fileName);

	i32 fileId = platform.Platform_OpenFileForWriting(path);
	Assert(fileId != -1);

    platform.Platform_WriteToFile(fileId, buf.ptrStart, written);
    printf("  Write %d bytes\n", written);

	// Clean up
    if (closeFileAfterWrite)
    {
		platform.Platform_CloseFileForWriting(fileId);
    }
    else
    {
        printf("Recording frames to \"%s\"\n", fileName);
    }

	platform.Platform_Free(&mem);
    
    return fileId;
}

void App_StopRecording()
{
    if (g_replayFileId != -1)
    {
        platform.Platform_SeekInFileFromStart(g_replayFileId, 0);
        platform.Platform_WriteToFile(g_replayFileId, (u8*)&g_replayHeader, sizeof(StateSaveHeader));
        platform.Platform_CloseFileForWriting(g_replayFileId);
        g_replayMode = NoReplayMode;
        g_replayFileId = -1;
    }
}

void App_StartRecording(GameState* gs)
{
	char fileName[128];

    DateTime dt;
    platform.Platform_GetDateTime(&dt);
	sprintf_s(fileName, 128, "DEMO_%d-%d-%d_%d-%d-%d.DEM", 
        dt.year, dt.month, dt.dayOfTheMonth,
        dt.hour, dt.minute, dt.second
    );
	//COM_StrReplace(fileName, ':', '_');
    #if 1
	if (g_replayMode != None)
	{
		printf("APP already recording...\n");
		return;
	}
	else
	{
		printf("APP recording to %s\n", fileName);
	}

	g_replayMode = RecordingReplay;
    // Write state and keep file open for writing frames
    g_replayFileId = App_WriteStateToFile(fileName, false, &g_replayHeader);

    #if 0
	u32 written = (u32)(buf->ptrWrite - buf->ptrStart);
    printf("APP Wrote %d bytes (%.2fKB) to %s:\n", written, ((f32)written / (f32)1024), demoName);
    printf("  BASE FILE: %s\n", h.baseFile);
    printf("  Dynamic Entities: offset %d count %d bytes %d\n",
        h.dynamicCommands.offset,
        h.dynamicCommands.count,
        h.dynamicCommands.size
    );
    #endif
	
    #endif
}

void App_EndSession()
{
	printf("APP Ending session\n");
	
    App_StopRecording();

	Game_Shutdown(&g_gameState);
	Phys_ClearWorld();
	// Clear command buffer
	//App_ClearClients();
	// Break client links
	App_ClearClientGameLinks(&g_gameState.clientList);
	COM_ZeroMemory((u8*)g_appWriteBuffer->ptrStart, g_appWriteBuffer->capacity);
}

u8 App_StartReplay(char* path)
{
    printf("APP Start Replay %s\n", path);

    App_EndSession();

    ///////////////////////////////////////////
    // Prepare replay buffer
    BlockRef ref = {};
    if (platform.Platform_LoadFileIntoHeap(&g_heap, &ref, path, false) == false)
    {
        printf("  APP Could not load replay file %s\n", path);
        ILLEGAL_CODE_PATH
        return 0;
    }
    Assert(ref.ptrMemory != NULL);
    printf("  replay is %d bytes.\n", ref.objectSize);
    
    g_replayReadBuffer = Heap_RefToByteBuffer(&g_heap, &ref);
    g_replayReadBuffer.ptrEnd = g_replayReadBuffer.ptrStart + g_replayReadBuffer.capacity;
    g_replayPtr = g_replayReadBuffer.ptrStart;
    g_replayMode = PlayingReplay;

    ///////////////////////////////////////////
    // Read header, load initial data
    g_replayPtr += COM_COPY_STRUCT(g_replayPtr, &g_replayHeader, StateSaveHeader);

    printf("  replay base: %s. Static cmds: %d Bytes. Dynamic cmds: %d Bytes.\n",
        g_replayHeader.baseFile,
        g_replayHeader.staticCommands.size,
        g_replayHeader.dynamicCommands.size
    );
    printf("  replay frames count: %d (%d Bytes)\n",
        g_replayHeader.frames.count,
        g_replayHeader.frames.size
    );

    App_ReadStateBuffer(&g_gameState, &g_replayReadBuffer);

    ///////////////////////////////////////////
    // Prepare frame reading
	
	// Frames array will be after the header + static size + dynamic size
	
	g_replayPtr = (u8*)ref.ptrMemory
		+ sizeof(StateSaveHeader)
		+ g_replayHeader.staticCommands.size
		+ g_replayHeader.dynamicCommands.size;
	
	ReplayFrameHeader frame = {};
	// DON'T advance the read pointer, just read the header to check we have a valid
	// starting point.
	COM_COPY_STRUCT(g_replayPtr, &frame, ReplayFrameHeader);
	if (!COM_CompareStrings(frame.label, "FRAME"))
	{
		printf("  First frame: %s (%d bytes)\n", frame.label, frame.size);
	}
	else
	{
		printf("  INVALID FRAME HEADER AT %d\n", (u32)g_replayPtr);
		ILLEGAL_CODE_PATH
	}
    return 1;
}

u8 App_StartSinglePlayer(char* path)
{
	printf(">>> APP Start single player session: %s <<<\n", path);
    
	App_EndSession();

    if (!App_LoadStateFromFile(&g_gameState, path))
    {
        return 0;
    }
	COM_CopyStringLimited(path, g_currentSceneName, MAX_SCENE_NAME_CHARS);
	
	App_StartRecording(&g_gameState);

	// Spawn local client
	// Assign local client id.
    Cmd_ClientUpdate spawnClient = {};
    // assign local id directly...
    // TODO: Do local client Id assignment via network command
    g_localClientId = -1;
    spawnClient.clientId = g_localClientId;
    spawnClient.state = CLIENT_STATE_OBSERVER;
    App_WriteGameCmd((u8*)&spawnClient, CMD_TYPE_CLIENT_UPDATE, sizeof(Cmd_ClientUpdate));
    return 1;
}

i32 App_StartSession(u8 netMode, char* path)
{
    printf("\n**** APP START SESSION ****\n");
    switch (netMode)
    {
        case NETMODE_SINGLE_PLAYER:
        {
            if (App_StartSinglePlayer(path))
            {
                g_gameState.netMode = NETMODE_SINGLE_PLAYER;
                return 1;
            }

        } break;

        case NETMODE_REPLAY:
        {
            if (App_StartReplay(path))
            {
                g_gameState.netMode = NETMODE_REPLAY;
                return 1;
            }
        } break;

        default:
        {
            printf("APP Cannot start unknown netMode %d\n", netMode);
        } break;
    }

    return 0;
}
