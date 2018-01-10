#pragma once

#include "../Shared/shared.h"
#include <stdio.h>

void Test_Com_CompareTest(char* a, char* b)
{
    printf("Compare: '%s' to '%s': %d\n", a, b, Com_CompareStrings(a, b));
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
    Com_CopyString(a, b);
    printf("b after copy: %s\n", b);
    char c[32];
    Com_CopyString(b, c);
    printf("b copied into c: %s\n", c);
}

void Test_Com_AsciToInt32(char* str)
{
    i32 val = Com_AsciToInt32(str);
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
    printf("Length of '%s': %d\n", "Foo", Com_StrLen("Foo"));
    printf("Length of '%s': %d\n", "Hello, World!<endL>", Com_StrLen("Hello, World!\n"));
}

void Test_COM_PrintMatrix(f32* m)
{
    printf("%.2f, %.2f, %.2f, %.2f\n", m[X0], m[X1], m[X2], m[X3]);
    printf("%.2f, %.2f, %.2f, %.2f\n", m[Y0], m[Y1], m[Y2], m[Y3]);
    printf("%.2f, %.2f, %.2f, %.2f\n", m[Z0], m[Z1], m[Z2], m[Z3]);
    printf("%.2f, %.2f, %.2f, %.2f\n", m[W0], m[W1], m[W2], m[W3]);
}

void Test_COM_PrintVector4(f32* v)
{
    printf("%.2f, %.2f, %.2f, %.2f\n", v[VEC_X], v[VEC_Y], v[VEC_Z], v[VEC_W]);
}

void Test_COM_MatrixMaths()
{
    printf("**************************\n");
    printf("TEST MATRIX MATHS\n");
    
    printf("\nSet to Identity Matrix:\n");
    f32 matrix[16];
    COM_SetToIdentityMatrix(matrix);
    Test_COM_PrintMatrix(matrix);

    printf("\nSet to scale by 2:\n");
    COM_SetAsScaleMatrix(matrix, 2, 2, 2);
    Test_COM_PrintMatrix(matrix);

    f32 v0[4];
    v0[VEC_X] = 3;
    v0[VEC_Y] = 2;
    v0[VEC_Z] = -4;
    v0[VEC_W] = 1;
    printf("Print vector v:\n");
    Test_COM_PrintVector4(v0);

    printf("\nMultiply v by m\n");
    COM_MultiplyVectorByMatrix(matrix, v0);
    Test_COM_PrintVector4(v0);

    printf("\nCreate Translation Matrix\n");
    COM_SetToIdentityMatrix(matrix);
    COM_SetMoveMatrix(matrix, 9, -3, 2);
    Test_COM_PrintMatrix(matrix);

    printf("Multiply v by m\n");
    COM_MultiplyVectorByMatrix(matrix, v0);
    Test_COM_PrintVector4(v0);

    printf("\n");
}

void Test_COM_Maths()
{
    Test_COM_MatrixMaths();
}

void Test_Com_Run()
{
    // Test_Com_StringCompareTests();
    // Test_Com_StringCopyTests();
    // Test_Com_AsciToInt32_Series();
    // Test_Com_StringLength();
    Test_COM_Maths();
}
