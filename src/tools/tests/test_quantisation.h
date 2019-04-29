#pragma once

#include "../common/common.h"

#include "math.h"

i32 CalcNumBits(i32 value)
{
    // + 1 for sign bit
    return (i32)ceilf(log2f((f32)value)) + 1;
}

i32 CalcQuantizationBits(i32 highLimit, i32 lowLimit)
{
    i32 bits = CalcNumBits(highLimit) + CalcNumBits(lowLimit);
    i32 bytes = (i32)ceilf(((f32)bits / 8.0f));
    printf("Num bits for %d metres, %d subdivisions\n",
        highLimit, lowLimit);
    printf("\t%d bits, %d bytes\n", bits, bytes);
    return bits;
}

void Tests_Quantisation()
{
    /*
    Current sync command sends:
    i32 networkId;
	Vec3 pos;
	Vec3 rot;
	Vec3 vel;
    i32 targetId;
    u8 priority;
    Current best case (entire packet for unreliable)
    gets 20 syncs across per packet.
    average is approx 700 bytes reserved for unreliable
    eg:
    id, 4 bytes
    targetId, 4 bytes
    pos at 3 bytes per part, 9 bytes total
    vel at 2 bytes per part, 6 bytes total
    rot at 2 bytes per part, 4 total
    tickTime, 4 bytes
    tickIndex, 1 byte
    32 bytes total.
    */
    printf("\nPosition quantization...\n");
    CalcQuantizationBits(512, 256);
    CalcQuantizationBits(1024, 512);
    CalcQuantizationBits(512, 2024);
    CalcQuantizationBits(2048, 2048);

    printf("\nVelocity quantization...\n");
    CalcQuantizationBits(128, 128);
    //f32 testValue = 27.9251f;
    //i32 numBits = (i32)ceilf(log2f(testValue));
    //printf("Num bits for %f: %d\n", testValue, numBits);
}
