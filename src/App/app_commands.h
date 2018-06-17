#pragma once

#include "app_module.cpp"

void App_SendToServer(u8* ptr, u32 type, u32 size)
{
    App_EnqueueCmd(ptr, type, size);
}

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
        printf("  IMPULSE <number> - Execute simple server commands\n");
        printf("  IMPULSE LIST - List impulse commands\n");
        printf("  TEXTURES - List App textures handles\n");
        printf("  GHOST - Toggle debug camera on/off\n");
        printf("  CLIENTS - List current client states\n");
        printf("  DEBUG COLLISION 0/1/2 - Debug collider volumes mode\n");
        printf("  DUMPHEAP - List Heap memory allocations\n");
        return 1;
    }
    if (!COM_CompareStrings(tokens[0], "IMPULSE"))
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
        App_SendToServer((u8*)&cmd, CMD_TYPE_IMPULSE, sizeof(cmd));
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
	if (!COM_CompareStrings(tokens[0], "CLIENTS"))
	{
		printf("APP Client list:\n");
		i32 len = g_clientList.max;
		for (i32 i = 0; i < g_clientList.max; ++i)
		{
			Client* cl = &g_clientList.items[i];
			printf("%d: IsLocal %d, State %d, Avatar: %d/%d\n",
				cl->clientId,
				cl->isLocal, cl->state, cl->entIdArr[0], cl->entIdArr[1]
			);

		}
		return 1;
	}
    if (!COM_CompareStrings(tokens[0], "DUMPHEAP"))
    {
        App_DumpHeap();
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
    if (COM_CompareStrings(tokens[0], "VERSION") == 0)
	{
		printf("APP Built %s: %s\n", __DATE__, __TIME__);
		return 0;
	}
    return 0;
}

void App_EnqueueCmd(u8* ptr, u32 type, u32 size)
{
    CmdHeader h = { type, size };
    u32 remaining = g_gameOutputByteBuffer.capacity - ((u32)g_gameOutputByteBuffer.ptrWrite - (u32)g_gameOutputByteBuffer.ptrStart);
    Assert(remaining >= (sizeof(CmdHeader) + size));
    g_gameOutputByteBuffer.ptrWrite += COM_COPY_STRUCT(&h, g_gameOutputByteBuffer.ptrWrite, CmdHeader);
    g_gameOutputByteBuffer.ptrWrite += COM_COPY(ptr, g_gameOutputByteBuffer.ptrWrite, size);
	g_gameOutputByteBuffer.ptrEnd = g_gameOutputByteBuffer.ptrWrite;
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
        ILLEGAL_CODE_PATH
    }

    // Read static
    ByteBuffer sub = {};
    sub.ptrStart = buf->ptrStart + h.staticEntities.offset;
    sub.capacity = h.staticEntities.size;
    sub.count = h.staticEntities.count;
    sub.ptrEnd = buf->ptrStart + buf->capacity;
    printf("APP Reading static Entitites (%d bytes)\n", sub.capacity);
    App_ReadCommandBuffer(sub);
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
// EXEC APP LEVEL COMMANDS
/////////////////////////////////////////////////////
void Exec_UpdateClient(Cmd_ClientUpdate* cmd)
{
    Client* cl = App_FindOrCreateClient(cmd->clientId);
    cl->state = cmd->state;
    cl->entIdArr[0] = cmd->entId.arr[0];
    cl->entIdArr[1] = cmd->entId.arr[1];

    // char buf[256];
    // sprintf_s(buf, 256, "APP: Client %d State: %d Avatar: iteration %d - id %d\n", cl->clientId, cl->state, cl->entIdArr[0], cl->entIdArr[1]);
    printf("APP EXEC Client %d State: %d Avatar id %d/%d\n", cl->clientId, cl->state, cl->entIdArr[0], cl->entIdArr[1]);

}

void Exec_ReadInput(u32 frameNumber, InputEvent ev)
{
    InputAction *action = Input_TestForAction(&g_inputActions, ev.value, ev.inputID, frameNumber);
}

/////////////////////////////////////////////////////
// READ COMMANDS
/////////////////////////////////////////////////////
void App_ReadCommand(u32 type, u32 bytes, u8 *ptrRead)
{
    switch (type)
    {
        case PLATFORM_EVENT_CODE_INPUT:
        {
            Assert(bytes == sizeof(InputEvent));
            InputEvent ev = {};
            ptrRead += COM_COPY_STRUCT(ptrRead, &ev, InputEvent);
            Exec_ReadInput(g_time.frameNumber, ev);
        }
        break;

        case CMD_TYPE_CLIENT_UPDATE:
        {
            Assert(bytes == sizeof(Cmd_ClientUpdate));
            Cmd_ClientUpdate cmd = {};
            ptrRead += COM_COPY_STRUCT(ptrRead, &cmd, Cmd_ClientUpdate);
            Exec_UpdateClient(&cmd);
        } break;

        default:
		{
			// Pass event down, if event is not handled
            if (Game_ReadCmd(&g_gameState, type, ptrRead, bytes))
            {
                ptrRead += bytes;
            }
            else
            {
                // buffer may be corrupted. All stop
                char buf[512];
                sprintf_s(buf, 512, "App: Unrecognised command type: %d\n", type);
                printf("%s\n", buf);
                platform.Platform_Error(buf, "APP");
                Assert(false);
            }
        }
        break;
    }
}

void App_ReadCommandBuffer(ByteBuffer commands)
{
    u8 *ptrRead = commands.ptrStart;
    
    while (ptrRead < commands.ptrEnd) // && (numRead + numSkipped) < commands.count)
    {
        BufferItemHeader header = {};
        ptrRead += COM_COPY_STRUCT(ptrRead, &header, BufferItemHeader);

        if (header.type == NULL)
        {
            // 0 == end here now
            ptrRead = commands.ptrEnd;
        }
        else
        {
            App_ReadCommand(header.type, header.size, ptrRead);
            ptrRead += header.size;
        }
    }
}
