#ifndef NET_RELIABILITY_H
#define NET_RELIABILITY_H

/*
Reliability  system for recording transmission and reception of packets in a sequence.
Each ack response contains 33 acks made up of two u32s:
> The highest remote sequence receive
> 32 bits each representing the highest sequence number minus n, where n is the bit number (0 to 31)
Records of transmission or reception are stored in rolling arrays
*/
#include "com_module.h"

#define ACK_CAPACITY 32
// +1 due to ack number (1) + ackbits (32)
#define ACK_RESULTS_CAPACITY 33

struct AckRecord
{
    u32 sequence;
    u32 acked;
	f32 sentTime;
	f32 receivedTime;
};

struct AckStream
{
	// Sequence number of transmitted packets - increment with each send
	u32 outputSequence;
	// Last 32 sent sequences and whether they have been acked or not
	AckRecord awaitingAck[ACK_CAPACITY];
	
	// most recent received packet (highest ack)
	u32 remoteSequence;
	// Used to build ack bits from this side
	u32 received[ACK_CAPACITY];
	
};

static void Ack_RecordPacketTransmission(
	AckStream* astream, u32 outputSequence, f32 time)
{
	u32 index = outputSequence % ACK_CAPACITY;
	astream->awaitingAck[index].sequence = outputSequence;
	astream->awaitingAck[index].acked = 0;
	astream->awaitingAck[index].sentTime = time;
}

static void Ack_RecordPacketReceived(AckStream* astream, u32 packetSequence)
{
	// update latest packet record
	if (astream->remoteSequence < packetSequence)
	{
		astream->remoteSequence = packetSequence;
	}
	u32 index = packetSequence % ACK_CAPACITY;
	astream->received[index] = packetSequence;
}

static f32 Ack_CalculateAverageDelay(AckStream* astream)
{
	i32 records = 0;
	f32 total = 0;
	for (i32 i = 0; i < ACK_CAPACITY; ++i)
	{
		AckRecord* rec = &astream->awaitingAck[i];
		if (rec->acked)
		{
			records++;
			total += rec->receivedTime - rec->sentTime;
		}
	}
	if (records == 0) { return 0; }
	return (total / (f32)records);
}

// Returns number of acknowledged packet sequences written to results
// Max results must == (ACK_CAPACITY + 1)!
static i32 Ack_CheckIncomingAcks(
	AckStream* astream, u32 packetAck, u32 packetAckBits, u32* results, f32 time)
{
	i32 resultIndex = 0;
	i32 index = packetAck % ACK_CAPACITY;
	AckRecord* rec = &astream->awaitingAck[index];
	if (rec->sequence = packetAck
		&& rec->acked == 0)
	{
		rec->acked = 1;
		rec->receivedTime = time;
		results[resultIndex++] = packetAck;
	}
	
	u32 bit = 0;
	packetAck -= 1;
	while (bit < 32)
	{
		if (packetAckBits & (1 << bit))
		{
			index = packetAck % ACK_CAPACITY;
			rec = &astream->awaitingAck[index];
			// only report once
			if (rec->sequence == packetAck && rec->acked == 0)
			{
				rec->acked = 1;
				rec->receivedTime = time;
				results[resultIndex++] = packetAck;
			}
		}
		packetAck--;
		bit++;
	}
	return resultIndex;
}

static u32 Ack_BuildOutgoingAckBits(AckStream* astream)
{
	u32 ackBits = 0;
	u32 bit = 0;
	u32 expectedSequence = astream->remoteSequence - 1;
	while (bit < 32)
	{
		i32 index = expectedSequence % ACK_CAPACITY;
		if (astream->received[index] == expectedSequence)
		{
			ackBits |= (1 << bit);
		}
		bit++;
		expectedSequence--;
	}
	return ackBits;
}

// NET_RELIABILITY_H
#endif
