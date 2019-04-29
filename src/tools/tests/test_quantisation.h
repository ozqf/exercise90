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

const i32 majorRange = 32;
const i32 precisionBits = 9;

u16 Test_QuantiseF32ToI16_B(f32 input, const i16 halfRange)
{
    const i32 totalBits = 16;
    i32 majorBits = (i32)log2f((f32)halfRange * 2);
    i32 minorBits = totalBits - majorBits;
    //printf("Bits: %d major %d minor\n", majorBits, minorBits);
    input += halfRange;
    f32 scale = powf(2, (f32)minorBits);
    input *= scale;
    printf("  scaled up result: %f\n", input);
    return (u16)input;
}

f32 Test_DequantiseI16ToF32_B(u16 input, const i16 halfRange)
{
    const i16 totalBits = 16;
    i32 majorBits = (i32)log2f((f32)halfRange * 2);
    i32 minorBits = totalBits - majorBits;
    printf("Bits: %d major %d minor\n", majorBits, minorBits);
    f32 output = input;
    f32 scale = powf(2, (f32)minorBits);
    output /= scale;
    output -= halfRange;
    return output;
}

i16 Test_QuantiseF32ToI16(f32 input)
{
    i32 precision = (i32)powf(2, (f32)precisionBits);
    i16 integer = (i16)((majorRange * 2) - 1) * (i16)precision;

    printf("Input %f, Precision Bits %d (scale %d) integer max: %d Bits required: %d\n",
        input, precisionBits, precision, integer, CalcNumBits(integer));
    
    input += majorRange;
    input *= precision;
    printf(" Result: %f\n", input);
    return (i16)input;
}

f32 Test_DequantiseI16ToF32(i16 input)
{
    f32 output = input;
    f32 precision = powf(2, (f32)precisionBits);
    output /= precision;
    output -= majorRange;
    return output;
}

void Test_QuantiseValue(f32 original)
{
    // i16 encoded = Test_QuantiseF32ToI16(original);
    // f32 result = Test_DequantiseI16ToF32(encoded);
    printf("-----\n> %f\n", original);
    i16 encoded = Test_QuantiseF32ToI16_B(original, 32);
    f32 result = Test_DequantiseI16ToF32_B(encoded, 32);
    printf("%f -> %d -> %f\n", original, encoded, result);
    f32 diff = original - result;
    if (diff < 0.0f) { diff *= -1; }
    if (diff < 0.01f)
    {
        printf("Okay!\n");
    }
    else
    {
        printf("Bad\n");
    }
    
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
    printf("\n-- Quantise Tests --\n");
    Test_QuantiseValue(24.0f);
    Test_QuantiseValue(26.0f);
    Test_QuantiseValue(29.35f);
    Test_QuantiseValue(31.1f);
    Test_QuantiseValue(-15.794f);
    //printf("And breaking it:\n");
    //Test_QuantiseValue(32.0f);
    //Test_QuantiseValue(-32.0f);
    //Test_QuantiseValue(-33.0f);

    //f32 testValue = 27.9251f;
    //i32 numBits = (i32)ceilf(log2f(testValue));
    //printf("Num bits for %f: %d\n", testValue, numBits);
}
