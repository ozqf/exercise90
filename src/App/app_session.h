#pragma once

#include "app_module.cpp"

////////////////////////////////////////////////////////////
// Save GameState
////////////////////////////////////////////////////////////

// Returns bytes written
u32 App_WriteSaveState(
    GameState *gs,
    ByteBuffer *buf,
    StateSaveHeader *header,
    ByteBuffer* nextFrameInputBuffer)
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

    // Record current position to calculate bytes written after loop
    u8 *startOfDynamicCmds = buf->ptrWrite;
    h.dynamicCommands.offset = (u32)(buf->ptrWrite - buf->ptrStart);

    /////////////////////////////////////////////////////////
    // Write ents
    /////////////////////////////////////////////////////////
    i32 numEntities = gs->entList.count;
    for (i32 i = 0; i < numEntities; ++i)
    {
        Ent *e = &gs->entList.items[i];
        if (e->inUse != ENTITY_STATUS_IN_USE)
        {
            continue;
        }

        EntityState data = {};
        Ent_CopyFullEntityState(gs, e, &data);
        u32 written = Ent_WriteEntityStateCmd(buf->ptrWrite, &data);
        printf("APP Write save state wrote %d bytes\n", written);
        buf->ptrWrite += written;

        h.dynamicCommands.count++;
    }

    /////////////////////////////////////////////////////////
    // Write Clients
    // (after entities so they are spawned first)
    /////////////////////////////////////////////////////////
    ClientList *cls = &gs->clientList;
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client *cl = &cls->items[i];
        Cmd_ClientUpdate cmd = {};
        cmd.Set(cl);
        COM_WRITE_CMD_TO_BUFFER(&buf->ptrWrite, CMD_TYPE_CLIENT_UPDATE, 0, cmd);

        h.dynamicCommands.count++;
    }

	/////////////////////////////////////////////////////////
	// Write Command buffer
	// Input that was pending for the next frame
	// Placed here so that state restore is completed before
	// these commands execute
	/////////////////////////////////////////////////////////
	if (nextFrameInputBuffer != NULL)
	{
		u8* copyPos = nextFrameInputBuffer->ptrStart;
		u32 bytes = nextFrameInputBuffer->ptrEnd - nextFrameInputBuffer->ptrStart;
		printf("Appending %d bytes of next-frame data\n", bytes);
		buf->ptrWrite += COM_COPY(copyPos, buf->ptrWrite, bytes);
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

// Returns id of the file opened to (if it is left open natch)
i32 App_WriteStateToFile(
    char *fileName, u8 closeFileAfterWrite, StateSaveHeader *header)
{
    printf("APP Writing state to %s\n", fileName);
    i32 capacity = MegaBytes(10);

    MemoryBlock mem = {};
    platform.Platform_Malloc(&mem, capacity);
    ByteBuffer buf = Buf_FromMemoryBlock(mem);

    u32 written;
	if (closeFileAfterWrite)
	{
		written = App_WriteSaveState(&g_gameState, &buf, header, g_appReadBuffer);
	}
	else
	{
		// Recording a demo. Input will be written to the frame buffer anyway,
		// so skip appending next frame buffer
		written = App_WriteSaveState(&g_gameState, &buf, header, NULL);
	}

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
        platform.Platform_WriteToFile(g_replayFileId, (u8 *)&g_replayHeader, sizeof(StateSaveHeader));
        platform.Platform_CloseFileForWriting(g_replayFileId);
        g_replayMode = NoReplayMode;
        g_replayFileId = -1;
    }
}

void App_StartRecording(GameState *gs)
{
    char fileName[128];

    DateTime dt;
    platform.Platform_GetDateTime(&dt);
    sprintf_s(fileName, 128, "DEMO_%d-%d-%d_%d-%d-%d.DEM",
              dt.year, dt.month, dt.dayOfTheMonth,
              dt.hour, dt.minute, dt.second);

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
}

void App_ClearLevel()
{
    GS_Clear(&g_gameState);
    PhysExt_ClearWorld();
    App_ClearClientGameLinks(&g_gameState.clientList);
    App_ClearIOBuffers();
}

void App_LoadLevel()
{
    App_ClearLevel();
}

void App_EndSession()
{
    printf("APP Ending session\n");

    App_StopRecording();
    App_ClearLevel();
    ZNet_Shutdown();
    COM_ZeroMemory((u8 *)g_appWriteBuffer->ptrStart, g_appWriteBuffer->capacity);
}

