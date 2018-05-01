#pragma once

#include "../common/com_module.h"
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

void Test_COM_PrintVector2(Vec2 v)
{
    printf("%.2f, %.2f\n", v.x, v.y);
    printf("%.2f, %.2f\n", v.e[0], v.e[1]);
}

void Test_COM_MatrixMaths()
{
    printf("**************************\n");
    printf("TEST MATRIX MATHS\n");
    
    Vec3 a;     a.x = 3;    a.y = 5;    a.z = 2;
    Vec3 b;     b.x = 2;    b.y = 2;    b.z = 2;
    Vec3 c;
    printf("Vector a:\n");
    Test_COM_PrintVector3(a);
    printf("Vector b:\n");
    Test_COM_PrintVector3(b);
    printf("c = a * b:\n");
    c.x = a.x * b.x;
    c.y = a.y * b.y;
    c.z = a.z * b.z;
    Test_COM_PrintVector3(c);
    
    Vec2 v2a;
    // v2a.X = 3;
    // v2a.Y = 7;
    v2a = { 3, 7 };

    printf("V2: 3, 7:\n%0.f2, %0.2f\n", v2a.x, v2a.y);
    printf("V2 via array union:\n%0.2f, %0.2f\n", v2a.e[0], v2a.e[1]);



}

void Test_COM_MatrixMaths_ArrayTypes()
{
    printf("**************************\n");
    printf("TEST MATRIX MATHS - Array types\n");
    
    printf("\nSet to Identity Matrix:\n");
    f32 m0[16];
    M4x4_SetToIdentity(m0);
    Test_COM_PrintMatrix(m0);

    printf("\nSet to scale by 2:\n");
    M4x4_SetAsScale(m0, 2, 2, 2);
    Test_COM_PrintMatrix(m0);

    f32 v0[4];
    v0[VEC_X] = 3;
    v0[VEC_Y] = 2;
    v0[VEC_Z] = -4;
    v0[VEC_W] = 1;
    printf("Print vector v:\n");
    Test_COM_PrintVector4(v0);

    printf("\nMultiply v by m\n");
    Vec3_MultiplyByMatrix(m0, v0);
    Test_COM_PrintVector4(v0);

    printf("\nCreate Translation Matrix\n");
    M4x4_SetToIdentity(m0);
    M4x4_SetMove(m0, 9, -3, 2);
    Test_COM_PrintMatrix(m0);

    printf("Multiply v by m\n");
    Vec3_MultiplyByMatrix(m0, v0);
    Test_COM_PrintVector4(v0);

    f32 m1[16];
    M4x4_SetToIdentity(m1);
    printf("\nCopy M0 into M1\n");
    M4x4_Copy(m0, m1);
    Test_COM_PrintMatrix(m1);

    printf("\nMultiply matrices\n");
    printf("M0:\n");
    M4x4_SetAsScale(m0, 3, 2, 4);
    Test_COM_PrintMatrix(m0);
    printf("M1:\n");
    Test_COM_PrintMatrix(m1);
    M4x4_Multiply(m0, m1, m1);
    printf("Result:\n");
    Test_COM_PrintMatrix(m1);

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
