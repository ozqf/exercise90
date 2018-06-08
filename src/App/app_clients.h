#pragma once

#include "app_module.cpp"

void App_EndAllClients()
{
	i32 len = g_clientList.max;
	for (i32 i = 0; i < g_clientList.max; ++i)
	{
		g_clientList.items[i] = {};
	}
}

void App_ClearClientGameLinks()
{
	i32 len = g_clientList.max;
	for (i32 i = 0; i < g_clientList.max; ++i)
	{
		Client* cl = &g_clientList.items[i];
		cl->entIdArr[0] = 0;
		cl->entIdArr[1] = 0;
		cl->state = CLIENT_STATE_OBSERVER;

	}
}

void App_AddLocalClient()
{

}


/////////////////////////////////////////////////////
// UPDATE CLIENTS
/////////////////////////////////////////////////////
void App_UpdateLocalClient(Client* cl, InputActionSet* actions, u32 frameNumber)
{
    switch (cl->state)
    {
        case CLIENT_STATE_OBSERVER:
        {
            #if 0
            if (Input_CheckActionToggledOn(actions, "Move Up", frameNumber))
            {
                Cmd_ServerImpulse cmd = {};
                cmd.clientId = cl->clientId;
                cmd.impulse = IMPULSE_JOIN_GAME;

                printf("APP: Client %d wishes to spawn\n", cl->clientId);
                App_EnqueueCmd((u8*)&cmd, CMD_TYPE_IMPULSE, sizeof(Cmd_ServerImpulse));
            }
            #endif
        } break;

        case CLIENT_STATE_PLAYING:
        {
			Cmd_PlayerInput cmd = {};
			cmd.clientId = cl->clientId;
			Game_CreateClientInput(&g_inputActions, &cmd.input);
			App_EnqueueCmd((u8*)&cmd, CMD_TYPE_PLAYER_INPUT, sizeof(Cmd_PlayerInput));
        } break;

        default:
        {
            char buf[256];
            sprintf_s(buf, 256, "APP Unknown client state %d\n", cl->state);
            platform.Platform_Error(buf, "APP");
            ILLEGAL_CODE_PATH
        } break;
    }
}

Client* App_FindClientById(i32 id)
{
    for (i32 i = 0; i < g_clientList.max; ++i)
    {
        Client* query = &g_clientList.items[i];
        if (query->clientId == id)
        {
            return query;
        }
    }
    return NULL;
}

Client* App_FindOrCreateClient(i32 id)
{
    Client* free = NULL;
    Client* result = NULL;
    for (i32 i = 0; i < g_clientList.max; ++i)
    {
        Client* query = &g_clientList.items[i];
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
            ILLEGAL_CODE_PATH
        }
        else
        {
            result = free;
        }
    }
    result->clientId = id;
	if (id == -1)
	{
		result->isLocal = 1;
	}
    return result;
}

void App_UpdateLocalClients(GameTime* time)
{
    i32 l = g_clientList.max;
    for (i32 i = 0; i < l; ++i)
    {
        Client* cl = &g_clientList.items[i];
        if (cl->clientId == 0) { continue; }
        if (cl->clientId == -1)
        {
            // Process input
            App_UpdateLocalClient(cl, &g_inputActions, time->frameNumber);
        }
    }
}

