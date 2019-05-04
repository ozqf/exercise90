#pragma once

#include "common.h"

#include <math.h>

struct QuantiseDef
{
    i32 halfMajorScale;
    i32 positiveMaximum;
    i32 majorBits;
    i32 minorBits;
    f32 minorScale;
};

struct QuantiseSet
{
    QuantiseDef pos;
    QuantiseDef vel;
    QuantiseDef rot;
};

internal u32 COM_QuantiseF2I(f32 input, QuantiseDef* def)
{
    // Remove sign
    input += def->halfMajorScale;
    // Clamp
    if (input > def->positiveMaximum) { input = (f32)def->positiveMaximum; }
    if (input < 0) { input = 0; }
    // Multiple by minor scale to preserve remaining space as decimal
    return (u32)(input * def->minorScale);
}

internal f32 COM_DequantiseI2F(u32 input, QuantiseDef* def)
{
    f32 output = (f32)input;
    output /= def->minorScale;
    output -=def->halfMajorScale;
    return output;
}

internal void COM_QuantiseInit(
    QuantiseDef* def, const i32 halfRange, const u8 numBits)
{
    def->halfMajorScale = halfRange;
    def->positiveMaximum = (def->halfMajorScale * 2) - 1;
    def->majorBits = (i32)log2f((f32)def->halfMajorScale * 2);
    def->minorBits = numBits - def->majorBits;
    def->minorScale = powf(2, (f32)def->minorBits);
}

// Quantise the given float into an integer between
// -halfRange to +(halfRange - 1), within the given bits.
// spare bits are used for decimal precision
internal u32 COM_QuantiseF2I_Slow(
    f32 input, const i32 halfRange, const u8 numBits)
{
    QuantiseDef def = {};
    COM_QuantiseInit(&def, halfRange, numBits);
    return COM_QuantiseF2I(input, &def);
}

// Reverse the Quantisation
internal f32 COM_DequantiseI2F_Slow(
    i32 input, const i32 halfRange, const u8 numBits)
{
    QuantiseDef def = {};
    COM_QuantiseInit(&def, halfRange, numBits);
    return COM_DequantiseI2F(input, &def);
    #if 0
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
    #endif
}

