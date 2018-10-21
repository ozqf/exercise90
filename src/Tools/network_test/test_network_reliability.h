#pragma once

#include "../../common/com_module.h"

#define MAX_PACKET_TRANSMISSION_MESSAGES 32
struct PacketTransmissionRecord
{
	u32 sequence;
	u32 numReliableMessages;
	u32 reliableMessageIds[MAX_PACKET_TRANSMISSION_MESSAGES];
};

#define MAX_TRANSMISSION_RECORDS 33 
PacketTransmissionRecord g_transmissions[MAX_TRANSMISSION_RECORDS];

PacketTransmissionRecord* Net_FindTransmissionRecord(PacketTransmissionRecord* records, u32 sequence)
{
	PacketTransmissionRecord* result = &records[sequence % MAX_TRANSMISSION_RECORDS];
	if (result->sequence == sequence)
	{
		result->sequence = 0;
		return result;
	}
	return NULL;
}

void Net_PacketAcked()
{
	// > Fetch Transmission record
	// > iterate messageIds, deleting them from the output buffer
}

#if 0
#define MAX_BUFFERED_MESSAGES 64
struct MessageQueue
{
	u32 sequence;
	u32 items[MAX_BUFFERED_MESSAGES];
	
	i32 InsertNewItem(u32 newItem)
	{
		for (i32 i = 0; i < MAX_BUFFERED_MESSAGES; ++i)
		{
			if (this->items[i] == 0)
			{
				this->items[i] = newItem;
				return i;
			}
		}
		return -1;
	}
	
	u32 FindAndPopItem(u32 item)
	{
		for (i32 i = 0; i < MAX_BUFFERED_MESSAGES; ++i)
		{
			u32 pendingSequence = this->items[i];
			if (pendingSequence != 0 && pendingSequence == item)
			{
				this->items[i] = 0;
				return pendingSequence;
			}
		}
		return 0;
	}

	i32 ClearItemAndAllPrevious(u32 queryItem)
	{
		i32 count = 0;
		for (i32 i = 0; i < MAX_BUFFERED_MESSAGES; ++i)
		{
			if (this->items[i] != 0 && this->items[i] <= queryItem)
			{
				this->items[i] = 0;
				count++;
			}
		}
		return count;
	}
	
	void Reset()
	{
		i32 bytes = MAX_BUFFERED_MESSAGES * sizeof(u32);
		COM_ZeroMemory((u8*)this->items, bytes);
		this->sequence = 0;
	}
	
	void Print(char* heading)
	{
		if (heading)
		{
			printf("%s: ", heading);
		}
		
		for (i32 i = 0; i < MAX_BUFFERED_MESSAGES; ++i)
		{
			u32 item = this->items[i];
			if (item != 0) { printf("%d, ", item); }
		}
		printf("\n");
	}
};

MessageQueue g_reliableInput;
MessageQueue g_reliableOutput;

void TNet_ReadInput()
{
	g_reliableInput.Print("Input");
	
	printf("Reading reliable queue from %d: ", (g_reliableInput.sequence + 1));
	
	for (;;)
	{
		u32 nextSequence = g_reliableInput.sequence + 1;
		u32 msg = g_reliableInput.FindAndPopItem(nextSequence);
		if (msg != 0)
		{
			g_reliableInput.sequence++;
			printf("%d, ", msg);
		}
		else
		{
			printf("...done. Last msg was %d\n", g_reliableInput.sequence);
			break;
		}
	}
	
	g_reliableInput.Print("Input");
}

i32 TNet_CheckForAck(u32 ack, u32 ackBits, u32 sequence)
{
	if (sequence > ack) { return 0; }
	
	/*
	eg if ack == 16, sequence == 15 then ack - seq == 1;
	if bit == 0 then ack and sequence match
	otherwise, bit -1 and look in ackbits
	*/
	u32 bit = ack - sequence;
	if (bit > 32) { return 0; }
	if (bit == 0) { return 1; }
	bit--;
	u32 bitValue = 1 << bit;
	if (ackBits & bitValue)
	{
		return 1;
	}
	return 0;
}
#endif
void TNet_TestReliability()
{
	Test_NetBuffer();
}