u8 App_StartReplay(char *path)
{
    printf("APP Start Replay %s\n", path);

    App_EndSession();

    ///////////////////////////////////////////
    // Prepare replay buffer
    BlockRef ref = {};
    if (platform.Platform_LoadFileIntoHeap(&g_heap, &ref, path, false) == false)
    {
        printf("  APP Could not load replay file %s\n", path);
        platform.Platform_WriteTextCommand("STOP");
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
           g_replayHeader.dynamicCommands.size);
    printf("  replay frames count: %d (%d Bytes)\n",
           g_replayHeader.frames.count,
           g_replayHeader.frames.size);

    App_ReadStateBuffer(&g_gameState, &g_replayReadBuffer);

    ///////////////////////////////////////////
    // Prepare frame reading

    // Frames array will be after the header + static size + dynamic size

    g_replayPtr = (u8 *)ref.ptrMemory + sizeof(StateSaveHeader) + g_replayHeader.staticCommands.size + g_replayHeader.dynamicCommands.size;

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

u8 App_LoadScene(char *path)
{
    printf(">>> APP load scene: %s <<<\n", path);

	if (!COM_CompareStrings(path, "TEST"))
	{
		Game_BuildTestScene(&g_gameState, 0);
	}
    else if (COM_MatchStringStart(path, "TEST_"))
    {
		i32 index = COM_StripTrailingInteger(path, '_', 0);
        Game_BuildTestScene(&g_gameState, index);
    }
    else
    {
        if (!App_LoadStateFromFile(&g_gameState, path))
        {
            return 0;
        }
    }

    COM_CopyStringLimited(path, g_currentSceneName, MAX_SCENE_NAME_CHARS);

    //App_StartRecording(&g_gameState);

    

    return 1;
}

i32 App_StartSession(u8 netMode, char *path)
{
    App_EndSession();
    printf("\n**** APP START SESSION ****\n");
    switch (netMode)
    {
    //case NETMODE_SINGLE_PLAYER:
    case NETMODE_LISTEN_SERVER:
    {
        ZNet_StartSession(netMode, NULL, ZNET_DEFAULT_SERVER_PORT);
        if (!App_LoadScene(path))
        {
            return 0;
        }
        g_gameState.netMode = NETMODE_LISTEN_SERVER;

        // Spawn local client if one hasn't been restored via file
        Client *cl = App_FindLocalClient(&g_gameState.clientList, 0);
        if (cl == NULL)
        {
            printf("SESSION: No local client loaded, creating\n");
            // Create a connection
            ZNetConnectionInfo info = {};
            if (ZNet_CreateLocalConnection(&info))
            {
                g_localClientId = App_GetNextClientId(&g_gameState.clientList);
				
				Cmd_ClientUpdate spawnClient = {};
				spawnClient.clientId = g_localClientId;
				spawnClient.state = CLIENT_STATE_OBSERVER;
				APP_WRITE_CMD(0, CMD_TYPE_CLIENT_UPDATE, 0, spawnClient);
            }
			else
			{
				ILLEGAL_CODE_PATH
			}
        }
		else
		{
			printf("Found local client %d for session start\n", cl->clientId);
		}

        return 1;
    }
    break;
    
    case NETMODE_DEDICATED_SERVER:
    {
        u16 port = ZNET_DEFAULT_SERVER_PORT;
        printf("SESSION dedicated server on port %d\n", port);
        ZNet_StartSession(netMode, NULL, port);
        if (!App_LoadScene("TEST"))
        {
            APP_ASSERT(0, "Failed to init network session\n");
        }
        g_gameState.netMode = NETMODE_DEDICATED_SERVER;
        return 1;
    }
    break;

    case NETMODE_CLIENT:
    {
        ZNetAddress addr = COM_LocalHost(ZNET_DEFAULT_SERVER_PORT);
        printf("SESSION join %d.%d.%d.%d:%d\n",
            addr.ip4Bytes[0], addr.ip4Bytes[1], addr.ip4Bytes[2], addr.ip4Bytes[3], addr.port);
        ZNet_StartSession(netMode, &addr, ZNET_DEFAULT_CLIENT_PORT);
        if (!App_LoadScene("TEST"))
        {
            APP_ASSERT(0, "Failed to init network session\n");
        }
        g_gameState.netMode = NETMODE_CLIENT;
        return 1;
    }
    break;

    case NETMODE_REPLAY:
    {
        if (App_StartReplay(path))
        {
            g_gameState.netMode = NETMODE_REPLAY;
            return 1;
        }
    }
    break;

    default:
    {
        printf("APP Cannot start unknown netMode %d\n", netMode);
    }
    break;
    }

    return 0;
}
