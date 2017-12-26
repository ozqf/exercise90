/*******************************************
 * Utility functions for manipulating raw memory
 ******************************************/
#pragma once
#include "shared_types.h"

/**
 * if strings are equal, return 0
 * if a is first alphabetically, return -1
 * if b is first alphabetically, return 1
 */
i32 Com_CompareStrings(char *a, char *b)
{
    while (*a == *b)
    {
        // End of string
        if (*a == NULL)
        {
            return 0;
        }
        ++a;
        ++b;
    }

    return ((*a < *b) ? -1 : 1);
}

void Com_CopyString(char *source, char *target)
{
    while (true)
    {
        *target = *source;
        if (*source == NULL)
        {
            return;
        }
        target++;
        source++;
    }
}

// decimal or hexadecimal
// negative and positive
// "-54" "12" "0x432146fd" "-0X4AbdC"
i32 Com_AsciToInt32(char *str)
{
    i32 sign = 1;
    i32 val = 0;
    char c;
    if (*str == '-')
    {
        sign = -1;
        str++;
    }

    // hexadecimal
    if (*str == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        // Move past first two characters
        str += 2;
        while (1)
        {
            c = *str;
            str++;
            if (c >= '0' && c <= '9')
            {
                val = val * 16 + c - '0';
            }
            else if (c >= 'a' && c <= 'f')
            {
                val = val * 16 + c - 'a' + 10;
            }
            else if (c >= 'A' && c <= 'F')
            {
                val = val * 16 + c - 'A' + 10;
            }
            else
            {
                return val * sign;
            }
        }
    }

    // decimal
    while (true)
    {
        c = *str;
        str++;
        if (c < '0' || c > '9')
        {
            // no numerical character
            return sign * val;
        }
        // '0' char = 48 in asci, so
        // c minus '0' = c minus 48
        // val * 10 moves to next digit
        // then add new value
        val = (val * 10) + (c - '0');
    }
    return val * sign;
}

f32 Com_AsciToFloat32(char *str)
{
    return 0.0f;
}
