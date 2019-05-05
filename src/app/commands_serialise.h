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
                write += COM_WriteU16((u16)COM_QuantiseF2I(
                    cmd->update.pos.z, &quantise->pos), write);
                
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
        case CMD_TYPE_S2C_BULK_SPAWN:
        {
            return COM_COPY_STRUCT(h, write, S2C_BulkSpawn);
            #if 0
            /*
            1 u8 type
            1 u8 sequenceOffset
            4 i32 tick
            4 i32 firstSerial
            4 i32 sourceSerial
            4 i32 forwardNormal
            2 u16 x
            2 u16 y
            2 u16 z
            2 u16 radius
            1 u8 patternIndex
            1 u8 numItems
            1 u8 seedIndex
            */
            S2C_BulkSpawn* cmd = (S2C_BulkSpawn*)h;
            *write = CMD_TYPE_S2C_BULK_SPAWN; write++;
            *write = (u8)sequenceOffset; write++;
            write += COM_WriteI32(cmd->header.tick, write);
            write += COM_WriteI32(cmd->def.base.firstSerial, write);
            write += COM_WriteI32(cmd->def.base.sourceSerial, write);
            // normal
            i32 normal = COM_PackVec3NormalToI32(cmd->def.base.pos.parts);
            write += COM_WriteI32(normal, write);
            // pos
            write += COM_WriteU16((u16)COM_QuantiseF2I(
                cmd->def.base.pos.x, &quantise->pos), write);
            write += COM_WriteU16((u16)COM_QuantiseF2I(
                cmd->def.base.pos.y, &quantise->pos), write);
            write += COM_WriteU16((u16)COM_QuantiseF2I(
                cmd->def.base.pos.z, &quantise->pos), write);
            // radius
            write += COM_WriteU16((u16)COM_QuantiseF2I(
                cmd->def.patternDef.radius, &quantise->pos), write);
            
            *write = (u8)cmd->def.patternDef.patternId; write++;
            *write = (u8)cmd->def.patternDef.numItems; write++;
            *write = cmd->def.base.seedIndex; write++;
           return (write - bufStart);
           #endif
        } break;
        #endif
        case CMD_TYPE_IMPULSE:
        return COM_COPY_STRUCT(h, write, CmdImpulse);
        // Just dumb copy the bytes
        default:
        return COM_COPY(h, write, h->size);
    }
}
