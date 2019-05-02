#pragma once

#include "commands.h"

// Returns bytes written
internal i32 Cmd_WriteToPacket(u8* bytes, Command* h, CmdSeq baseSequence)
{
    switch (h->type)
    {
        case CMD_TYPE_IMPULSE:
        return COM_COPY_STRUCT(h, bytes, CmdImpulse);
        // Just dumb copy the bytes
        default:
        return COM_COPY(h, bytes, h->size);
    }
}

// returns bytes read
internal i32 Cmd_ReadFromPacket(
    u8* source, u8* buffer, i32 capacity, CmdSeq baseSequence)
{
    // If dumb copying the bytes, don't step forward yet
    u8 type = *source;
    switch (type)
    {
        case CMD_TYPE_IMPULSE:
        return COM_COPY_STRUCT(source, buffer, CmdImpulse);
        break;
        case CMD_TYPE_S2C_HANDSHAKE:
        return COM_COPY_STRUCT(source, buffer, S2C_Handshake);
        break;
        case CMD_TYPE_S2C_SET_SCENE:
        return COM_COPY_STRUCT(source, buffer, CmdSetScene);
        break;
        case CMD_TYPE_S2C_RESTORE_ENTITY:
        return COM_COPY_STRUCT(source, buffer, S2C_RestoreEntity);
        break;
        case CMD_TYPE_S2C_BULK_SPAWN:
        return COM_COPY_STRUCT(source, buffer, S2C_Sync);
        break;
        case CMD_TYPE_S2C_SESSION_SYNC:
        return COM_COPY_STRUCT(source, buffer, S2C_Sync);
        break;
        case CMD_TYPE_C2S_INPUT:
        return COM_COPY_STRUCT(source, buffer, C2S_Input);
        break;
        case CMD_TYPE_S2C_SYNC_ENTITY:
        return COM_COPY_STRUCT(source, buffer, S2C_EntitySync);
        break;
        case CMD_TYPE_PING:
        return COM_COPY_STRUCT(source, buffer, CmdPing);
        break;
        case CMD_TYPE_S2C_INPUT_RESPONSE:
        return COM_COPY_STRUCT(source, buffer, S2C_InputResponse);
        break;
        case CMD_TYPE_S2C_REMOVE_ENTITY:
        return COM_COPY_STRUCT(source, buffer, S2C_RemoveEntity);
        break;
        case CMD_TYPE_S2C_REMOVE_ENTITY_GROUP:
        return COM_COPY_STRUCT(source, buffer, S2C_RemoveEntityGroup);
        break;
        default:
        // Don't know size of command automatically, so
        // just give up
        COM_ASSERT(0, "No read function for command");
        break;
    }
    return 0;
}
