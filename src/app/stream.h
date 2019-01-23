#pragma once
///////////////////////////////////////////////////////////
// Header only - commands to/from NetStream
// Does not do anything related to packets or compression
// other than store transmission records.
///////////////////////////////////////////////////////////
#include "../common/com_module.h"
#include "commands_base.h"

internal TransmissionRecord* Stream_AssignTransmissionRecord(
        TransmissionRecord* records,
        u32 sequence)
{
    TransmissionRecord* rec = &records[sequence % MAX_TRANSMISSION_RECORDS];
    rec->sequence = sequence;
    rec->numReliableMessages = 0;
    return rec;
}

internal TransmissionRecord* Stream_FindTransmissionRecord(
    TransmissionRecord* records, u32 sequence)
{
    for (i32 i = 0; i < MAX_TRANSMISSION_RECORDS; ++i)
    {
        if (records[i].sequence == sequence)
        {
            return &records[i];
        }
    }
    return NULL;
}

internal Command* Stream_FindMessageBySequence(u8* ptr, i32 numBytes, i32 sequence)
{
    u8* read = ptr;
    u8* end = ptr + numBytes;
    while (read < end)
    {
        Command* header = (Command*)read;
        Assert(Cmd_Validate(header) == COM_ERROR_NONE)
        if (header->sequence == sequence) { return header; }
        read += header->size;
    }
    return NULL;
}

/**
 * Before:
 * blockStart* (space ---- )(rest of buffer ---- )bufferEnd*
 * After:
 * blockStart* (rest of buffer ---- )bufferEnd*
 */
internal void Stream_DeleteCommand(ByteBuffer* b, Command* cmd)
{
    Assert(Cmd_Validate(cmd) == COM_ERROR_NONE)
    u8* start = b->ptrStart;
    u8* bufEnd = start + b->Written();
    u8* cmdPtr = (u8*)cmd;
    Assert(cmdPtr >= start)
    Assert(cmdPtr < bufEnd)

    i32 bytesToDelete = cmd->size;
	u8* copyBlockDest = (u8*)cmd;
    u8* copyBlockStart = (u8*)cmd + bytesToDelete;
    i32 bytesToCopy = bufEnd - copyBlockStart;
    printf("Deleting %d bytes. Copying %d bytes\n", bytesToDelete, bytesToCopy);
	COM_CopyMemory(copyBlockStart, copyBlockDest, bytesToCopy);
	b->ptrWrite -= bytesToDelete;
}

internal void Stream_DeleteCommandBySequence(ByteBuffer* b, i32 sequence)
{
	Command* cmd = Stream_FindMessageBySequence(b->ptrStart, b->Written(), sequence);
	if (cmd)
	{
		Stream_DeleteCommand(b, cmd);
	}
}

internal void Stream_ClearReceivedOutput(
	NetStream* stream, u32 packetSequence)
{
    TransmissionRecord* rec = Stream_FindTransmissionRecord(
		stream->transmissions, packetSequence);
    ByteBuffer* b = &stream->outputBuffer;
    if (!rec)
    {
        return;
    }
    i32 numMessages = rec->numReliableMessages;
    for (i32 i = 0; i < numMessages; ++i)
    {
        i32 seq = rec->reliableMessageIds[i];
        Command* cmd = Stream_FindMessageBySequence(
			b->ptrStart, b->Written(), seq);
        if (cmd == NULL) { continue; }
        Stream_DeleteCommand(b, cmd);
    }
}

internal void Stream_EnqueueOutput(NetStream* stream, Command* cmd)
{
    i32 error = Cmd_Validate(cmd);
    if (error != COM_ERROR_NONE)
    {
        printf("STREAM cmd for enqueue is invalid. Code %d\n", error);
        return;
    }
    cmd->sequence = stream->outputSequence++;
    ByteBuffer* b = &stream->outputBuffer;
    Assert(b->Space() >= cmd->size);
    b->ptrWrite += COM_CopyMemory((u8*)cmd, b->ptrWrite, cmd->size);
}
