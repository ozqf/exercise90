#pragma once

#include "commands.h"

// Returns bytes written
internal i32 Cmd_Serialise(u8* write, Command* h, CmdSeq sequenceOffset)
{
    const u8* source = write;
    switch (h->type)
    {
        #if 1
        case CMD_TYPE_S2C_SYNC_ENTITY:
        {
            S2C_EntitySync* cmd = (S2C_EntitySync*)h;
            // Command type
            *write = h->type; write++;
            // Sub-type
            *write = cmd->type; write++;
            if (cmd->type == S2C_ENTITY_SYNC_TYPE_DEATH)
            {
                write += COM_WriteI32(cmd->networkId, write);
            }
            else
            {
                return COM_COPY_STRUCT(h, source, CmdImpulse);
            }
            return (write - source);
        } break;
        #endif
        case CMD_TYPE_IMPULSE:
        return COM_COPY_STRUCT(h, write, CmdImpulse);
        // Just dumb copy the bytes
        default:
        return COM_COPY(h, write, h->size);
    }
}
