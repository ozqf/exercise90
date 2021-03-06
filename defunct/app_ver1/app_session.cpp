#pragma once

#include "app_module.h"

////////////////////////////////////////////////////////////
// Save GameScene
////////////////////////////////////////////////////////////

// Returns bytes written
u32 App_WriteSaveState(
    GameSession* session,
    GameScene *gs,
    ByteBuffer *buf,
    StateSaveHeader *header,
    ByteBuffer* nextFrameInputBuffer)
{
    StateSaveHeader h = {};
    h.magic[0] = 'S';
    h.magic[1] = 'A';
    h.magic[2] = 'V';
    h.magic[3] = 'E';
    COM_CopyStringLimited(GetCurrentSceneName(), h.baseFile, 32);
    printf("Header base file: %s\n", h.baseFile);

    // step forward, write header later
    buf->ptrWrite += sizeof(StateSaveHeader);
    //platform.Platform_WriteToFile(g_replayFileId, (u8*)&h, sizeof(StateSaveHeader));

    printf("APP sizeof(StateSaveHeader): %d\n", sizeof(StateSaveHeader));
    printf("APP sizeof(EntityState): %d\n", sizeof(EntityState));

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
        u32 written = Ent_WriteStateCmdToAppBuffer(buf->ptrWrite, &data);
        printf("APP Write save state wrote %d bytes\n", written);
        buf->ptrWrite += written;

        h.dynamicCommands.count++;
    }

    /////////////////////////////////////////////////////////
    // Write Clients
    // (after entities so they are spawned first)
    /////////////////////////////////////////////////////////
    ClientList* clients = &session->clientList;
    for (i32 i = 0; i < clients->max; ++i)
    {
        Client *cl = &clients->items[i];
        Cmd_ClientUpdate cmd = {};
        cmd.Set(cl, 1);
        COM_WRITE_CMD_TO_BUFFER(&buf->ptrWrite, cmd);

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
    GameSession* session, GameScene* gs,
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
		written = App_WriteSaveState(session, gs, &buf, header, g_appReadBuffer);
	}
	else
	{
		// Recording a demo. Input will be written to the frame buffer anyway,
		// so skip appending next frame buffer
		written = App_WriteSaveState(session, gs, &buf, header, NULL);
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

    platform.Win32_Free(&mem);

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

void App_StartRecording(GameSession* session, GameScene *gs)
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
    g_replayFileId = App_WriteStateToFile(
        session, gs,
        fileName, false, &g_replayHeader);
}

void App_ClearScene(GameScene* gs)
{
    GS_Clear(gs);
    PhysExt_ClearWorld();
}

void App_EndSession(GameSession* session, GameScene* scene)
{
    printf("APP Ending session\n");

    session->netMode = NETMODE_NONE;
    Stream_Clear(&g_serverStream);
    App_StopRecording();
    App_ClearScene(scene);
    
    App_DeleteClients(GetClientList());
    ZNet_Shutdown();
    GameSession_Clear(session);
    Buf_Clear(g_appWriteBuffer);
}

u8 App_StartReplay(char *path, GameSession* session, GameScene* scene)
{
    printf("APP Start Replay %s\n", path);

    App_EndSession(session, scene);

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

    App_ReadStateBuffer(GetSession(), GetScene(), &g_replayReadBuffer);

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

i32 App_LoadScene(char *path, GameSession* session, GameScene* gs)
{
    printf("\n>>> APP load scene: %s <<<\n\n", path);

    App_ClearScene(gs);

	if (!COM_CompareStrings(path, "TEST"))
	{
		Game_BuildTestScene(gs, 0);
	}
    else if (COM_MatchStringStart(path, "TEST_"))
    {
		i32 index = COM_StripTrailingInteger(path, '_', 0);
        Game_BuildTestScene(gs, index);
    }
    else
    {
        if (!App_LoadStateFromFile(GetSession(), gs, path))
        {
            return COM_ERROR_MISSING_FILE;
        }
    }

    COM_CopyStringLimited(path, GetCurrentSceneName(), MAX_SCENE_NAME_CHARS);

    //App_StartRecording(&g_gameScene);
    // Mark end of load
    Cmd_Quick cmd = {};
    cmd.SetAsLevelLoadComplete();
    APP_WRITE_CMD(0, cmd);

    return COM_ERROR_NONE;
}

i32 App_StartSession(u8 netMode, char *path, GameSession* session, GameScene* gs)
{
    App_EndSession(session, gs);
    printf("\n**** APP START SESSION ****\n");
    i32 error = COM_ERROR_NONE;
    switch (netMode)
    {
        case NETMODE_NONE: return COM_ERROR_NONE;
        {

        } break;
        //case NETMODE_SINGLE_PLAYER:
        case NETMODE_LISTEN_SERVER:
        {
            error = ZNet_StartSession(netMode, NULL, ZNET_DEFAULT_SERVER_PORT);
            if (error) { printf("APP Start session failed\n"); return error; }
            error = App_LoadScene(path, session, gs);
            if (error) { printf("APP Start session failed\n"); return error; }
            session->netMode = NETMODE_LISTEN_SERVER;

            // Spawn local client if one hasn't been restored via file
            Client *cl = App_FindLocalClient(&session->clientList, 0);
            if (cl == NULL)
            {
                printf("SESSION: No local client loaded, creating\n");
                // Create a connection
                ZNetConnectionInfo info = {};
                if (ZNet_CreateLocalConnection(&info))
                {
                    session->clientList.localClientId = App_GetNextClientId(&session->clientList);
	    			printf("SV Creating local client %d\n", session->clientList.localClientId);
	    			Cmd_ClientUpdate spawnClient = {};
	    			spawnClient.clientId = session->clientList.localClientId;
	    			spawnClient.state = CLIENT_STATE_OBSERVER;
	    			APP_WRITE_CMD(0, spawnClient);
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

            return COM_ERROR_NONE;
        } break;
    
        case NETMODE_DEDICATED_SERVER:
        {
            u16 port = ZNET_DEFAULT_SERVER_PORT;
            printf("SESSION dedicated server on port %d\n", port);
            error = ZNet_StartSession(netMode, NULL, port);
            if (error) { return error; }
            error = App_LoadScene("TEST", session, gs);
            if (error) { return error; }
            session->netMode = NETMODE_DEDICATED_SERVER;
            return COM_ERROR_NONE;
        }
        break;
    
        case NETMODE_CLIENT:
        {
            ZNetAddress addr = COM_LocalHost(ZNET_DEFAULT_SERVER_PORT);
            printf("SESSION join %d.%d.%d.%d:%d\n",
                addr.ip4Bytes[0], addr.ip4Bytes[1], addr.ip4Bytes[2], addr.ip4Bytes[3], addr.port);
            error = ZNet_StartSession(netMode, &addr, ZNET_DEFAULT_CLIENT_PORT);
            if (error) { return error; }
            //error = App_LoadScene("TEST");
            //if (error) { return error; }
            session->netMode = NETMODE_CLIENT;
            return COM_ERROR_NONE;
        }
        break;
    
        case NETMODE_REPLAY:
        {
            if (App_StartReplay(path, session, gs))
            {
                session->netMode = NETMODE_REPLAY;
                return COM_ERROR_NONE;
            }
        }
        break;
    
        default:
        {
            printf("APP Cannot start unknown netMode %d\n", netMode);
        }
        break;
    }

    return COM_ERROR_UNKNOWN;
}
