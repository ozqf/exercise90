#ifndef EVENTS_H
#define EVENTS_H

#include "shared.h"

/*
> 1st byte: type, convert to struct?
*/

struct CmdGeneric
{
    i32 data1;
    i32 data2;
};

i32 bufferSize;

char* buffer;

void ConceptReadBuffer()
{
    char* cursor = buffer;
    i32 bytesRead = 0;
    bool reading = true;
    while (reading)
    {
        uChar type = *cursor;
        switch (type)
        {
            case 0:
            {
                CmdGeneric cmd;
            }
            break;
        
            default:
            // null/unknown cmd, stop
            break;
        }

        cursor++;
        bytesRead++;
    }
}

#endif