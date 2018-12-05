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
    cl->connectionId = cmd->connectionId;
    printf("GAME EXEC Client %d conn %d, State: %d Avatar id %d/%d\n",
        cl->clientId, cl->connectionId, cl->state, cl->entId.iteration, cl->entId.index);
    
    if (disconnected)
    {
        App_ResetClient(cl);
    }
    // Changes that server should act on
    if (created)
    {
        SV_ProcessClientCreated(session, cl);
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

internal void Exec_EntitySync(GameScene* gs, Cmd_EntitySync* cmd)
{
    EntId id = cmd->entId;
    Ent* ent = Ent_GetEntityById(gs, &id);
    if (ent == NULL)
    { 
        printf("No entity %d/%d for sync\n", id.iteration, id.index);
        return;
    }
    EC_Transform* ect = EC_FindTransform(gs, &id);
    EC_Collider* col = EC_FindCollider(gs, &id);
    printf("@");
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
