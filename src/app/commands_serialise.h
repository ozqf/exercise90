#pragma once

#include "commands.h"

// Returns bytes written
internal i32 Cmd_Serialise(
    QuantiseSet* quantise,
    u8* write,
    Command* h,
    CmdSeq sequenceOffset)
{
    const u8* bufStart = write;
    switch (h->type)
    {
        #if 1
        case CMD_TYPE_S2C_SYNC_ENTITY:
        {
            S2C_EntitySync* cmd = (S2C_EntitySync*)h;
            // Command type
            *write = h->type; write++;
            // Sub-type
            *write = cmd->subType; write++;
            if (cmd->subType == S2C_ENTITY_SYNC_TYPE_DEATH)
            {
                write += COM_WriteI32(cmd->networkId, write);
                return (write - bufStart);
            }
            else
            {
                // Network Id, targetId, pos, vel
                write += COM_WriteI32(cmd->networkId, write);
                write += COM_WriteI32(cmd->update.targetId, write);

                write += COM_WriteU16((u16)COM_QuantiseF2I(
                    cmd->update.pos.x, &quantise->pos), write);
                write += COM_WriteU16((u16)COM_QuantiseF2I(
                    cmd->update.pos.y, &quantise->pos), write);
                write += COM_WriteU16(
                    (u16)COM_QuantiseF2I(cmd->update.pos.z, &quantise->pos), write);
                
                write += COM_WriteU16(
                    (u16)COM_QuantiseF2I(cmd->update.vel.x, &quantise->vel), write);
                write += COM_WriteU16(
                    (u16)COM_QuantiseF2I(cmd->update.vel.x, &quantise->vel), write);
                write += COM_WriteU16(
                    (u16)COM_QuantiseF2I(cmd->update.vel.x, &quantise->vel), write);
                return (write - bufStart);
                
                //return COM_COPY_STRUCT(h, bufStart, S2C_EntitySync);
            }
        } break;
        #endif
        case CMD_TYPE_IMPULSE:
        return COM_COPY_STRUCT(h, write, CmdImpulse);
        // Just dumb copy the bytes
        default:
        return COM_COPY(h, write, h->size);
    }
}
