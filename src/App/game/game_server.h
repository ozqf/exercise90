#pragma once
/*
Server only functions
*/
#include "app_module.cpp"

// internal void SV_UpdateLocalPlayers(GameScene* gs, GameTime* time)
// {

// }

internal u8 SV_ReadImpulse(
    u8 netMode,
    ClientList* clients,
    GameScene* gs,
    Cmd_ServerImpulse* cmd)
{
    if (!IsRunningServer(netMode))
	{
		printf("GAME Cannot impulse if not hosting the server!\n");
		return 1;
	}
    printf("SV EXEC impulse %d from client %d\n", cmd->impulse, cmd->clientId);
	switch (cmd->impulse)
	{
        case IMPULSE_NEXT_WEAPON:
        {
            Client* cl = App_FindClientById(cmd->clientId, clients);
            Assert(cl);
            if (cl->entId.value == 0) { return 1; }
            Ent* ent = Ent_GetEntityById(gs, &cl->entId);
            Assert(ent);
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &ent->entId);
            Assert(motor);
            motor->state.attack1Type++;
            if (motor->state.attack1Type >= 4)
            {
                motor->state.attack1Type = 1;
            }
            return  1;
        } break;


		case IMPULSE_JOIN_GAME:
		{
			// How to assign player Id at this point
            Client* cl = App_FindClientById(cmd->clientId, clients);
			APP_ASSERT(cl, "Client for join game impulse could not be found");
            if (cl->state != CLIENT_STATE_OBSERVER)
            {
                printf("SV Cannot spawn - client is free or playing already\n");
                return 1;
            }
			if (!gs->AllowPlayerSpawning())
			{
				printf("SV: Instance disallows player spawning\n");
				return 1;
			}
			
            EntitySpawnOptions options = {};
            options.pos.x = 0;
            options.pos.y = 0;
            options.pos.z = 7;
            EntId entId = Ent_QuickSpawnCmd(gs, ENTITY_TYPE3_ACTOR_GROUND, &options);
            
            printf("SV Spawning local client avatar %d/%d\n", entId.iteration, entId.index);

            Cmd_ClientUpdate clUpdate = {};
            clUpdate.clientId = cmd->clientId;
            clUpdate.state = CLIENT_STATE_PLAYING;
            clUpdate.entId = entId;
            APP_WRITE_CMD(0, clUpdate);
			return 1;
		} break;

		default:
		{
            printf("SV UNKNOWN IMPULSE %d from client %d\n", cmd->impulse, cmd->clientId);
			//ILLEGAL_CODE_PATH
			return 1;
		} break;
	}
}

/*
Server construction of a relevance list for a specific client.
> Returns the number of links created
> Ignore entities that are not in use (obviously)
> Ignore entities that are marked as 'static'. These entities are most likely
    world volumes that should never change
> Set priority here too, based on the ent's settings
> Future: Ignore entities based on location/distance/line of sight (not necessary
    whilst the whole game is one arena though!)

*/
/*internal*/ i32 SV_BuildRelevanceList(Client* cl, EntityLink* links, i32 maxLinks, EntList* ents)
{
    i32 count = 0;
    for (u32 i = 0; i < ents->count; ++i)
    {
        Ent* e = & ents->items[i];
        if (e->inUse == ENTITY_STATUS_IN_USE)
        {
            links->entId = e->entId;
            links->priority = e->priority;
            links->importance = 0;

            ++links;
            ++count;
        }
    }
    return count;
}

/*internal*/ void SV_IterateImportance(EntityLink* links, i32 numLinks)
{

    /*
    Network entity update priority queue:
    > Calculate a priority based on:
        > entity types' inherent priority
            (projectiles are higher than regular enemies etc)
        > Distance from this client
        > Are they targetting this client directly? This could 
            be used to cancel the distance effect
		> direction of movement: Projectiles moving away are
			not as important
    > Increase each entity link's importance by it's priority
    > Sort list by descending importance.
    > Iterate over sorted list, writing entity updates until
        packet is full. Reset importance to 0 for each entity
        written
    > Send packet of updates to client
    */
    for (i32 i = 0; i < numLinks; ++i)
    {
        Assert(links->priority > 0);
        links->importance += links->priority;
    }

    // Sort list by importance
}

internal void SV_TransmitClientListToClient(ClientList* cls, Client* receiver)
{
	if (receiver->clientId == cls->localClientId)
	{
		printf("SV Not transmitting client list - client is local!\n");
		return;
	}
    printf("SV Transmitting client list to cl %d\n", receiver->clientId);
	i32 receiverConnId = receiver->connectionId;
    for(i32 i = 0; i < cls->max; ++i)
    {
        Client* cl = &cls->items[i];
        if (cl->state == CLIENT_STATE_FREE)
		{ continue; }
        Cmd_ClientUpdate cmd;
        cmd.Set(cl, (receiver->connectionId == cl->connectionId));
		// override connectionId info. Only send an actual Id to the receiver
        NET_MSG_TO_OUTPUT(&receiver->stream, &cmd);
    }
}

