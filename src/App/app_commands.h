#pragma once

#include "app_module.cpp"
#if 0
void App_WriteGameCmd(u8* ptr, u8 type, u16 size)
{
    CmdHeader h = { type, size };
    u32 remaining = g_appWriteBuffer->capacity - ((u32)g_appWriteBuffer->ptrWrite - (u32)g_appWriteBuffer->ptrStart);
    Assert(remaining >= (sizeof(CmdHeader) + size));
    u8* start = g_appWriteBuffer->ptrWrite;
	
    g_appWriteBuffer->ptrWrite += COM_COPY_STRUCT(&h, g_appWriteBuffer->ptrWrite, CmdHeader);
    g_appWriteBuffer->ptrWrite += COM_COPY(ptr, g_appWriteBuffer->ptrWrite, size);
	g_appWriteBuffer->ptrEnd = g_appWriteBuffer->ptrWrite;
    
    if (g_time.singleFrame)
	{
		printf("APP Wrote type %d (%d size, %d actual) to buffer %s\n",
            type,
            size,
            ((u32)g_appWriteBuffer->ptrWrite - (u32)start),
            App_GetBufferName(g_appWriteBuffer->ptrStart)
        );
	}
}
#endif

// void App_SendToServer(u8* ptr, u8 type, u16 size)
// {
//     App_WriteGameCmd(ptr, type, size);
// }

void App_DumpHeap()
{
    printf("APP HEAP STATUS: Used: %d Free: %d NextId: %d\n", g_heap.usedSpace, g_heap.freeSpace, g_heap.nextID);
    Heap_DebugPrintAllocations2(&g_heap);
}

u8 App_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
    //printf("App Parse %s\n", str);
    if (!COM_CompareStrings(tokens[0], "HELP"))
    {
        printf("  LOAD SOMEFILE.LVL - Load and execute a level/save/demo file\n");
        printf("  SAVE SOMEFILE - Save Game State\n");
        printf("  I or IMPULSE <number> - Execute simple server commands\n");
        printf("  IMPULSE LIST - List impulse commands\n");
        printf("  ENTS - List entities\n");
        printf("  VARS - list data variables\n");
        printf("  DEBUG - Cycle debug print outs\n");
        printf("  DEBUG COLLISION 0/1/2 - Debug collider volumes mode\n");
        printf("  TEXTURES - List App textures handles\n");
        printf("  GHOST - Toggle debug camera on/off\n");
        printf("  CLIENTS - List current client states\n");
        printf("  DUMPHEAP - List Heap memory allocations\n");
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
        cmd.clientId = g_localClientId;
        cmd.impulse = COM_AsciToInt32(tokens[1]);
        printf("Client %d sending impulse %d\n", cmd.clientId, cmd.impulse);
        APP_WRITE_CMD(0, CMD_TYPE_IMPULSE, 0, cmd);
        //App_SendToServer((u8*)&cmd, CMD_TYPE_IMPULSE, sizeof(cmd));
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
        StateSaveHeader h = {};
        App_WriteStateToFile(tokens[1], true, &h);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "LOAD"))
    {
        if (numTokens == 2)
        {
            if (!App_StartSession(NETMODE_SINGLE_PLAYER, tokens[1]))
            {
                printf("Failed to load game\n");
            }
        }
        else
        {
            printf(" LOAD: load single player game. eg LOAD testbox.lvl\n");
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
        App_StartSession(NETMODE_REPLAY, tokens[1]);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "STOP"))
    {
        // Stop recording, or stop replay playback
        g_replayMode = NoReplayMode;
        App_StartSession(NETMODE_SINGLE_PLAYER, "maps\\testbox.lvl");
        return 1;

    }
    if (!COM_CompareStrings(tokens[0], "ENTS"))
    {
        App_DebugPrintEntities(&g_gameState);
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "PLAYERS"))
    {
        //App_DebugPrintPlayers(&g_gameState);
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
            g_gameState.debugMode = (u16)val;
            printf("APP Debug text mode %d\n", g_gameState.debugMode);
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
		i32 len = g_gameState.clientList.max;
		for (i32 i = 0; i < g_gameState.clientList.max; ++i)
		{
			Client* cl = &g_gameState.clientList.items[i];
			printf("%d: IsLocal %d, State %d, Avatar: %d/%d\n",
				cl->clientId,
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
    return 0;
}

////////////////////////////////////////////////////////////////////////////
// Loading
////////////////////////////////////////////////////////////////////////////

void App_ReadStateBuffer(GameState *gs, ByteBuffer *buf)
{
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
    sub.count = h.staticCommands.count;
    sub.ptrEnd = buf->ptrStart + buf->capacity;
    
	if (sub.capacity != 0) {
		printf("APP Reading static commands (%d bytes)\n", sub.capacity);
		i32 count = Game_ReadCommandBuffer(gs, &sub, (g_time.singleFrame != 0));
        printf("  Executed %d commands\n", count);
	}
	else
	{
		printf("APP Read no static command bytes\n");
	}
	
	// Read Dynamic
	sub.ptrStart = buf->ptrStart + h.dynamicCommands.offset;
	sub.capacity = h.dynamicCommands.size;
	sub.count = h.dynamicCommands.count;
	sub.ptrEnd = buf->ptrStart + buf->capacity;
	if (sub.capacity > 0)
	{
		printf("APP Reading dynamic commands (%d bytes)\n", sub.capacity);
		i32 count = Game_ReadCommandBuffer(gs, &sub, (g_time.singleFrame != 0));
        printf("  Executed %d commands\n", count);
	}
	else
	{
		printf("APP Read no dynamic commands bytes\n");
	}
}

u8 App_LoadStateFromFile(GameState *gs, char *fileName)
{
    printf("APP Load state from %s\n", fileName);
    BlockRef ref = {};
    platform.Platform_LoadFileIntoHeap(&g_heap, &ref, fileName, 0);
    ByteBuffer bytes = Heap_RefToByteBuffer(&g_heap, &ref);

    App_ReadStateBuffer(gs, &bytes);

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
            Assert(bytes == sizeof(InputEvent));
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
