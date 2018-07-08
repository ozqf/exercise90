#pragma once 

#include "../common/com_module.h"

/*
1 2
2 4
3 8
4 16
5 32
6 64
7 128
8 256

9 512
10 1024
11 2048
12 4096
13 8192
14 16384
15 32768
16 65536

17 131072
18 262144
19 524288
20 1048576
21 2097152
22 4194304
23 8388608
24 16777216

25 
26 
27 
28 
29 
30 
31 
32 
*/

union FourBytesU
{
	i32 asI32;
	u32 asU32;
    u8 uBytes[4];
	i8 bytes[4];
	i16 int16s[2];
	u16 uInt16s[2];
};

void BitPack1()
{
    u16 maxU16 = 4096;
    u8 offset = 0;
    u8 b = 0;
    for (i32 i = 0; i < 16; ++i)
    {
        u8 bit = offset + (u8)i;
        i32 shift = 1 & (maxU16 & (1 << i));
        printf("%d: %d, ", bit, shift);
        
    }
    printf("\nresult: %d\n", b);
}

void Test_PrintBits(i32 val, u8 printNewLine)
{
    for (i32 i = 31; i >= 0; --i)
    {
        i32 result = 1 & (val >> i);
        printf("%d", result);
    }
    if (printNewLine)
    {
        printf("\n");
    }
}

void Test_PrintABC(i32 a, i32 b, i32 c)
{
    printf("a: %d\nb: %d\nc: %d\n", a, b, c);
}

void Test_CombineAB(i32 a, i32 b)
{
    i32 c = a | b;
    Test_PrintABC(a, b, c);
    Test_PrintBits(a, 1);
    Test_PrintBits(b, 1);
    Test_PrintBits(c, 1);
}

void Test_Bitshift1()
{
    i32 a = 27534;
    i32 a2 = (a << 16);
    i32 a3 = (a2 >> 16);
    i32 b = -7000;
    //i32 c = b | a2;
    i32 c = a2 | b;
    printf("a: %d a2: %d a3, %d\nb: %d\nc: %d\n", a, a2, a3, b, c);
    
    printf("\n");
}

i32 Test_PrintVal(i32 val, char* name)
{
    printf("%s: %d\n", name, val);
    Test_PrintBits(val, 1);
    return val;
}

i32 T_CreateBitwiseAndMask(i32 numBits)
{
    i32 m = 0;
    i32 bit = 0;
    while(bit < numBits)
    {
        m |= (1 << bit);
        bit++;
    }
    return m;
}

i32 T_ExtractBitRange(i32 source, i32 firstBit, i32 numBits)
{
    //i32 shifted = (source >> firstBit);
    //printf("Extract %d bits from %d\n", numBits, firstBit);
    //printf("%d shifted: %d\n", source, shifted);
    i32 mask = T_CreateBitwiseAndMask(numBits);
    printf("Extraction mask for %d bits from %d\n", numBits, firstBit);
    mask = (mask << firstBit);
    Test_PrintBits(mask, 1);
    source = source & mask;
    return (source >> firstBit);
    //shifted &= mask;
    //printf("%d ANDed: %d\n", source, shifted);
    //return shifted;
}

i32 T_BuildTenBitVec3(f32 x, f32 y, f32 z)
{
    printf("Prepack: %.4f, %.4f, %.4f\n", x, y, z);
    i32 a = (i32)((x + 1) * 500);
    i32 b = (i32)((y + 1) * 500);
    i32 c = (i32)((z + 1) * 500);
    printf("Packing ints: %d, %d, %d\n", a, b, c);
    i32 r = 0;
    r |= (a << 20);
    r |= (b << 10);
    r |= (c << 0);

    Test_PrintBits((a << 20), 1);
    Test_PrintBits((b << 10), 1);
    Test_PrintBits((c << 0), 1);
    Test_PrintBits(r, 1);

    f32 x2 = (f32)(((f32)a / 500) - 1);
    f32 y2 = (f32)(((f32)b / 500) - 1);
    f32 z2 = (f32)(((f32)c / 500) - 1);
    printf("Unshifted unpacked: %.4f, %.4f, %.4f\n", x2, y2, z2);

    return r;
}

