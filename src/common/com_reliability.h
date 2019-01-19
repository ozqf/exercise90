#ifndef NET_RELIABILITY_H
#define NET_RELIABILITY_H

#include "com_module.h"

#define ACK_CAPACITY 32
// +1 due to ack number (1) + ackbits (32)
#define ACK_RESULTS_CAPACITY 33

struct AckRecord
{
    u32 sequence;
    u32 acked;
};

struct AckStream
{
	// Sequence number of outgoing packets
	u32 sequence;
	// Last 32 sent sequences and whether they have been acked or not
	AckRecord awaitingAck[ACK_CAPACITY];
	
	// most recent received packet
	u32 remoteSequence;
	// Used to build ack bits from this side
	u32 received[ACK_CAPACITY];
	
};
#if 1
static void Ack_RecordPacketTransmission(AckStream* astream, u32 outputSequence)
{
	u32 index = outputSequence % ACK_CAPACITY;
	astream->awaitingAck[index].sequence = outputSequence;
	astream->awaitingAck[index].acked = 0;
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

// Returns number of acknowledged packet sequences written to results
// Max results must == (ACK_CAPACITY + 1)!
static i32 Ack_CheckIncomingAcks(
	AckStream* astream, u32 packetAck, u32 packetAckBits, u32* results)
{
	i32 resultIndex = 0;
	i32 index = packetAck % ACK_CAPACITY;
	AckRecord* rec = &astream->awaitingAck[index];
	if (rec->sequence = packetAck
		&& rec->acked == 0)
	{
		rec->acked = 1;
		results[resultIndex++] = packetAck;
	}
	
	u32 bit = 0;
	packetAck -= 1;
	while (bit < 32)
	{
		if (packetAckBits & (1 << bit))
		{
			index = packetAck % ACK_CAPACITY;
			printf(" Sequence: %d Bit %d index %d\n", packetAck, bit, index);
			#if 1
			rec = &astream->awaitingAck[index];
			// only report once
			if (rec->sequence == packetAck && rec->acked == 0)
			{
				rec->acked = 1;
				results[resultIndex++] = packetAck;
			}
			#endif
		}
		packetAck--;
		bit++;
	}
	return resultIndex;
}

extern "C"
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

extern "C"
static void Ack_Test()
{
	
}
#endif

// NET_RELIABILITY_H
#endif
