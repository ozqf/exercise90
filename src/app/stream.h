#pragma once
///////////////////////////////////////////////////////////
// Header only - commands to/from NetStream
// Does not do anything related to packets or compression
// other than store transmission records.
///////////////////////////////////////////////////////////
#include "../common/common.h"
#include "commands_base.h"

#define MAX_PACKET_TRANSMISSION_MESSAGES 64
#define MAX_PACKET_SYNC_MESSAGES 64
struct TransmissionRecord
{
	u32 sequence;
	u32 numReliableMessages;
	u32 reliableMessageIds[MAX_PACKET_TRANSMISSION_MESSAGES];
	u32 numSyncMessages;
	i32 syncIds[MAX_PACKET_SYNC_MESSAGES];
};

struct PacketStats
{
	i32 totalBytes;
    i32 reliableBytes;
    i32 unreliableBytes;
	i32 numReliableMessages;
	i32 numReliableSkipped;
	i32 numUnreliableMessages;
	i32 commandCounts[256];
};

struct StreamStats
{
	i32 numPackets;
	i32 totalBytes;
    i32 reliableBytes;
    i32 unreliableBytes;
	i32 numReliableMessages;
	i32 numReliableSkipped;
	i32 numUnreliableMessages;
	i32 commandCounts[256];
};

struct ReliableCmdQueueStats
{
    i32 count;
    CmdSeq highestSeq;
    CmdSeq lowestSeq;
};

#define MAX_TRANSMISSION_RECORDS 33
struct NetStream
{
	// Has allocated buffers
	i32 initialised;
    // latest reliable command from remote executed here
    CmdSeq          inputSequence;
    ByteBuffer      inputBuffer;

    // id of next reliable message sent to remote
    CmdSeq          outputSequence;
    ByteBuffer      outputBuffer;
    // the most recented remotely acknowledged message Id
    u32             ackSequence;
	
	AckStream		ackStream;

    TransmissionRecord transmissions[MAX_TRANSMISSION_RECORDS];
};

internal void COM_InitStream(NetStream* stream, ByteBuffer input, ByteBuffer output)
{
    stream->inputBuffer = input;
    stream->outputBuffer = output;
    stream->inputSequence = 0;
}

internal ReliableCmdQueueStats Stream_CountCommands(ByteBuffer* b)
{
    ReliableCmdQueueStats result = {};
    result.lowestSeq = CMD_SEQ_MAX;
    u8* read = b->ptrStart;
    u8* end = b->ptrWrite;
    while (read < end)
    {
        Command* cmd = (Command*)read;
        i32 err = Cmd_Validate(cmd);
        if (err)
        {
            return result;
        }
        if (cmd->sequence < result.lowestSeq) { result.lowestSeq = cmd->sequence; }
        if (cmd->sequence > result.highestSeq) { result.highestSeq = cmd->sequence; }
        read += cmd->size;
        result.count++;
    }
    return result;
}

