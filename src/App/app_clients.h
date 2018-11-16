#pragma once

#include "app_module.cpp"

internal i32 App_GetNextClientId(ClientList* cls)
{
    // The first ID given out must be 1
    // 0 == no client
    i32 id = ++cls->nextId;
    return id;
}

/*internal*/ void App_EndAllClients(ClientList* cls)
{
	for (i32 i = 0; i < cls->max; ++i)
	{
		cls->items[i] = {};
	}
}

internal void App_ClearClientGameLinks(ClientList* cls)
{
	for (i32 i = 0; i < cls->max; ++i)
	{
		Client* cl = &cls->items[i];
        cl->entId = {};
		cl->state = CLIENT_STATE_FREE;// CLIENT_STATE_OBSERVER;

	}
}

internal void App_DeleteClients(ClientList* cls)
{
    for (i32 i = 0; i < cls->max; ++i)
	{
		Client* cl = &cls->items[i];
        printf("APP Deleting client %d I/O Streams\n", cl->clientId);
        if (cl->stream.inputBuffer.ptrStart)
        {
            free((void*)cl->stream.inputBuffer.ptrStart);
        }
        if (cl->stream.outputBuffer.ptrStart)
        {
            free((void*)cl->stream.outputBuffer.ptrStart);
        }
        *cl = {};
	}
}

internal Client* App_FindLocalClient(ClientList* cls, u8 checkIfPlaying)
{
    if (g_localClientId == 0) { return NULL; }
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client* cl = &cls->items[i];
        if (cl->state != CLIENT_STATE_FREE && cl->clientId == g_localClientId)
        {
            if (checkIfPlaying && cl->state != CLIENT_STATE_PLAYING)
            {
                return NULL;
            }
            return cl;
        }
    }
    return NULL;
}

/*internal*/ void App_BuildClientUpdate(GameScene* gs, Client* cl)
{

}

/*internal*/ void App_AddLocalClient()
{

}

/////////////////////////////////////////////////////
// UPDATE CLIENTS
/////////////////////////////////////////////////////
internal void App_UpdateLocalClient(Client* cl, InputActionSet* actions, u32 frameNumber)
{
    switch (cl->state)
    {
        case CLIENT_STATE_OBSERVER:
        {
            if (g_debugCameraOn)
            {
                return;
            }
            #if 1
            if (Input_CheckActionToggledOn(actions, "Move Up", frameNumber))
            {
                Cmd_ServerImpulse cmd = {};
                cmd.clientId = cl->clientId;
                cmd.impulse = IMPULSE_JOIN_GAME;

                printf("APP: Client %d wishes to spawn\n", cl->clientId);
                APP_WRITE_CMD(&g_appWriteBuffer->ptrWrite, CMD_TYPE_IMPULSE, 0, cmd);
            }
            #endif
        } break;

        case CLIENT_STATE_PLAYING:
        {
            if (g_menuOn == 0 && !g_debugCameraOn)
            {
                Game_CreateClientInput(&g_inputActions, &cl->input);
            }
            else
            {
                cl->input.buttons = 0;
            }
            // Always copy input (and other vars) even if it hasn't
            //mbeen affected or the players orientation will be reset!
            // (and everything else, including state)
            Cmd_PlayerInput cmd = {};
			cmd.connectionId = cl->connectionId;
            cmd.input = cl->input;
            APP_WRITE_CMD(
                &g_appWriteBuffer->ptrWrite,
                CMD_TYPE_PLAYER_INPUT, 0, cmd);
            //App_WriteGameCmd((u8*)&cmd, CMD_TYPE_PLAYER_INPUT, sizeof(Cmd_PlayerInput));
        } break;

        case CLIENT_STATE_FREE:
        {
            printf("APP Attempting to update a free client. ID: %d\n ", cl->clientId);
        } break;

        default:
        {
            char buf[256];
            sprintf_s(buf, 256, "APP Unknown client state %d\n", cl->state);
            platform.Platform_Error(buf, "APP");
        } break;
    }
}

internal Client* App_FindClientById(i32 id, ClientList* cls)
{
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client* query = &cls->items[i];
        if (query->clientId == id)
        {
            return query;
        }
    }
    return NULL;
}

i32 App_NumPlayingClients(ClientList* cls)
{
    i32 count = 0;
    for (i32 i = 0; i < cls->max; ++i)
    {
        if (cls->items[i].entId.value != 0)
        {
            count++;
        }
    }
    return count;
}

internal Client* App_FindClientByEntId(EntId entId, ClientList* cls)
{
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client* query = &cls->items[i];
        if (query->entId.value == entId.value)
        {
            return query;
        }
    }
    return NULL;
}

internal Client* App_FindClientByConnectionId(ClientList* cls, i32 connId)
{
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client* query = &cls->items[i];
        if (query->connectionId == connId)
        {
            return query;
        }
    }
    return NULL;
}

internal Client* App_FindOrCreateClient(i32 id, ClientList* cls)
{
    Client* free = NULL;
    Client* result = NULL;
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client* query = &cls->items[i];
        if (query->clientId == id)
        {
            result = query;
            break;
        }
        else if (free == NULL && query->clientId == 0)
        {
            free = query;
        }
    }
    if (result == NULL)
    {
        if (free == NULL)
        {
            platform.Platform_Error("No free clients", "APP");
        }
        else
        {
            result = free;
            NetStream* stream = &result->stream;
            printf("APP Allocating I/O streams for client %d\n", id);
            u32 bytes = KiloBytes(64);
            stream->inputBuffer = Buf_FromMalloc(
                malloc(bytes), bytes
            );
            stream->outputBuffer = Buf_FromMalloc(
                malloc(bytes), bytes
            );
        }
    }
    result->clientId = id;
	if (id == -1)
	{
		result->isLocal = 1;
	}
    return result;
}

internal void App_UpdateLocalClients(GameTime* time, ClientList* cls)
{
    if (g_localClientId == 0) { return; }
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client* cl = &cls->items[i];
        //if (cl->clientId == 0) { continue; }
        if (cl->state == CLIENT_STATE_FREE) { continue; }
        if (cl->clientId == g_localClientId)
        {
            // Process input
            App_UpdateLocalClient(cl, &g_inputActions, time->gameFrameNumber);
        }
    }
}
