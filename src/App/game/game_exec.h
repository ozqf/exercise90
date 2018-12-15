#pragma once

#include "game.h"



internal void Exec_UpdateClient(GameSession* session, GameScene* gs, Cmd_ClientUpdate* cmd)
{
    APP_ASSERT(App_IsClientStateValid(cmd->state), "Exec_ClientUpdate: state is invalid");

    // Detected change events
    i32 spawn = 0, death = 0, created = 0, disconnected = 0;

    Client* cl = App_FindClientById(cmd->clientId, &session->clientList);
    if (cl == NULL)
    {
        cl = App_CreateClient(cmd->clientId, cmd->connectionId, &session->clientList);
        created = 1;
    }
    printf("Client avatar was %d/%d now %d/%d\n",
        cl->entId.iteration, cl->entId.index,
        cmd->entId.iteration, cmd->entId.index
    );
    spawn = (cl->entId.value == 0 && cmd->entId.value != 0);
    death = (cl->entId.value != 0 && cmd->entId.value == 0);
    disconnected = (cl->state != CLIENT_STATE_FREE && cmd->state == CLIENT_STATE_FREE);

    cl->state = cmd->state;
    cl->entId = cmd->entId;
    // DON'T copy connectionId. It is set by the server on connect only
    // and never known by clients
    //cl->connectionId = cmd->connectionId;
    printf("GAME EXEC Client %d conn %d, State: %d Avatar id %d/%d\n",
        cl->clientId, cl->connectionId, cl->state, cl->entId.iteration, cl->entId.index);
    
    if (disconnected)
    {
        App_ResetClient(cl);
    }
    // Changes that server should act on
    if (created)
    {
        SV_ProcessClientCreated(session, gs, cl);
    }
    if (spawn)
    {
        SV_ProcessClientSpawn(session->netMode, gs, cl, cmd);
    }
    if (death)
    {
        // deth!
        SV_ProcessClientDeath(session->netMode, gs, &session->clientList, cl, cmd);
    }
    #if 0
    if (cl->flags != cmd->flags)
    {
        // Find changes
        if (COM_IsFlagDifferent(cmd->flags, cl->flags, CLIENT_FLAG_LOCAL))
        {
            // This won't work, remote clients will replicate this and break things!
            //g_localClientId = cmd->clientId;
            printf("GAME Client flag %d changed\n", CLIENT_FLAG_LOCAL);
        }
    }
    cl->flags = cmd->flags;
    #endif

    if (IS_SERVER)
    {
        printf("SV TRANSMIT cl update to all clients\n");
        // TODO: Could transmit the private connection Id to a new client here,
        // to avoid sending this information twice (once in a quick msg, then again here)
        cmd->connectionId = 0;
        NET_MSG_TRANSMIT_TO_ALL_CLIENTS((&session->clientList), cmd);
    }
    
    //SV_OutputToAllClients(session->netMode, &session->clientList, gs, cmd);
}

internal void Exec_UpdateGameInstance(GameScene* gs, Cmd_GameSessionState* cmd)
{
    if (gs->state == cmd->state) { return; }
	printf("GAME Session state changing. %d to %d\n", gs->state, cmd->state);
	gs->state = cmd->state;
    if (cmd->state == GAME_SESSION_STATE_PLAYING)
    {
        sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, "");
    }
    else if (cmd->state == GAME_SESSION_STATE_FAILED)
    {
        sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, "GAME OVER");
    }
    else if (cmd->state == GAME_SESSION_STATE_SUCCESS)
    {
        sprintf_s(g_hud_centreText, HUD_CENTRE_TEXT_LENGTH, "SUCCESS");
    }
}

internal void Exec_SpawnViaTemplate(GameScene* gs, Cmd_SpawnViaTemplate* cmd)
{
	
}

internal void Exec_QuickCommand(GameSession* session, GameScene* gs, Cmd_Quick* cmd)
{
    switch (cmd->quickType)
    {
        case CMD_QUICK_TYPE_PACKET_DELIVERED:
        {
            Net_ProcessPacketDelivery(session, cmd->connectionId, cmd->packetNumber);
        } break;
        case CMD_QUICK_TYPE_CONFIRM_CLIENT_ID:
        {
            session->clientList.localClientId = cmd->clientId;
            //APP_ASSERT(false, "Set local client Id from quick cmd");
            printf(">>> GAME Set local client Id: %d\n",
                session->clientList.localClientId);
        } break;
        case CMD_QUICK_TYPE_CONNECTION_LOST:
        {
            printf(">>> GAME: Issuing disconnect text command");
            App_WriteTextCommand("DISCONNECT");
        } break;
        default:
        {
            printf("GAME Unknown quick command type: %d\n", cmd->quickType);
        } break;
    }
}

