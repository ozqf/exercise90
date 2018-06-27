#pragma once
/*
Server only functions
*/
#include "app_module.cpp"

void SV_UpdateLocalPlayers(GameState* gs, GameTime* time)
{

}

u8 SV_ReadImpulse(GameState* gs, Cmd_ServerImpulse* cmd)
{
    if (!IsRunningServer(gs->netMode))
	{
		printf("GAME Cannot impulse if not hosting the server!\n");
		return 1;
	}
    printf("SV EXEC impulse %d from client %d\n", cmd->impulse, cmd->clientId);
	switch (cmd->impulse)
	{
		case IMPULSE_JOIN_GAME:
		{
            #if 0
			printf("SV Client %d impulse %d: Spawn player\n", cmd->clientId, cmd->impulse);

            Player* plyr = SV_FindPlayerByClientId(&gs->playerList, cmd->clientId);
            if (plyr != NULL)
            {
                printf("  Client already has a player.\n");
                return 1;
            }

            Cmd_PlayerState s = {};
            SV_CreateNewPlayer(&gs->playerList, cmd->clientId, &s);

            printf("  Created player %d for client %d\n", s.playerId, cmd->clientId);

            App_WriteGameCmd((u8*)&s, CMD_TYPE_PLAYER_STATE, sizeof(Cmd_PlayerState));

            return 1;
            #endif
            
            #if 1
			// How to assign player Id at this point
            Client* cl = App_FindClientById(cmd->clientId);
            if (cl->state != CLIENT_STATE_OBSERVER)
            {
                printf("GAME Cannot spawn - client is free or playing already\n");
                return 1;
            }
            
			Cmd_EntityState spawn = {};
			spawn.factoryType = ENTITY_TYPE_ACTOR_GROUND;
			spawn.entityId = Ent_ReserveFreeEntity(&gs->entList);
			
            Cmd_ClientUpdate clUpdate = {};
            clUpdate.clientId = cmd->clientId;
            clUpdate.state = CLIENT_STATE_PLAYING;
            clUpdate.entId = spawn.entityId;

            printf("SV Write CMD %d\n", CMD_TYPE_ENTITY_STATE);
            printf("SV Write CMD %d\n", CMD_TYPE_CLIENT_UPDATE);

            App_WriteGameCmd((u8*)&spawn, CMD_TYPE_ENTITY_STATE, sizeof(Cmd_EntityState));
            App_WriteGameCmd((u8*)&clUpdate, CMD_TYPE_CLIENT_UPDATE, sizeof(Cmd_ClientUpdate));

            // Exec_Spawn(gs, &spawn);
            // Exec_UpdateClient(&clUpdate);
			return 1;
            #endif
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
i32 SV_BuildRelevanceList(Client* cl, EntityLink* links, i32 maxLinks, EntList* ents)
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

void SV_IterateImportance(EntityLink* links, i32 numLinks)
{

    /*
    Network entity update priority queue:
    > Calculate a priority based on the 
        > entity types' inherent priority
            (projectiles are higher than regular enemies etc)
        > Distance from this client
        > Are they targetting this client directly? This could 
            be used to cancel the distance effect
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