void T_Unpack10BitVec3Array(i32 source, f32* destination)
{
    printf("Extract source %d\n", source);
    i32 a = T_ExtractBitRange(source, 20, 10);
    i32 b = T_ExtractBitRange(source, 10, 10);
    i32 c = T_ExtractBitRange(source, 0, 10);
    printf("Unpacked ints %d, %d, %d\n", a, b, c);  
    destination[0] = (f32)(((f32)a / 500.0f) - 1);
    destination[1] = (f32)(((f32)b / 500.0f) - 1);
    destination[2] = (f32)(((f32)c / 500.0f) - 1);
}

i32 T_ExtractBitRange2(i32 source, i32 firstBit, i32 numBits)
{
    /*

    */
    i32 result = 0;
    i32 end = firstBit + numBits;
    //printf("Extract %d bits from %d\n", numBits, firstBit);
    for (i32 shift = firstBit; shift < end; ++shift)
    {
        // extract bit
        i32 bit = source & (shift << 1);
        //printf("Shift %d: %d\n", shift, bit);
        result |= bit;
    }
    return result;
}

void Test_Bitshift2()
{
    i32 val = 1;
    #if 0
    for (int i = 0; i < 32; ++i)
    {
        Test_PrintBits(val, 1);
        if (i < 31)
        {
            val *= 2;
        }
        
    }
    #endif


    #if 1
    i32 a = Test_PrintVal(T_BuildTenBitVec3(0.304f, -0.211f, 0.929f), "a");
    printf("----------------------------------\n");
    f32 b[3];
    T_Unpack10BitVec3Array(a, b);
    printf("b: (a Unpacked) %.4f, %.4f, %.4f\n", b[0], b[1], b[2]);
    #endif

    #if 0
    //i32 a = (1 << 2);
    printf("--- Simple Shifts ---\n");
    //i32 a = Test_PrintVal((1 << 2), "a");
    //i32 b = Test_PrintVal((a << 2), "b");
    //i32 c = Test_PrintVal((b >> 2), "c");

    //i32 _d = Test_PrintVal(27534, "d");
    //i32 e = Test_PrintVal(T_ExtractBitRange(_d, 0, 4), "e");

    //i32 f = Test_PrintVal(T_ExtractBitRange(_d, 7, 8), "f");
    //i32 g = Test_PrintVal(T_CreateBitwiseAndMask(32), "mask 32");
    //i32 h = Test_PrintVal(T_CreateBitwiseAndMask(7), "mask 7");
    i32 i = Test_PrintVal(T_BuildTenBitVec3(0.304f, -0.211f, 0.929f), "i");
    f32 j[3];
    T_Unpack10BitVec3Array(i, j);
    printf("J: (i Unpacked) %.4f, %.4f, %.4f\n", j[0], j[1], j[2]);
    #endif
    #if 0
    printf("--- Simple XOR ---\n");
    i32 b_or_c = Test_PrintVal((b | c), "b or c");
    i32 c_or_b = Test_PrintVal((c | b), "c or b");

    printf("--- Bigger XOR ---\n");
    i32 _d = Test_PrintVal(27534, "d");
    i32 e = Test_PrintVal(-7000, "e");
    i32 d_or_e = Test_PrintVal(_d | e, "d or e");

    i32 f = Test_PrintVal((_d << 16), "f (d shifted 16)");
    i32 g = Test_PrintVal((u16)f, "g (f as u16)");
    i32 g2 = Test_PrintVal((u16)_d, "g2 (d as u16)");
    #endif
    #if 0
    for (int i = 0; i < 32; ++i)
    {
        Test_PrintBits(val, 1);
        val /= 2;
    }
    Test_PrintBits(-1, 1);
    #endif
    //Test_PrintBits(1, 1);
    //Test_PrintBits(64, 1);
    //Test_CombineAB(1, 2);
    //Test_CombineAB(2, 1);

    //Test_CombineAB(40054312, -34);
    
    
}

void Test_Bitpacking()
{
    // 9 bits for 0-512 degrees rot
    //Test_PrintBits(4096);
    Test_Bitshift2();
}

