#ifndef MEMORY_H
#define MEMORY_H

void ZeroMemory(char *start, int numBytes)
{
    for (int i = 0; i < numBytes; ++i)
    {
        *start = 0;
        start++;
    }
}

#endif
