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

void Test_Com_Run()
{
    Test_Com_StringCompareTests();
    Test_Com_StringCopyTests();
    Test_Com_AsciToInt32_Series();
    Test_Com_StringLength();
}
