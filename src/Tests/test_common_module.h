#pragma once

#include "../common/com_module.h"
#include <stdio.h>

void Test_Com_CompareTest(char* a, char* b)
{
    printf("Compare: '%s' to '%s': %d\n", a, b, COM_CompareStrings(a, b));
}

void Test_Com_StringCompareTests()
{
    printf("\n** STRING COMPARE TESTS **\n");
    Test_Com_CompareTest("aab", "aac");
    Test_Com_CompareTest("aac", "aaa");
    Test_Com_CompareTest("aa", "aaa");
    Test_Com_CompareTest("asdzfda", "asdz f");
    Test_Com_CompareTest("Hello, world!", "Hello, world!");
    Test_Com_CompareTest(__TIME__, __DATE__);
    Test_Com_CompareTest("43132", "43112");
    Test_Com_CompareTest("Foobar", "Foobar");
}

void Test_Com_StringCopyTests()
{
    printf("\n** STRING COPY TESTS **\n");
    char* a = "Hello, World!";
    printf("Original: %s\n", a);
    char b[32];
    printf("b: %s\n", b);
    COM_CopyString(a, b);
    printf("b after copy: %s\n", b);
    char c[32];
    COM_CopyString(b, c);
    printf("b copied into c: %s\n", c);
}

void Test_Com_AsciToInt32(char* str)
{
    i32 val = COM_AsciToInt32(str);
    printf("%s as int: %d\n", str, val);
}

void Test_Com_AsciToInt32_Series()
{
    printf("\n** ASCI_TO_INT_32 (atoi) TESTS **\n");
    Test_Com_AsciToInt32("50");
    Test_Com_AsciToInt32("-324");
    Test_Com_AsciToInt32("0");
    Test_Com_AsciToInt32("this isn't a number");
    Test_Com_AsciToInt32("0x400"); // 1024
    Test_Com_AsciToInt32("-0x1631"); // -5681
}

void Test_Com_StringLength()
{
    printf("\n** Test String length **\n");
    printf("Length of '%s': %d\n", "Foo", COM_StrLen("Foo"));
    printf("Length of '%s': %d\n", "Hello, World!<endL>", COM_StrLen("Hello, World!\n"));
}

void Test_COM_PrintMatrix(f32* m)
{
    printf("%.2f, %.2f, %.2f, %.2f\n", m[M4x4_X0], m[M4x4_X1], m[M4x4_X2], m[M4x4_X3]);
    printf("%.2f, %.2f, %.2f, %.2f\n", m[M4x4_Y0], m[M4x4_Y1], m[M4x4_Y2], m[M4x4_Y3]);
    printf("%.2f, %.2f, %.2f, %.2f\n", m[M4x4_Z0], m[M4x4_Z1], m[M4x4_Z2], m[M4x4_Z3]);
    printf("%.2f, %.2f, %.2f, %.2f\n", m[M4x4_W0], m[M4x4_W1], m[M4x4_W2], m[M4x4_W3]);
}

// void Test_COM_PrintMatrix(M4x4* m)
// {
//     printf("%.2f, %.2f, %.2f, %.2f\n", m->x0, m->x1, m->x2, m->x3);
//     printf("%.2f, %.2f, %.2f, %.2f\n", m->y0, m->y1, m->y2, m->y3);
//     printf("%.2f, %.2f, %.2f, %.2f\n", m->z0, m->z1, m->z2, m->z3);
//     printf("%.2f, %.2f, %.2f, %.2f\n", m->w0, m->w1, m->w2, m->w3);
// }

void Test_COM_PrintVector4(f32* v)
{
    printf("%.2f, %.2f, %.2f, %.2f\n", v[VEC_X], v[VEC_Y], v[VEC_Z], v[VEC_W]);
}

void Test_COM_PrintVector3(Vec3 v)
{

    printf("x/y/z: %.2f, %.2f, %.2f\n", v.x, v.y, v.z);
    printf("array:%.2f, %.2f, %.2f\n", v.e[0], v.e[1], v.e[2]);
}

void Test_Com_Run()
{
    // Test_Com_StringCompareTests();
    // Test_Com_StringCopyTests();
    // Test_Com_AsciToInt32_Series();
    // Test_Com_StringLength();
    //Test_COM_Maths();
}
