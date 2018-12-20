#pragma once

//#include "app_module.cpp"

#define CLIENT_APP_STATE_NULL 0
#define CLIENT_APP_STATE_LOADING 1
#define CLIENT_APP_STATE_IN_PLAY 2

#define MAX_TEST_CLIENT_NAME_LENGTH 32

////////////////////////////////////////////////////////////////////
// Network
////////////////////////////////////////////////////////////////////

#define STREAM_MSG_HEADER_SENTINEL_SIZE 4
struct StreamMsgHeader
{
    u32 id;
    i32 size;
    f32 resendTime;
    f32 resendMax;
	char sentinel[STREAM_MSG_HEADER_SENTINEL_SIZE];
};

// Big-endian
//#define NET_DESERIALISE_CHECK 0xDEADBEEF
// Little-endian
#define NET_DESERIALISE_CHECK 0xEFBEADDE
#define NET_DESERIALISE_CHECK_LABEL "0xDEADBEEF"

#define NET_DEFAULT_RESEND_RATE 0.05f

// (u16 - num reliable bytes) (u32 - first message Id)
#define NET_SIZE_OF_RELIABLE_HEADER (sizeof(u16) + sizeof(u32))

// (u16 -num reliable bytes)
#define NET_SIZE_OF_UNRELIABLE_HEADER (sizeof(u16))

#define MAX_PACKET_TRANSMISSION_MESSAGES 64
struct TransmissionRecord
{
	u32 sequence;
	u32 numReliableMessages;
	u32 reliableMessageIds[MAX_PACKET_TRANSMISSION_MESSAGES];
};

#define MAX_TRANSMISSION_RECORDS 33
struct NetStream
{
    // latest reliable command from remote executed here
    u32 inputSequence;
    // id of next reliable message sent to remote
    u32 outputSequence;
    // the most recented acknowledged message Id
    u32 ackSequence;

    ByteBuffer inputBuffer;
    ByteBuffer outputBuffer;
    
    TransmissionRecord transmissions[MAX_TRANSMISSION_RECORDS];
};

struct PacketDescriptor
{
    u8* ptr;
	u8* cursor;
    u16 size;
	
	u32 transmissionSimFrameNumber;
	f32 transmissionSimTime;
	// if 0, no data
	// num bytes is offset gap to unreliable section - sync check size.
	u16 reliableOffset;
	u32 deserialiseCheck;
	// if 0, no data.
	// num bytes is size of packet - offset.
	u16 unreliableOffset;
	
	i32 Space()
	{
		return size - (cursor - ptr);
	}
};

struct ClientInfo
{
    u8 colourRGB[3];
    u8 nameLength;
    char name[MAX_TEST_CLIENT_NAME_LENGTH];

    i32 Read(u8* ptr)
    {
        this->colourRGB[0] = COM_ReadByte(&ptr);
        this->colourRGB[1] = COM_ReadByte(&ptr);
        this->colourRGB[2] = COM_ReadByte(&ptr);
        this->nameLength = COM_ReadByte(&ptr);
        if (nameLength > MAX_TEST_CLIENT_NAME_LENGTH)
        {
            nameLength = MAX_TEST_CLIENT_NAME_LENGTH;
        }
        ptr += COM_CopyStringLimited((char*)ptr, this->name, MAX_TEST_CLIENT_NAME_LENGTH);
    }

    i32 Write(u8* ptr)
    {
        ptr += COM_WriteByte(this->colourRGB[0], ptr);
        ptr += COM_WriteByte(this->colourRGB[1], ptr);
        ptr += COM_WriteByte(this->colourRGB[2], ptr);
        u8 chars = (u8)COM_StrLen(this->name) + 1; // + 1 for terminator;
        this->nameLength = chars;
        ptr += COM_WriteByte(chars, ptr);
        ptr += COM_COPY(this->name, ptr, chars);
    }
};
