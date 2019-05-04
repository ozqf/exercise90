#pragma once

#include "commands.h"

// Returns bytes written
internal i32 Cmd_Serialise(u8* bytes, Command* h, CmdSeq sequenceOffset)
{
    u8* write = bytes;
    switch (h->type)
    {
        #if 0
        case CMD_TYPE_S2C_SYNC_ENTITY:
        {
            S2C_EntitySync* cmd = (S2C_EntitySync*)h;
            *write = h->type; write++;
            if (cmd->type == S2C_ENTITY_SYNC_TYPE_DEATH)
            {

            }
            else
            {

            }
            return (write - bytes);
        } break;
        #endif
        case CMD_TYPE_IMPULSE:
        return COM_COPY_STRUCT(h, write, CmdImpulse);
        // Just dumb copy the bytes
        default:
        return COM_COPY(h, write, h->size);
    }
}
