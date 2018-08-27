#pragma once
/*
Server only functions
*/
#include "app_module.cpp"

internal void SV_UpdateLocalPlayers(GameState* gs, GameTime* time)
{

}

internal u8 SV_ReadImpulse(GameState* gs, Cmd_ServerImpulse* cmd)
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
			// How to assign player Id at this point
            Client* cl = App_FindClientById(cmd->clientId, &gs->clientList);
            Assert(cl);
            if (cl->state != CLIENT_STATE_OBSERVER)
            {
                printf("GAME Cannot spawn - client is free or playing already\n");
                return 1;
            }

            EntitySpawnOptions options = {};
            options.pos.x = 0;
            options.pos.y = 0;
            options.pos.z = 7;
            EntId entId = Game_WriteSpawnCmd(gs, ENTITY_TYPE_ACTOR_GROUND, &options);
            
            printf("SV Spawning local client avatar %d/%d\n", entId.iteration, entId.index);

            Cmd_ClientUpdate clUpdate = {};
            clUpdate.clientId = cmd->clientId;
            clUpdate.state = CLIENT_STATE_PLAYING;
            clUpdate.entId = entId;
            APP_WRITE_CMD(0, CMD_TYPE_CLIENT_UPDATE, 0, clUpdate);
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
internal i32 SV_BuildRelevanceList(Client* cl, EntityLink* links, i32 maxLinks, EntList* ents)
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

internal void SV_IterateImportance(EntityLink* links, i32 numLinks)
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

void SV_DontRunMe()
{
    ILLEGAL_CODE_PATH
    SV_BuildRelevanceList(NULL, NULL, 0, NULL);
    SV_IterateImportance(NULL, 0);
    SV_UpdateLocalPlayers(NULL, NULL);
}

internal void SV_RemoveEntity(GameState* gs, Cmd_RemoveEntity* cmd)
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
			Game_SpawnLocalEntity(p.x, p.y, p.z, &normal, 1);
		}
		else
		{
			printf("GAME Ent %d/%d type %d has no transform...?\n",
				ent->entId.iteration, ent->entId.index,
				ent->factoryType
			);
		}

        // inform interested parties

        // is the entity a player?
        Client* cl = App_FindClientByEntId(cmd->entId, &gs->clientList);
        if (cl)
        {
            // Do stuff
            printf("GAME Client %d avatar died\n", cl->clientId);
            Cmd_ClientUpdate clUpdate = {};
            clUpdate.clientId = cl->clientId;
            clUpdate.state = CLIENT_STATE_OBSERVER;
            clUpdate.entId = { };
            APP_WRITE_CMD(0, CMD_TYPE_CLIENT_UPDATE, 0, clUpdate);
        }
        
		Ent_Free(gs, ent);
	}
	else
	{
		printf("GAME Ent %d/%d not found to remove!\n", cmd->entId.iteration, cmd->entId.index);
	}
}