internal void Exec_S2CSync(GameSession* session, GameScene* gs, Cmd_S2C_Sync* cmd)
{
    printf("S2C Sync: %s\n", cmd->fileName);
    App_LoadScene("TEST");
}

internal void Exec_EntitySync(GameScene* gs, Cmd_ActorSync* cmd)
{
    EntId id = cmd->entId;
    Ent* ent = Ent_GetEntityById(gs, &id);
    if (ent == NULL)
    { 
        printf("No sync %d/%d", id.iteration, id.index);
        return;
    }
    EC_Transform* ect = EC_FindTransform(gs, &id);
    EC_Collider* col = EC_FindCollider(gs, &id);
    
    if (col == NULL)
    {
        ect->t.pos = cmd->pos;
        //ect->t.rotation = cmd->rot;
    }
    else
    {
        PhysCmd_TeleportShape(col->shapeId, cmd->pos.x, cmd->pos.y, cmd->pos.z);
    }
}

internal i32 Game_ReadCmd(
    GameSession* session,
    GameScene* gs,
    CmdHeader* header,
    u8* read,
    i32 verbose)
{
    ClientList* clients = &session->clientList;
    u8 type = *read;
    //switch (header->GetType())
    if (verbose) { printf("GAME Exec cmd type %d\n", type); }
    switch (type)
    {
        case CMD_TYPE_ENTITY_SYNC:
        {
            if (!IS_CLIENT) { printf("SV cannot exec entity sync commands!\n"); return 1; }
            Cmd_ActorSync cmd = {};
            cmd.Read(read);
            Exec_EntitySync(gs, &cmd);
            return COM_ERROR_NONE;
        } break;

        case CMD_TYPE_ENTITY_STATE_2:
        {
            if (verbose)
            {
                printf("GAME reading Entity state stream cmd (%d bytes)\n", header->GetSize());
            }
            Ent_ReadStateData(gs, read, header->GetSize(), gs->Verbose());
            
			if (IS_SERVER)
			{
                // TODO:: Entity state doesn't work directly through a command object
                // and requires this stuff to send it instead...
                for (i32 i = 0; i < session->clientList.max; ++i)
				{
					Client* cl = &session->clientList.items[i];
					// check in use or local
					if (cl->state == CLIENT_STATE_FREE) { continue; }
                    // ...Actually, send to client
					//if (cl->connectionId == 0) { continue; }

					NetStream* s = &cl->stream;
					ByteBuffer* b = &s->outputBuffer;
					u32 msgId = ++s->outputSequence;
					i32 requiredSpace = header->GetSize() + sizeof(StreamMsgHeader);
					i32 space = b->Space();
                    if (space < requiredSpace)
                    {
                        printf("CLIENT %d has no output capacity (has %d need %d, written %d)!\n",
							cl->connectionId, b->Space(), requiredSpace, b->Written());
						if (space > 60000)
						{
							printf(" WTF? Space left is %d!\n", space);
						}
                        // Remote client and No output capacity?
                        // Drop client!
                        continue;
                    }
                    //printf("SV Write state of ent to %d\n", cl->connectionId);
					b->ptrWrite += Stream_WriteStreamMsgHeader(b->ptrWrite, msgId, header->GetSize(), 0.1f);
					b->ptrWrite += COM_COPY(read, b->ptrWrite, header->GetSize());
				}
			}
            return COM_ERROR_NONE;
        } break;

        case CMD_TYPE_PACKET:
        {
            if (verbose) { printf("  GAME Packet cmd\n"); }
            //Cmd_Packet cmd = {};
			// Read packet info
            read += sizeof(u8);
            i32 connId = COM_ReadI32(&read);
            i32 numBytes = COM_ReadI32(&read);
            //read += COM_COPY_STRUCT(read, &cmd, Cmd_Packet);

            // Read should now be at the start of the packet block!
            Net_ProcessPacket(connId, read, (u16)numBytes);
            //Net_ProcessPacket(cmd.connectionId, read, (u16)cmd.numBytes);
            return COM_ERROR_NONE;
        } break;

        case CMD_TYPE_ENTITY_STATE:
        {
            printf("GAME: Defunct dynamic state call\n");
            #if 0
            Cmd_EntityState cmd = {};
            read += cmd.Read(header, read);
            if (gs->verbose)
            {
                printf("EXEC spawn dynamic ent %d/%d type %d\n",
                    cmd.entityId.iteration, cmd.entityId.index, cmd.factoryType
                );
            }
            Exec_DynamicEntityState(gs, &cmd);
            #endif
			return COM_ERROR_NONE;
        } break;
		
		case CMD_TYPE_SPAWN_VIA_TEMPLATE:
		{
			Cmd_SpawnViaTemplate cmd = {};
			read += cmd.Read(read);
			Exec_SpawnViaTemplate(gs, &cmd);
			return COM_ERROR_NONE;
		} break;

        case CMD_TYPE_STATIC_STATE:
        {
            printf("GAME: Defunct static state call\n");
            //Cmd_Spawn cmd = {};
            //read += cmd.Read(read);
            //Exec_StaticEntityState(gs, &cmd);
            return COM_ERROR_NONE;
        } break;

        case CMD_TYPE_REMOVE_ENT:
        {
            Cmd_RemoveEntity cmd = {};
            read += cmd.Read(read);
            Game_RemoveEntity(session->netMode, clients, gs, &cmd);

            if (IS_SERVER)
            {
                NET_MSG_TRANSMIT_TO_ALL_CLIENTS((&session->clientList), &cmd);
            }

            return COM_ERROR_NONE;
        } break;
        
		case CMD_TYPE_PLAYER_INPUT:
		{
            Cmd_PlayerInput cmd;
            u16 measuredSize = cmd.MeasureForReading(read);
            APP_ASSERT(measuredSize == header->GetSize(), "Command read size mismatch");
            read += cmd.Read(read);
            Client* cl = App_FindClientById(cmd.clientId, clients);
            Assert(cl != NULL);
            //Ent* ent = Ent_GetEntityById(&gs->entList, (EntId*)&cl->entIdArr);
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &cl->entId);
			if (motor == NULL)
			{
				//printf("!GAME No motor for Entity %d/%d\n",
                //    cl->entId.iteration,
                //    cl->entId.index);
				return COM_ERROR_NONE;
			}
            motor->state.input = cmd.input;
			return COM_ERROR_NONE;
		} break;

        case CMD_TYPE_QUICK:
        {
            Cmd_Quick cmd = {};
            read += cmd.Read(read);
            Exec_QuickCommand(session, gs, &cmd);
            return COM_ERROR_NONE;
        } break;
        
        case CMD_TYPE_IMPULSE:
        {
            Cmd_ServerImpulse cmd = {};
            read += cmd.Read(read);
			return SV_ReadImpulse(session->netMode, clients, gs, &cmd);
        } break;

        case CMD_TYPE_CLIENT_UPDATE:
        {
            if (verbose) { printf("  CMD Client input\n"); }
            Cmd_ClientUpdate cmd = {};
            read += cmd.Read(read);
            Exec_UpdateClient(session, gs, &cmd);
            return COM_ERROR_NONE;
        } break;
		
		case CMD_TYPE_GAME_SESSION_STATE:
		{
			Cmd_GameSessionState cmd = {};
			read += cmd.Read(read);
			Exec_UpdateGameInstance(gs, &cmd);
			return COM_ERROR_NONE;
		} break;

        case CMD_TYPE_SPAWN_HUD_ITEM:
        {
            Cmd_SpawnHudItem cmd = {};
            read += cmd.Read(read);
            Exec_SpawnHudItem(gs, &cmd);
            return COM_ERROR_NONE;
        } break;

        case CMD_TYPE_S2C_SYNC:
        {
            Cmd_S2C_Sync cmd = {};
            read += cmd.Read(read);
            Exec_S2CSync(session, gs, &cmd);
            return COM_ERROR_NONE;
        }
    }
    return COM_ERROR_UNKNOWN_COMMAND;
}
