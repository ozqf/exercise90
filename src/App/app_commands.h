#pragma once

#include "app_module.cpp"

//////////////////////////////////////////////////////////////////////
// Write unknown bytes into the current command buffer
//////////////////////////////////////////////////////////////////////

// leave a space for a header and return a write position to the caller
inline u8* App_StartCommandStream()
{
    u8* headerWritePosition = g_appWriteBuffer->ptrWrite;
    g_appWriteBuffer->ptrWrite += sizeof(CmdHeader);
    return headerWritePosition;
}

inline u32 App_WriteCommandBytesToFrameOutput(u8* inputStream, u32 numBytes)
{
    g_appWriteBuffer->ptrWrite +=
        COM_COPY(inputStream, g_appWriteBuffer->ptrWrite, numBytes);
    return numBytes;
}

// Write the given header information into the given position
inline void App_FinishCommandStream(u8* ptr, u8 cmdType, u8 cmdFlags, u16 cmdSize)
{
    CmdHeader h = {};
    h.SetType(cmdType);
    h.SetSize(cmdSize);
    h.Write(ptr);
}

void App_DumpHeap()
{
    printf("APP HEAP STATUS: Used: %d Free: %d NextId: %d\n",
        g_heap.usedSpace, g_heap.freeSpace, g_heap.nextID);
    Heap_DebugPrintAllocations2(&g_heap);
}

