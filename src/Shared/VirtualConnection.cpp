#ifndef VIRTUALCONNECTION_CPP
#define VIRTUALCONNECTION_CPP

#include "sharedTypes.h"

#define VIRTUAL_CONN_BUFFER_SIZE_TOTAL = 6144;
#define VIRTUAL_CONN_BUFFER_COUNT = 6;
#define VIRTUAL_CONN_BUFFER_SIZE = VIRTUAL_CONN_BUFFER_SIZE_TOTAL / VIRTUAL_CONN_BUFFER_COUNT;

struct ZVirtualConnection1
{
    // 0 == Disconnected, 1 == Handshaking, 2 == syncing, 3 == Connected
    char state;
    
    int cID = -1;                // Related connection id

    uint localSequence = 0;      // Current sequence, increment with every packet sent
    uint remoteSequence = 0;     // Latest sequence number from related receiver own messages
        
    // Unreliable commands for immediate transmission to remote receiver
    char outputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Reliable commands awaiting transmission to remote receiver
    // Repeatedly sent until acked by receiver. Disconnect on overflow.
    char reliableOutputBuf[VIRTUAL_CONN_BUFFER_SIZE];

    // Unreliable commands from remote sender awaiting execution when order allows
    char inputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Reliable commands from remote sender awaiting execution
    char reliableInputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Commands received to ack to remote sender
    char outputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Last reliable input executed. Only execute reliable input in order
    u32 inputReliableSequence = 0;
    // This connection's reliable sequence for remote receivers to follow
    u32 outputReliableSequence = 1;

};

struct ZVirtualConnectionOld
{
    // 0 == Disconnected, 1 == Handshaking, 2 == syncing, 3 == Connected
    char state;
    
    int cID = -1;                // Related connection id

    uint localSequence = 0;      // Current sequence, increment with every packet sent
    uint remoteSequence = 0;     // Latest sequence number from related receiver own messages
        
    // Unreliable commands for immediate transmission to remote receiver
    char outputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Reliable commands awaiting transmission to remote receiver
    char reliableOutputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Commands from here awaiting ack from remote receiver
    char outputAck[VIRTUAL_CONN_BUFFER_SIZE];

    // Commands from remote sender awaiting execution when order allows
    char inputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Reliable commands from remote sender awaiting execution
    char reliableInputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Commands received to ack to remote sender
    char outputBuf[VIRTUAL_CONN_BUFFER_SIZE];
    // Last reliable input executed. Only execute reliable input in order
    u32 inputReliableSequence = 0;
    // This connection's reliable sequence for remote receivers to follow
    u32 outputReliableSequence = 1;

};

#endif