internal TransmissionRecord* Stream_AssignTransmissionRecord(
        TransmissionRecord* records,
        u32 sequence)
{
    TransmissionRecord* rec = &records[sequence % MAX_TRANSMISSION_RECORDS];
    rec->sequence = sequence;
    rec->numReliableMessages = 0;
    rec->numSyncMessages = 0;
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
        ErrorCode err = Cmd_Validate(header);
        COM_ASSERT(err == COM_ERROR_NONE,
            "Invalid command")
        if (header->sequence == sequence)
        { 
            return header;
        }
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
internal void Stream_DeleteCommand(ByteBuffer* b, Command* cmd, i32 remainingSpace)
{
    ErrorCode err = Cmd_Validate(cmd); 
    COM_ASSERT(err == COM_ERROR_NONE, "Invalid command")
    u8* start = b->ptrStart;
    u8* bufEnd = start + b->Written();
    u8* cmdPtr = (u8*)cmd;
    COM_ASSERT(cmdPtr >= start, "Cmd position < buffer start")
    COM_ASSERT(cmdPtr < bufEnd, "Cmd position > buffer end")
    
    i32 bytesToDelete = cmd->size - remainingSpace;
	u8* copyBlockDest = cmdPtr + remainingSpace;
    u8* copyBlockStart = (u8*)cmd + cmd->size;
    i32 bytesToCopy = bufEnd - copyBlockStart;
    // printf("Deleting %d bytes. Copying %d bytes\n", bytesToDelete, bytesToCopy);
	COM_CopyMemory(copyBlockStart, copyBlockDest, bytesToCopy);
	b->ptrWrite -= bytesToDelete;
}

internal void Stream_DeleteCommand_Original(ByteBuffer* b, Command* cmd, i32 remainingSpace)
{
    ErrorCode err = Cmd_Validate(cmd); 
    COM_ASSERT(err == COM_ERROR_NONE, "Invalid command")
    u8* start = b->ptrStart;
    u8* bufEnd = start + b->Written();
    u8* cmdPtr = (u8*)cmd;
    COM_ASSERT(cmdPtr >= start, "Cmd position < buffer start")
    COM_ASSERT(cmdPtr < bufEnd, "Cmd position > buffer end")

    i32 bytesToDelete = cmd->size;
	u8* copyBlockDest = (u8*)cmd;
    u8* copyBlockStart = (u8*)cmd + bytesToDelete;
    i32 bytesToCopy = bufEnd - copyBlockStart;
    // printf("Deleting %d bytes. Copying %d bytes\n", bytesToDelete, bytesToCopy);
	COM_CopyMemory(copyBlockStart, copyBlockDest, bytesToCopy);
	b->ptrWrite -= bytesToDelete;
}

internal void Stream_DeleteCommandBySequence(ByteBuffer* b, i32 sequence)
{
	Command* cmd = Stream_FindMessageBySequence(
        b->ptrStart, b->Written(), sequence);
	if (cmd)
	{
		Stream_DeleteCommand(b, cmd, 0);
	}
}

internal TransmissionRecord* Stream_ClearReceivedOutput(
	NetStream* stream, u32 packetSequence)
{
    TransmissionRecord* rec = Stream_FindTransmissionRecord(
		stream->transmissions, packetSequence);
    ByteBuffer* b = &stream->outputBuffer;
    if (!rec)
    {
        return NULL;
    }
    i32 numMessages = rec->numReliableMessages;
    for (i32 i = 0; i < numMessages; ++i)
    {
        i32 seq = rec->reliableMessageIds[i];
        Command* cmd = Stream_FindMessageBySequence(
			b->ptrStart, b->Written(), seq);
        if (cmd == NULL) { continue; }
        Stream_DeleteCommand(b, cmd, 0);
    }
    return rec;
}

#if 0
internal void Stream_ProcessPacketAcks(NetStream* stream, u32* packetAcks, i32 numPacketAcks)
{
	for (i32 i = 0; i < numPacketAcks; ++i)
	{
		Stream_ClearReceivedOutput(stream, packetAcks[i]);
	}
}
#endif

// returns bytes written
internal i32 Stream_EnqueueUnreliableInput(
    NetStream* stream, Command* cmd)
{
    //printf("CL Attempting to enqueue %d\n", cmd->sequence);
    i32 error = Cmd_Validate(cmd);
    if (error != COM_ERROR_NONE)
    {
        printf("STREAM cmd for enqueue is invalid. Code %d\n", error);
        return 0;
    }
    ByteBuffer* b = &stream->inputBuffer;
    COM_ASSERT(b->Space() >= cmd->size, "Unreliable stream is full");
    b->ptrWrite += COM_CopyMemory((u8*)cmd, b->ptrWrite, cmd->size);
    return cmd->size;
} 

// returns bytes written
internal i32 Stream_EnqueueReliableInput(
    NetStream* stream, Command* cmd)
{
    //printf("CL Attempting to enqueue %d\n", cmd->sequence);
    i32 error = Cmd_Validate(cmd);
    if (error != COM_ERROR_NONE)
    {
        printf("STREAM cmd for enqueue is invalid. Code %d\n", error);
        return 0;
    }
    ByteBuffer* b = &stream->inputBuffer;
    //If the current input sequence is higher or the command is already
    //buffered, ignore
    if ((u32)cmd->sequence < stream->inputSequence)
    {
        //printf("    CL ignored input: sequence %d < %d\n",
        //    cmd->sequence, stream->inputSequence);
        return 0;
    }
    Command* current = Stream_FindMessageBySequence(
        stream->inputBuffer.ptrStart,
        stream->inputBuffer.Written(),
        cmd->sequence);
    if (current == NULL)
    {
        //printf("CL Enqueuing CMD %d\n", cmd->sequence);
        COM_ASSERT(b->Space() >= cmd->size, "Reliable stream is full");
        b->ptrWrite += COM_CopyMemory((u8*)cmd, b->ptrWrite, cmd->size);
        return cmd->size;
    }
    else
    {
        //printf("    CL ignored input: sequence %d already queued\n",
        //    cmd->sequence);
        return 0;
    }
}

internal void Stream_EnqueueOutput(NetStream* stream, Command* cmd)
{
    i32 error = Cmd_Validate(cmd);
    if (error != COM_ERROR_NONE)
    {
        APP_LOG(64, "STREAM cmd for enqueue is invalid. Code %d\n", error);
        return;
    }
    // Only place where sequence should be set
    cmd->sequence = stream->outputSequence++;
    
    ByteBuffer* b = &stream->outputBuffer;
    COM_ASSERT(b->Space() >= cmd->size, "Not space for output Cmd");
    // TODO: Replace direct copy with customised encoding functions when protocol is ready for it
    b->ptrWrite += COM_CopyMemory((u8*)cmd, b->ptrWrite, cmd->size);
}