u8 App_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
    if (!COM_CompareStrings(tokens[0], "GOD"))
    {
        Ent* ent = Game_GetLocalClientEnt(&g_session.clientList, &g_gameScene.entList);
        if (!ent) { return 1; }
        EC_Health* hp = EC_FindHealth(&g_gameScene, ent);
        if (!hp) { return 1; }
        if ((hp->state.flags & EC_HEALTH_FLAG_INVULNERABLE))
        {
            COM_DisableBits(&hp->state.flags, EC_HEALTH_FLAG_INVULNERABLE);
            printf("God OFF\n");
        }
        else
        {
            printf("God ON\n");
            hp->state.flags |= EC_HEALTH_FLAG_INVULNERABLE;
        }
        return 1;
    }
    //printf("App Parse %s\n", str);
    if (!COM_CompareStrings(tokens[0], "HELP"))
    {
        printf("  LOAD SOMEFILE.LVL - Load and execute a level/save/demo file\n");
        printf("  SAVE SOMEFILE - Save Game State\n");
        printf("  TESTSCENE - Load basic test level\n");
        printf("  I or IMPULSE <number> - Execute simple server commands\n");
        printf("  IMPULSE LIST - List impulse commands\n");
        printf("  ENTS - List entities\n");
        printf("  VARS - list data variables\n");
        printf("  DEBUG - Cycle debug print outs\n");
        printf("  DEBUG COLLISION 0/1/2 - Debug collider volumes mode\n");
        printf("  TEXTURES - List App textures handles\n");
        printf("  GHOST - Toggle debug camera on/off\n");
        printf("  CLIENTS - List current client states\n");
        printf("  DUMPHEAP - List Heap memory alallocations\n");
		printf("  MENU - Open specific menu\n");
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "SAY"))
    {
        //printf("APP Client say...\n");
        Net_TransmitSay(&g_session, str, tokens, numTokens);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "TESTSCENE"))
    {
        Game_BuildTestScene(&g_gameScene, 0);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "IMPULSE") || !COM_CompareStrings(tokens[0], "I"))
    {
        if (numTokens != 2)
        {
            printf("Wrong number of tokens for impulse\n");
            return 1;
        }
        Cmd_ServerImpulse cmd = {};
        cmd.clientId = g_session.clientList.localClientId;
        cmd.impulse = COM_AsciToInt32(tokens[1]);
        printf("Client %d sending impulse %d\n", cmd.clientId, cmd.impulse);
        APP_WRITE_CMD(0, cmd);
        //App_SendToServer((u8*)&cmd, CMD_TYPE_IMPULSE, sizeof(cmd));
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "SPAWN"))
    {
        if (numTokens == 2)
        {
			i32 i = COM_AsciToInt32(tokens[1]);
			if (i != 0)
			{
				EntitySpawnOptions options = {};
                options.scale = { 1, 1, 1 };
				Ent_QuickSpawnCmd(&g_gameScene, i, &options);
				return 1;
			}
            if (!COM_CompareStrings(tokens[1], "ENEMY"))
            {
                printf("*** SPAWN ENEMY ***\n");
                EntitySpawnOptions options = {};
                options.scale = { 1, 1, 1 };
                options.pos.y = 0.75f;
                options.rot.y = 45;
                Ent_QuickSpawnCmd(&g_gameScene, ENTITY_TYPE6_ENEMY, &options);
            }
        }
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "GFX"))
    {
        if (numTokens == 1) { printf("Need GFX type\n"); return 1; }
        i32 i = COM_AsciToInt32(tokens[1]);
        Game_SpawnLocalEntity(0, 0, 0, NULL, 0, i);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "VARS"))
    {
        App_DebugListVariables(g_vars, g_nextVar, MAX_VARS);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "SAVE"))
    {
        if (numTokens != 2)
        {
            printf("  No file name specified for save\n");
            return 1;
        }
        printf("APP Buffered state op SAVE %s\n", tokens[1]);
		COM_CopyStringLimited(tokens[1], g_appStateOperation.fileName, 63);
		g_appStateOperation.op = APP_STATE_OP_SAVE;
        /*StateSaveHeader h = {};
        App_WriteStateToFile(tokens[1], true, &h);*/
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "LOAD"))
    {
        if (numTokens == 2)
        {
            COM_CopyStringLimited(tokens[1], g_appStateOperation.fileName, 63);
			g_appStateOperation.op = APP_STATE_OP_LOAD;
        }
        else
        {
            printf(" LOAD: load single player game. eg LOAD testbox.lvl\n");
        }
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "HOST"))
    {
        if (numTokens == 2)
        {
            // TODO Specifying port and other settings here too!
			COM_CopyStringLimited(tokens[1], g_appStateOperation.fileName, 63);
			g_appStateOperation.op = APP_STATE_OP_HOST;
        }
        else
        {
            printf(" HOST: Start a multiplayer game on the given level, eg HOST testbox.lvl\n");
        }
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "JOIN"))
    {
        if (numTokens == 2)
        {
            COM_CopyStringLimited(tokens[1], g_appStateOperation.fileName, 63);
			g_appStateOperation.op = APP_STATE_OP_JOIN;
        }
        else
        {
            printf(" JOIN: connect to a multiplaye game at the given ip address and port\n");
        }
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "PLAY"))
    {
        // playback the given replay file
        if (numTokens != 2)
        {
            printf("Incorrect parameter count\n");
            return 1;
        }
        printf("Replay from %s\n", tokens[1]);
        App_StartSession(NETMODE_REPLAY, tokens[1], &g_session);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "STOP"))
    {
        // Stop recording, or stop replay playback
        g_replayMode = NoReplayMode;
        App_StartSession(NETMODE_SINGLE_PLAYER, APP_FIRST_MAP, &g_session);
        return 1;

    }
    if (!COM_CompareStrings(tokens[0], "ENTS"))
    {
        App_DebugPrintEntities(&g_gameScene);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "PLAYERS"))
    {
        //App_DebugPrintPlayers(&g_gameScene);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "TEXTURES"))
    {
        AppListTextures();
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "GHOST"))
    {
        g_debugCameraOn = !g_debugCameraOn;
        printf(" Ghost mode: %d\n", g_debugCameraOn);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "S"))
    {
        BlockRef ref = {};
        platform.Platform_LoadFileIntoHeap(&g_heap, &ref, "sounds\\Weapon_Pickup.wav", 0);
        platform.Platform_LoadSound((u8*)ref.ptrMemory, ref.objectSize);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "UIGFX"))
    {
        Cmd_SpawnHudItem cmd = {};
        cmd.pos = Game_RandomSpawnOffset(24, 0, 24);
        APP_WRITE_CMD(0, cmd);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "DEBUG"))
    {
        if (numTokens == 1)
        {
            printf("DEBUG requires additional parameters\n");
            return 1;
        }
        else if (numTokens == 2)
        {
            i32 val = COM_AsciToInt32(tokens[1]);
            g_gameScene.debugMode = (u16)val;
            printf("APP Debug text mode %d\n", g_gameScene.debugMode);
        }
        // else if (numTokens == 2)
        // {
        //     if (!COM_CompareStrings(tokens[1], "COLLISION"))
        //     {

        //     }
        //     return 1;
        // }
        else if (numTokens == 3)
        {
            if (!COM_CompareStrings(tokens[1], "COLLISION"))
            {
                i32 val = COM_AsciToInt32(tokens[2]);
                switch (val)
                {
                    case 0:
                    {
                        printf("No collision debug\n");
                        g_debugColliders = 0;
                    } break;
                    case 1:
                    {
                        printf("Show colliders in ghost mode\n");
                        g_debugColliders = 1;
                    } break;
                    case 2:
                    {
                        printf("Show colliders always\n");
                        g_debugColliders = 2;
                    } break;
                    default:
                    {
                        printf("Debug collision can be 0, 1 or 2\n");
                    } break;
                }
            }
        }
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "CFG"))
    {
        if (numTokens != 3)
        {
            printf("  Incorrect tokens for CFG command. CFG <Action> <fileName>");
            printf("  Actions: load, save");
            return 1;
        }
        if (!COM_CompareStrings(tokens[1], "SAVE"))
        {
            App_SaveDataVariables(tokens[2]);
        }
        
    }
	if (!COM_CompareStrings(tokens[0], "CLIENTS"))
	{
		printf("APP Client list:\n");
        ClientList* cls = &g_session.clientList;
		i32 len = cls->max;
		for (i32 i = 0; i < cls->max; ++i)
		{
			Client* cl = &cls->items[i];
			printf("%d: ConnId %d, IsLocal %d, State %d, Avatar: %d/%d\n",
				cl->clientId, cl->connectionId,
				cl->isLocal, cl->state, cl->entId.iteration, cl->entId.index
			);

		}
		return 1;
	}
    if (!COM_CompareStrings(tokens[0], "DUMPHEAP"))
    {
        App_DumpHeap();
        return 1;
    }
    if (COM_CompareStrings(tokens[0], "VERSION") == 0)
	{
		printf("APP Built %s: %s\n", __DATE__, __TIME__);
		return 0;
	}
	if (!COM_CompareStrings(tokens[0], "MENU"))
	{
		return Menu_ParseCommandString(str, tokens, numTokens);
	}
	if (!COM_CompareStrings(tokens[0], "MINIMISED"))
	{
		g_minimised = 1;
	}
	if (!COM_CompareStrings(tokens[0], "MAXIMISED"))
	{
		g_minimised = 0;
	}
    if (numTokens == 2 && !COM_CompareStrings(tokens[0], "APP") && !COM_CompareStrings(tokens[1], "CRASH"))
	{
		APP_ASSERT(0, "BOOOOOOM");
        return 1;
	}
    return 0;
}