internal void SV_ProcessClientCreated(GameSession* session, Client* cl)
{
    if (!IS_SERVER) { return; }
    //APP_ASSERT(cl->state == CLIENT_STATE_SYNC, "New client is not in sync state");
    // Tell the new client what their public Id is
    Cmd_Quick cmd = {};
    cmd.SetAsConfimClientId(cl->clientId);
    //cmd.data1 = CMD_QUICK_TYPE_CONFI_CLIENT_ID;
    //cmd.data2 = cl->clientId;
    NET_MSG_TO_OUTPUT(&cl->stream, &cmd);
    SV_TransmitClientListToClient(&session->clientList, cl);

    // scene sync
    #if 0
    Cmd_S2C_Sync sync = {};
    sync.length = (u8)sprintf_s(
        sync.fileName, CMD_SYNC_FILE_NAME_LENGTH,
        "TEST"
    );
    printf("SV Issuing sync cmd %s to CL %d\n", sync.fileName, cl->connectionId);
    NET_MSG_TO_OUTPUT(&cl->stream, &cmd);
    #endif
}

internal void SV_ProcessClientSpawn(u8 netMode, GameScene* gs, Client* cl, Cmd_ClientUpdate* cmd)
{
	if(!IsRunningServer(netMode)) { return; }
    if (gs->state == 0)
    {
		// TODO: Replicate client state change to all connected clients
		// Make sure connection Id is NOT replicated!
		cmd->connectionId = 0;
		
        printf("SV: GAME START\n");
        Cmd_GameSessionState sessionCmd = {};
        sessionCmd.state = GAME_SESSION_STATE_PLAYING;
        APP_WRITE_CMD(0, sessionCmd);
        //sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, "");
    }
}

internal void SV_ProcessClientDeath(
    u8 netMode,
    GameScene* gs,
    ClientList* clients,
    Client* cl,
    Cmd_ClientUpdate* cmd)
{
	if(!IsRunningServer(netMode)) { return; }
    if (App_NumPlayingClients(clients) == 0)
    {
        // TODO: Replicate client state change to all connected clients
		// Make sure connection Id is NOT replicated!
		cmd->connectionId = 0;
		
        printf("SV: GAME OVER\n");
        Cmd_GameSessionState sessionCmd = {};
        sessionCmd.state = GAME_SESSION_STATE_FAILED;
        APP_WRITE_CMD(0, sessionCmd);
    }
}

internal void SV_ProcessEntityDeath(u8 netMode, ClientList* clients, GameScene* gs, Ent* ent)
{
    if(!IsRunningServer(netMode)) { return; }
    // is the entity a player?
    Client* cl = App_FindClientByEntId(ent->entId, clients);
    if (cl)
    {
        printf("GAME Client %d avatar died\n", cl->clientId);
        Cmd_ClientUpdate clUpdate = {};
        clUpdate.clientId = cl->clientId;
        clUpdate.state = CLIENT_STATE_OBSERVER;
        clUpdate.entId = { };
        APP_WRITE_CMD(0, clUpdate);
    }
    // inform source entity
    EC_Thinker* thinker = EC_FindThinker(gs, &ent->source);
    if (thinker)
    {
        if (thinker->state.type == EC_THINKER_SPAWNER)
        {
            thinker->state.count--;
        }
    }
}

// TODO: MOVE ME: This isn't a server only function!
internal void Game_RemoveEntity(u8 netMode, ClientList* clients, GameScene* gs, Cmd_RemoveEntity* cmd)
{
    if (g_verbose)
    {
        printf("GAME Removing Ent %d/%d\n", cmd->entId.iteration, cmd->entId.index);
    }
    Ent* ent = Ent_GetEntityToRemoveById(&gs->entList, &cmd->entId);
	if (ent != NULL)
	{
        EC_Transform* ecT = EC_FindTransform(gs, ent);

        // spawn fx
		if (ecT)
		{
			Vec3 p = ecT->t.pos;
			Vec3 normal = COM_UnpackVec3Normal(cmd->gfxNormal);
			Game_SpawnLocalEntity(p.x, p.y, p.z, &normal, 1, LOCAL_ENT_TYPE_IMPACT);
		}
		else
		{
			printf("GAME Ent %d/%d type %d has no transform...?\n",
				ent->entId.iteration, ent->entId.index,
				ent->factoryType
			);
		}

        // inform interested parties
        SV_ProcessEntityDeath(netMode, clients, gs, ent);
		Ent_Free(gs, ent);
	}
	else
	{
		printf("GAME Ent %d/%d not found to remove!\n", cmd->entId.iteration, cmd->entId.index);
	}
}
