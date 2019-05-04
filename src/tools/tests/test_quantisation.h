#pragma once

#include "../../common/common.h"

#include "math.h"

struct I32Range
{
    i32 min;
    i32 max;
};

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

// Quantise the given float into an integer between
// -halfRange to +(halfRange - 1), within the given bits.
// spare bits are used for decimal precision
internal u32 Test_QuantiseF2I(f32 input, const i32 halfRange, const u8 numBits)
{
    // Remove sign
    input += halfRange;
    i32 positiveMaximum = (halfRange * 2) - 1;
    // clamp
    if (input > positiveMaximum) { input = (f32)positiveMaximum; }
    if (input < 0) { input = 0; }
	// Calc precision
	i32 majorBits = (i32)log2f((f32)halfRange * 2);
	i32 minorBits = numBits - majorBits;
	f32 scale = powf(2, (f32)minorBits);
    return (u32)(input * scale);
}

// Reverse the Quantisation
internal f32 Test_DequantiseI2F(i32 input, const i32 halfRange, const u8 numBits)
{
	// Calc precision
	f32 fullRange = (f32)(halfRange * 2);
	i32 majorBits = (i32)log2f(fullRange);
	i32 minorBits = numBits - majorBits;
	f32 scale = powf(2, (f32)minorBits);
	// unpack
	f32 output = (f32)input;
	output /= scale;
	output -= halfRange;
	return output;
}

void Test_QuantiseValue(f32 original, i32 halfRange, u8 numBits)
{
    i32 mask = COM_CreateBitmask(numBits);
	printf("-----\n> %f in %d bits (%d bit mask)\n", original, numBits, numBits);
    u32 encoded = COM_QuantiseF2I_Slow(original, halfRange, numBits);
    COM_PrintBits(encoded, 1);
    COM_PrintBits(mask, 1);
    encoded = encoded & mask;
    f32 result = COM_DequantiseI2F_Slow(encoded, halfRange, numBits);
	
    printf("%f -> %d -> %f\n", original,  encoded, result);
    f32 diff = original - result;
    if (diff < 0.0f) { diff *= -1; }
    printf("\tLoss: %f\n", diff);
}

void Tests_Quantisation()
{
    printf("\n-- Quantise Tests --\n");
    Test_QuantiseValue(30.14959403f, 32, 16);
    Test_QuantiseValue(30.14959403f, 32, 20);
    Test_QuantiseValue(30.14959403f, 32, 17);
    Test_QuantiseValue(30.14959403f, 32, 9);
    Test_QuantiseValue(0.14959403f, 2, 10);
    Test_QuantiseValue(1.0f, 2, 10);
    Test_QuantiseValue(-1.0f, 2, 10);
    Test_QuantiseValue(30.14959403f, 128, 16);

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
    #if 0
    printf("\nPosition quantization...\n");
    CalcQuantizationBits(32, 256);
    CalcQuantizationBits(64, 128);
    CalcQuantizationBits(512, 256);
    CalcQuantizationBits(1024, 512);
    CalcQuantizationBits(512, 2024);
    CalcQuantizationBits(2048, 2048);

    printf("\nVelocity quantization...\n");
    CalcQuantizationBits(128, 128);
    #endif
}