////////////////////////////////////////////////////////////////////////////
// Loading
////////////////////////////////////////////////////////////////////////////

void App_ReadStateBuffer(GameSession* session, GameScene* gs, ByteBuffer *buf)
{
    ClientList* clients = &session->clientList;
    //App_ReadCommandBuffer(&g_time, buf);
    u8 *read = buf->ptrStart;
    StateSaveHeader h = {};
    read += COM_COPY_STRUCT(read, &h, StateSaveHeader);
    if (
        h.magic[0] != 'S' || h.magic[1] != 'A' || h.magic[2] != 'V' || h.magic[3] != 'E')
    {
		platform.Platform_Error("Buffer is not a SAVE file", "APP");
    }

    if (h.protocol != COMMAND_PROTOCOL_ID)
    {
        char errorMsg[128];
        sprintf_s(errorMsg, 128, "Protocol mismatch in state buffer. Expected %d got %d\n", COMMAND_PROTOCOL_ID, h.protocol);
        platform.Platform_Error(errorMsg, "APP");
    }

    // Read static
    ByteBuffer sub = {};
    sub.ptrStart = buf->ptrStart + h.staticCommands.offset;
    sub.capacity = h.staticCommands.size;
    sub.ptrEnd = buf->ptrStart + buf->capacity;
    
	if (sub.capacity != 0) {
		printf("APP Reading static commands (%d bytes)\n", sub.capacity);
		i32 count = Game_ReadCommandBuffer(session, gs, &sub, (g_time.singleFrame != 0));
        printf("  Executed %d commands\n", count);
	}
	else
	{
		printf("APP Read no static command bytes\n");
	}
	
	// Read Dynamic
	sub.ptrStart = buf->ptrStart + h.dynamicCommands.offset;
	sub.capacity = h.dynamicCommands.size;
	sub.ptrEnd = buf->ptrStart + buf->capacity;
	if (sub.capacity > 0)
	{
		printf("APP Reading dynamic commands (%d bytes)\n", sub.capacity);
		i32 count = Game_ReadCommandBuffer(session, gs, &sub, (g_time.singleFrame != 0));
        printf("  Executed %d commands\n", count);
	}
	else
	{
		printf("APP Read no dynamic commands bytes\n");
	}
}

u8 App_LoadStateFromFile(GameSession* session, GameScene *gs, char *fileName)
{
    printf("APP Load state from %s\n", fileName);
    BlockRef ref = {};
    platform.Platform_LoadFileIntoHeap(&g_heap, &ref, fileName, 0);
    ByteBuffer bytes = Heap_RefToByteBuffer(&g_heap, &ref);

    App_ReadStateBuffer(session, gs, &bytes);

    Heap_Free(&g_heap, ref.id);
    return 1;
}

/////////////////////////////////////////////////////
// READ COMMANDS
/////////////////////////////////////////////////////
void App_ReadPlatformCommand(u32 type, u32 bytes, u8 *ptrRead)
{
    switch (type)
    {
        case PLATFORM_EVENT_CODE_INPUT:
        {
            APP_ASSERT((bytes == sizeof(InputEvent)), "Platform event cmd wrong size");
            InputEvent ev = {};
            ptrRead += COM_COPY_STRUCT(ptrRead, &ev, InputEvent);
            Input_TestForAction(&g_inputActions, ev.value, ev.inputID, g_time.platformFrameNumber);
        }
        break;
    }
}

void App_ReadPlatformCommandBuffer(ByteBuffer* commands)
{
    u8 *ptrRead = commands->ptrStart;
    
    while (ptrRead < commands->ptrEnd) // && (numRead + numSkipped) < commands.count)
    {
        PlatformEventHeader header = {};
        ptrRead += COM_COPY_STRUCT(ptrRead, &header, PlatformEventHeader);

        if (header.type == NULL)
        {
            // 0 == end here now
            ptrRead = commands->ptrEnd;
        }
        else
        {
            App_ReadPlatformCommand(header.type, header.size, ptrRead);
            ptrRead += header.size;
        }
    }
}
